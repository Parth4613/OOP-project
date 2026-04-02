#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include "../utils/FileUtils.h"
#include "../utils/TimeUtils.h"
using namespace std;

class Ticket {
protected:
    string ticketId;
    string passengerName;
    string trainNumber;
    string trainName;
    int seatNumber;
    double fare;
    string status;     // CONFIRMED, CANCELLED, WAITLISTED
    time_t bookingTime;

public:
    Ticket() : seatNumber(0), fare(0), bookingTime(0) {}

    Ticket(const string& id, const string& pName, const string& trainNo,
           const string& tName, int seat, double f, const string& st, time_t bt)
        : ticketId(id), passengerName(pName), trainNumber(trainNo), trainName(tName),
          seatNumber(seat), fare(f), status(st), bookingTime(bt) {}

    // --- Getters ---
    string getTicketId() const { return ticketId; }
    string getPassengerName() const { return passengerName; }
    string getTrainNumber() const { return trainNumber; }
    string getTrainName() const { return trainName; }
    int getSeatNumber() const { return seatNumber; }
    double getFare() const { return fare; }
    string getStatus() const { return status; }
    time_t getBookingTime() const { return bookingTime; }

    // --- Setters ---
    void setStatus(const    string& s) { status = s; }
    void setSeatNumber(int s) { seatNumber = s; }

    // Serialize to pipe-delimited string for file storage
    string serialize() const {
        stringstream ss;
        ss << ticketId << "|" << passengerName << "|" << trainNumber << "|"
           << trainName << "|" << seatNumber << "|" << fare << "|"
           << status << "|" << bookingTime;
        return ss.str();
    }

    // Deserialize from pipe-delimited string
    static Ticket deserialize(const string& data) {
        vector<string> parts = FileUtils::split(data, '|');
        if (parts.size() < 8)
            throw runtime_error("Invalid ticket data");
        return Ticket(parts[0], parts[1], parts[2], parts[3],
                      stoi(parts[4]), stod(parts[5]),
                      parts[6], stol(parts[7]));
    }

    /*
     * Operator<< Overloading: Pretty-print the ticket in a formatted box
     * Demonstrates: Operator Overloading, friend function
     */
    friend std::ostream& operator<<(std::ostream& os, const Ticket& t) {
        os << "\n  +================================================+\n";
        os << "  |            RAILWAY TICKET                       |\n";
        os << "  +================================================+\n";
        os << "  |  Ticket ID  : " << std::left << std::setw(32) << t.ticketId << "|\n";
        os << "  |  Passenger  : " << std::left << std::setw(32) << t.passengerName << "|\n";
        os << "  |  Train No   : " << std::left << std::setw(32) << t.trainNumber << "|\n";
        os << "  |  Train Name : " << std::left << std::setw(32) << t.trainName << "|\n";
        os << "  |  Seat No    : " << std::left << std::setw(32) << t.seatNumber << "|\n";
        os << "  |  Fare       : Rs. " << std::left << std::setw(28)
           << std::fixed << std::setprecision(2) << t.fare << "|\n";
        os << "  |  Status     : " << std::left << std::setw(32) << t.status << "|\n";
        os << "  |  Booked On  : " << std::left << std::setw(32)
           << TimeUtils::formatTime(t.bookingTime) << "|\n";
        os << "  +================================================+\n";
        return os;
    }
};
