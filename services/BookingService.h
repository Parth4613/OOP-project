#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "../models/Ticket.h"
#include "../models/Payment.h"
#include "../models/Train.h"
#include "../utils/IDGenerator.h"
#include "../utils/TimeUtils.h"
#include "../utils/InputValidator.h"
#include "TrainService.h"
#include "PaymentService.h"
#include "LogService.h"
using namespace std;

/*
 * BookingService: Handles ticket booking, cancellation, waitlist,
 *                 and automatic seat allocation
 * Demonstrates: Encapsulation, STL (vector, map), Exception Handling
 */
class BookingService {
    TrainService& trainService;
    LogService& logger;
    string ticketsFile;
    string waitlistFile;

public:
    BookingService(TrainService& ts, LogService& log,
                   const string& tf = "data/tickets.txt",
                   const string& wf = "data/waitlist.txt")
        : trainService(ts), logger(log), ticketsFile(tf), waitlistFile(wf) {
        IDGenerator::loadUsedIds(ticketsFile);  // Prevent duplicate IDs
    }

    // ======================== TICKET LOADING ========================

    // Load all booking records from file
    vector<BookingRecord> loadBookings() const {
        vector<BookingRecord> records;
        vector<string> lines = FileUtils::readLines(ticketsFile);
        for (const auto& line : lines) {
            try { records.push_back(BookingRecord::deserializeFull(line)); }
            catch (...) { /* Skip malformed entries */ }
        }
        return records;
    }

    // Save all booking records to file
    void saveBookings(const vector<BookingRecord>& records) const {
        vector<string> lines;
        for (const auto& r : records) lines.push_back(r.serializeFull());
        FileUtils::writeLines(ticketsFile, lines);
    }

    // ======================== BOOKING ========================

    // Book a ticket: auto-allocate seat, calculate fare, process payment
    BookingRecord bookTicket(const string& username, const string& trainNumber,
                             bool isTatkal = false) {
        Train train = trainService.getTrainByNumber(trainNumber);

        // Booking time constraint: check if departure is in the past
        if (TimeUtils::isPast(train.getSchedule().getDepartureDate(),
                              train.getSchedule().getDepartureTime())) {
            throw BookingException("Cannot book: Train departure time has already passed!");
        }

        // Check seat availability
        int seatNum = train.getFirstAvailableSeat();
        if (seatNum == -1) {
            // No seats available — offer waitlist
            cout << "\n  [!] No seats available on this train.\n";
            cout << "  Would you like to be added to the waitlist? (1=Yes, 2=No): ";
            int choice = InputValidator::getValidInteger("", 1, 2);
            if (choice == 1) {
                addToWaitlist(username, trainNumber);
                cout << "  [+] You have been added to the waitlist.\n";
            }
            throw BookingException("No seats available. Added to waitlist if requested.");
        }

        // Calculate fare using polymorphic pricing
        double fare = PaymentService::calculateFare(train.getBaseFare(), train.getType(), isTatkal);

        // Display fare and confirm booking
        cout << "\n  Fare for this booking: Rs. " << fixed << setprecision(2) << fare << "\n";
        if (isTatkal) cout << "  (Tatkal surcharge applied)\n";

        // Process payment
        string paymentMethod = PaymentService::selectPaymentMethod();

        // Allocate seat on the train
        train.bookSeat(seatNum, username);
        trainService.updateTrain(train);

        // Generate unique ticket ID
        string ticketId = IDGenerator::generateTicketId();
        time_t now = TimeUtils::getCurrentTime();

        // Create ticket and payment records
        Ticket ticket(ticketId, username, trainNumber, train.getName(),
                      seatNum, fare, "CONFIRMED", now);
        Payment payment(ticketId, fare, paymentMethod, "COMPLETED", 0.0);
        BookingRecord record(ticket, payment);

        // Save to file
        vector<BookingRecord> records = loadBookings();
        records.push_back(record);
        saveBookings(records);

        logger.log("BOOKING", "Ticket " + ticketId + " booked by " + username
                   + " on train " + trainNumber + " seat " + to_string(seatNum));

        return record;
    }

    // ======================== CANCELLATION ========================

