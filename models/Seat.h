#pragma once
#include <string>
using namespace std;
enum class SeatType { LOWER, MIDDLE, UPPER, SIDE_LOWER, SIDE_UPPER };

class Seat {
    int seatNumber;
    SeatType type;
    bool booked;
    string bookedBy;

public:
    Seat() : seatNumber(0), type(SeatType::LOWER), booked(false) {}
    Seat(int num, SeatType t) : seatNumber(num), type(t), booked(false) {}

    int getSeatNumber() const { return seatNumber; }
    SeatType getType() const { return type; }
    bool isBooked() const { return booked; }
    string getBookedBy() const { return bookedBy; }

    void book(const string& username) { booked = true; bookedBy = username; }
    void cancel() { booked = false; bookedBy = ""; }

    static string seatTypeToString(SeatType t) {
        switch (t) {
            case SeatType::LOWER:      return "Lower";
            case SeatType::MIDDLE:     return "Middle";
            case SeatType::UPPER:      return "Upper";
            case SeatType::SIDE_LOWER: return "Side-Lower";
            case SeatType::SIDE_UPPER: return "Side-Upper";
            default: return "Unknown";
        }
    }

    static SeatType stringToSeatType(const string& s) {
        if (s == "Lower")      return SeatType::LOWER;
        if (s == "Middle")     return SeatType::MIDDLE;
        if (s == "Upper")      return SeatType::UPPER;
        if (s == "Side-Lower") return SeatType::SIDE_LOWER;
        if (s == "Side-Upper") return SeatType::SIDE_UPPER;
        return SeatType::LOWER;
    }

    static SeatType getSeatTypeForNumber(int num) {
        int pos = (num - 1) % 8;
        switch (pos) {
            case 0: case 3: return SeatType::LOWER;
            case 1: case 4: return SeatType::MIDDLE;
            case 2: case 5: return SeatType::UPPER;
            case 6: return SeatType::SIDE_LOWER;
            case 7: return SeatType::SIDE_UPPER;
            default: return SeatType::LOWER;
        }
    }
};
