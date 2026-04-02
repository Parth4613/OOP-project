#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "Seat.h"
#include "Schedule.h"
#include "../utils/FileUtils.h"
using namespace std;

/*
 * Train: Represents a railway train with route, schedule, and seat management
 * Demonstrates: Encapsulation, Composition (Schedule, vector<Seat>), STL (vector)
 */
enum class TrainType { AC, SLEEPER };

class Train {
    string trainNumber;
    string name;
    string source;
    string destination;
    TrainType type;
    int totalSeats;
    double baseFare;
    Schedule schedule;
    vector<Seat> seats;

public:
    Train() : type(TrainType::SLEEPER), totalSeats(0), baseFare(0) {}

    Train(const string& tNo, const string& n, const string& src,
          const string& dest, TrainType t, int total, double fare, const Schedule& sched)
        : trainNumber(tNo), name(n), source(src), destination(dest),
          type(t), totalSeats(total), baseFare(fare), schedule(sched) {
        initializeSeats();
    }

    // Create seat objects with auto-assigned types
    void initializeSeats() {
        seats.clear();
        for (int i = 1; i <= totalSeats; i++)
            seats.push_back(Seat(i, Seat::getSeatTypeForNumber(i)));
    }

    // Count available (unbooked) seats
    int getAvailableSeatCount() const {
        int count = 0;
        for (const auto& s : seats) if (!s.isBooked()) count++;
        return count;
    }

    // Get the first available seat number, or -1 if none
    int getFirstAvailableSeat() const {
        for (const auto& s : seats) if (!s.isBooked()) return s.getSeatNumber();
        return -1;
    }

    // Book a specific seat for a user
    bool bookSeat(int seatNum, const string& username) {
        if (seatNum < 1 || seatNum > (int)seats.size()) return false;
        if (seats[seatNum - 1].isBooked()) return false;
        seats[seatNum - 1].book(username);
        return true;
    }

    // Cancel (free) a specific seat
    bool cancelSeat(int seatNum) {
        if (seatNum < 1 || seatNum > (int)seats.size()) return false;
        if (!seats[seatNum - 1].isBooked()) return false;
        seats[seatNum - 1].cancel();
        return true;
    }

    // --- Getters ---
    string getTrainNumber() const { return trainNumber; }
    string getName() const { return name; }
    string getSource() const { return source; }
    string getDestination() const { return destination; }
    TrainType getType() const { return type; }
    int getTotalSeats() const { return totalSeats; }
    double getBaseFare() const { return baseFare; }
    Schedule& getSchedule() { return schedule; }
    const Schedule& getSchedule() const { return schedule; }
    vector<Seat>& getSeats() { return seats; }
    const vector<Seat>& getSeats() const { return seats; }

    // --- Setters ---
    void setTotalSeats(int s) { totalSeats = s; }
    void setSchedule(const Schedule& s) { schedule = s; }

    // Type conversion helpers
    static string trainTypeToString(TrainType t) { return (t == TrainType::AC) ? "AC" : "Sleeper"; }
    static TrainType stringToTrainType(const string& s) { return (s == "AC") ? TrainType::AC : TrainType::SLEEPER; }

    // Serialize train data to a pipe-delimited string
    // Booked seats appended as: seatNum:username,seatNum:username,...
    string serialize() const {
        stringstream ss;
        ss << trainNumber << "|" << name << "|" << source << "|" << destination << "|"
           << trainTypeToString(type) << "|" << totalSeats << "|" << baseFare << "|"
           << schedule.getDepartureDate() << "|" << schedule.getDepartureTime() << "|"
           << schedule.getArrivalTime() << "|";
        bool first = true;
        for (const auto& seat : seats) {
            if (seat.isBooked()) {
                if (!first) ss << ",";
                ss << seat.getSeatNumber() << ":" << seat.getBookedBy();
                first = false;
            }
        }
        return ss.str();
    }

    // Deserialize from pipe-delimited string
    static Train deserialize(const string& data) {
        vector<string> parts = FileUtils::split(data, '|');
        if (parts.size() < 10)
            throw runtime_error("Invalid train data format");
        Schedule sched(parts[7], parts[8], parts[9]);
        Train train(parts[0], parts[1], parts[2], parts[3],
                    stringToTrainType(parts[4]), stoi(parts[5]),
                    stod(parts[6]), sched);
        // Restore booked seats
        if (parts.size() > 10 && !parts[10].empty()) {
            vector<string> bookings = FileUtils::split(parts[10], ',');
            for (const auto& b : bookings) {
                vector<string> info = FileUtils::split(b, ':');
                if (info.size() == 2)
                    train.bookSeat(stoi(info[0]), info[1]);
            }
        }
        return train;
    }

    // Pretty-print train information to console
    void displayInfo() const {
        cout << "  +-----------------------------------------------+\n";
        cout << "  | Train No  : " << left << setw(33) << trainNumber << "|\n";
        cout << "  | Name      : " << left << setw(33) << name << "|\n";
        cout << "  | Route     : " << left << setw(33) << (source + " -> " + destination) << "|\n";
        cout << "  | Type      : " << left << setw(33) << trainTypeToString(type) << "|\n";
        cout << "  | Date      : " << left << setw(33) << schedule.getDepartureDate() << "|\n";
        cout << "  | Departure : " << left << setw(33) << schedule.getDepartureTime() << "|\n";
        cout << "  | Arrival   : " << left << setw(33) << schedule.getArrivalTime() << "|\n";
        cout << "  | Base Fare : Rs. " << left << setw(28) << fixed
             << setprecision(2) << baseFare << "|\n";
        cout << "  | Available : " << left << setw(33)
             << (to_string(getAvailableSeatCount()) + " / " + to_string(totalSeats)) << "|\n";
        cout << "  +-----------------------------------------------+\n";
    }
};