    // Cancel a ticket: process refund, free seat, promote from waitlist
    double cancelTicket(const string& ticketId, const string& username) {
        vector<BookingRecord> records = loadBookings();
        bool found = false;
        double refundAmount = 0;
        string trainNumber;

        for (auto& record : records) {
            if (record.getTicketId() == ticketId && record.getPassengerName() == username) {
                if (record.getStatus() == "CANCELLED") {
                    throw BookingException("Ticket is already cancelled!");
                }

                found = true;
                trainNumber = record.getTrainNumber();

                // Get train for departure time (for refund calculation)
                try {
                    Train train = trainService.getTrainByNumber(trainNumber);

                    // Calculate refund
                    refundAmount = PaymentService::calculateRefund(
                        record.getFare(), record.getBookingTime(),
                        train.getSchedule().getDepartureDate(),
                        train.getSchedule().getDepartureTime());

                    // Free the seat on the train
                    train.cancelSeat(record.getSeatNumber());
                    trainService.updateTrain(train);
                } catch (const BookingException&) {
                    // Train might have been deleted — give 50% refund
                    refundAmount = record.getFare() * 0.50;
                }

                // Update ticket status
                record.setStatus("CANCELLED");
                record.setPaymentStatus("REFUNDED");
                record.setRefundAmount(refundAmount);
                break;
            }
        }

        if (!found) throw BookingException("Ticket not found or unauthorized!");

        saveBookings(records);
        logger.log("CANCEL", "Ticket " + ticketId + " cancelled by " + username
                   + ". Refund: Rs. " + to_string(refundAmount));

        // Promote from waitlist if applicable
        if (!trainNumber.empty()) {
            promoteFromWaitlist(trainNumber);
        }

        return refundAmount;
    }

    // ======================== WAITLIST ========================

    // Add a passenger to the waitlist for a train
    void addToWaitlist(const string& username, const string& trainNumber) {
        string entry = trainNumber + "|" + username + "|"
                          + to_string(TimeUtils::getCurrentTime()) + "|WAITING";
        FileUtils::appendLine(waitlistFile, entry);
        logger.log("WAITLIST", username + " added to waitlist for train " + trainNumber);
    }

    // Promote the first waitlisted passenger when a seat becomes available
    void promoteFromWaitlist(const string& trainNumber) {
        vector<string> lines = FileUtils::readLines(waitlistFile);
        vector<string> remaining;
        bool promoted = false;

        for (const auto& line : lines) {
            vector<string> parts = FileUtils::split(line, '|');
            if (!promoted && parts.size() >= 4 && parts[0] == trainNumber && parts[3] == "WAITING") {
                // Try to book for this waitlisted passenger
                try {
                    Train train = trainService.getTrainByNumber(trainNumber);
                    int seatNum = train.getFirstAvailableSeat();
                    if (seatNum != -1) {
                        // Auto-book for waitlisted passenger
                        string user = parts[1];
                        double fare = PaymentService::calculateFare(train.getBaseFare(), train.getType());
                        train.bookSeat(seatNum, user);
                        trainService.updateTrain(train);

                        string ticketId = IDGenerator::generateTicketId();
                        time_t now = TimeUtils::getCurrentTime();
                        Ticket ticket(ticketId, user, trainNumber, train.getName(),
                                      seatNum, fare, "CONFIRMED", now);
                        Payment payment(ticketId, fare, "AUTO", "COMPLETED", 0.0);
                        BookingRecord record(ticket, payment);

                        vector<BookingRecord> records = loadBookings();
                        records.push_back(record);
                        saveBookings(records);

                        logger.log("WAITLIST", "Promoted " + user + " from waitlist. Ticket: " + ticketId);
                        cout << "\n  [+] Waitlisted passenger '" << user
                                  << "' has been auto-booked! Ticket: " << ticketId << "\n";
                        promoted = true;
                        continue;  // Don't add this entry back to waitlist
                    }
                } catch (...) { /* Could not promote, keep in waitlist */ }
            }
            remaining.push_back(line);
        }
        FileUtils::writeLines(waitlistFile, remaining);
    }

    // ======================== HISTORY & QUERIES ========================

    // Get booking history for a specific user
    vector<BookingRecord> getBookingHistory(const string& username) const {
        vector<BookingRecord> all = loadBookings();
        vector<BookingRecord> userBookings;
        for (const auto& r : all) {
            if (r.getPassengerName() == username) userBookings.push_back(r);
        }
        return userBookings;
    }

    // Get all bookings (for admin)
    vector<BookingRecord> getAllBookings() const { return loadBookings(); }

    // Get total revenue from confirmed tickets for a specific train
    double getTrainRevenue(const string& trainNumber) const {
        double total = 0;
        vector<BookingRecord> records = loadBookings();
        for (const auto& r : records) {
            if (r.getTrainNumber() == trainNumber && r.getStatus() == "CONFIRMED")
                total += r.getFare();
        }
        return total;
    }

    // Get total system revenue
    double getTotalRevenue() const {
        double total = 0;
        vector<BookingRecord> records = loadBookings();
        for (const auto& r : records) {
            if (r.getStatus() == "CONFIRMED") total += r.getFare();
        }
        return total;
    }
};
