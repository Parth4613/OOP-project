#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <functional>
#include "../models/Ticket.h"
#include "../models/Payment.h"
#include "../models/Train.h"
using namespace std;

/*
 * ReportGenerator: Template-based report generator
 * Demonstrates: Templates, Generic Programming, STL
 */
template <typename T>
class ReportGenerator {
    string title;
    vector<T> records;

public:
    ReportGenerator(const string& t, const vector<T>& r)
        : title(t), records(r) {}

    // Generate a formatted report to console
    void generate() const {
        cout << "\n  ====================================================\n";
        cout << "    " << title << "\n";
        cout << "  ====================================================\n";
        cout << "  Total Records: " << records.size() << "\n";
        cout << "  ----------------------------------------------------\n";
        for (size_t i = 0; i < records.size(); i++) {
            cout << "  [" << (i + 1) << "] ";
            printRecord(records[i]);
            cout << "  ----------------------------------------------------\n";
        }
    }

    // Aggregate a numeric field using a lambda extractor
    double aggregate(function<double(const T&)> extractor) const {
        double total = 0;
        for (const auto& r : records) total += extractor(r);
        return total;
    }

    // Find max element using a custom comparator
    const T* findMax(function<bool(const T&, const T&)> comparator) const {
        if (records.empty()) return nullptr;
        const T* maxElem = &records[0];
        for (size_t i = 1; i < records.size(); i++) {
            if (comparator(records[i], *maxElem)) maxElem = &records[i];
        }
        return maxElem;
    }

    // Count records matching a predicate
    int count(function<bool(const T&)> predicate) const {
        int c = 0;
        for (const auto& r : records) if (predicate(r)) c++;
        return c;
    }

private:
    // Specialization for printing different types
    void printRecord(const BookingRecord& r) const {
        cout << r.getTicketId() << " | " << r.getPassengerName()
                  << " | Train: " << r.getTrainNumber()
                  << " | Seat: " << r.getSeatNumber()
                  << " | Rs. " << fixed << setprecision(2) << r.getFare()
                  << " | " << r.getStatus() << "\n";
    }

    void printRecord(const Train& t) const {
        cout << t.getTrainNumber() << " | " << t.getName()
                  << " | " << t.getSource() << " -> " << t.getDestination()
                  << " | " << Train::trainTypeToString(t.getType())
                  << " | Seats: " << t.getAvailableSeatCount() << "/" << t.getTotalSeats()
                  << " | Rs. " << fixed << setprecision(2) << t.getBaseFare() << "\n";
    }

    // Fallback for any other type
    template <typename U>
    void printRecord(const U&) const {
        cout << "[Record]\n";
    }
};

/*
 * ReportService: Uses ReportGenerator to produce system reports
 * Demonstrates: Templates, Generic Programming
 */
class ReportService {
public:
    // Generate a report of all bookings
    static void allBookingsReport(const vector<BookingRecord>& bookings) {
        ReportGenerator<BookingRecord> report("ALL BOOKINGS REPORT", bookings);
        report.generate();

        int confirmed = report.count([](const BookingRecord& r) { return r.getStatus() == "CONFIRMED"; });
        int cancelled = report.count([](const BookingRecord& r) { return r.getStatus() == "CANCELLED"; });
        double totalRevenue = report.aggregate([](const BookingRecord& r) {
            return r.getStatus() == "CONFIRMED" ? r.getFare() : 0.0;
        });

        cout << "\n  Summary:\n";
        cout << "  Confirmed: " << confirmed << " | Cancelled: " << cancelled << "\n";
        cout << "  Total Revenue: Rs. " << fixed << setprecision(2) << totalRevenue << "\n";
    }

    // Generate a revenue report across all trains
    static void revenueReport(const vector<BookingRecord>& bookings,
                               const vector<Train>& trains) {
        cout << "\n  ====================================================\n";
        cout << "    REVENUE REPORT\n";
        cout << "  ====================================================\n";

        double totalRevenue = 0;
        for (const auto& train : trains) {
            double trainRevenue = 0;
            int bookingCount = 0;
            for (const auto& b : bookings) {
                if (b.getTrainNumber() == train.getTrainNumber() && b.getStatus() == "CONFIRMED") {
                    trainRevenue += b.getFare();
                    bookingCount++;
                }
            }
            cout << "  Train " << train.getTrainNumber() << " (" << train.getName() << ")\n";
            cout << "    Bookings: " << bookingCount
                      << " | Revenue: Rs. " << fixed << setprecision(2)
                      << trainRevenue << "\n";
            totalRevenue += trainRevenue;
        }
        cout << "  ----------------------------------------------------\n";
        cout << "  TOTAL SYSTEM REVENUE: Rs. " << fixed << setprecision(2)
                  << totalRevenue << "\n";
        cout << "  ====================================================\n";
    }

    // Generate train occupancy report
    static void occupancyReport(const vector<Train>& trains) {
        ReportGenerator<Train> report("TRAIN OCCUPANCY REPORT", trains);
        report.generate();

        for (const auto& t : trains) {
            int booked = t.getTotalSeats() - t.getAvailableSeatCount();
            double occupancy = (t.getTotalSeats() > 0)
                ? (booked * 100.0 / t.getTotalSeats()) : 0.0;
            cout << "  Train " << t.getTrainNumber()
                      << ": " << booked << "/" << t.getTotalSeats()
                      << " (" << fixed << setprecision(1) << occupancy << "% occupied)\n";
        }
    }
};
