#pragma once
#include <string>
#include <sstream>
#include <stdexcept>
#include "../utils/FileUtils.h"
using namespace std;
class Payment {
protected:
    string paymentId;
    double amount;
    string method;          
    string paymentStatus;   
    double refundAmount;

public:
    Payment() : amount(0), refundAmount(0) {}

    Payment(const string& id, double amt, const string& m,
            const string& st, double refund = 0.0)
        : paymentId(id), amount(amt), method(m), paymentStatus(st), refundAmount(refund) {}

    string getPaymentId() const { return paymentId; }
    double getAmount() const { return amount; }
    string getMethod() const { return method; }
    string getPaymentStatus() const { return paymentStatus; }
    double getRefundAmount() const { return refundAmount; }

    void setPaymentStatus(const string& s) { paymentStatus = s; }
    void setRefundAmount(double amt) { refundAmount = amt; }

    string serialize() const {
        stringstream ss;
        ss << paymentId << "|" << amount << "|" << method << "|"
           << paymentStatus << "|" << refundAmount;
        return ss.str();
    }

    
    static Payment deserialize(const string& data) {
        vector<string> parts = FileUtils::split(data, '|');
        if (parts.size() < 5)
            throw runtime_error("Invalid payment data");
        return Payment(parts[0], stod(parts[1]), parts[2],
                       parts[3], stod(parts[4]));
    }
};

class BookingRecord : public Ticket, public Payment {
public:
    BookingRecord() = default;

    BookingRecord(const Ticket& t, const Payment& p)
        : Ticket(t), Payment(p) {}

    string serializeFull() const {
        return Ticket::serialize() + "|" + Payment::serialize();
    }
    static BookingRecord deserializeFull(const string& data) {
        vector<string> parts = FileUtils::split(data, '|');
        if (parts.size() < 13)
            throw runtime_error("Invalid booking record data");

        string ticketData = parts[0];
        for (int i = 1; i < 8; i++) ticketData += "|" + parts[i];
        string paymentData = parts[8];
        for (int i = 9; i < 13; i++) paymentData += "|" + parts[i];

        Ticket t = Ticket::deserialize(ticketData);
        Payment p = Payment::deserialize(paymentData);
        return BookingRecord(t, p);
    }

    void displayFull() const {
        cout << *static_cast<const Ticket*>(this);
        cout << "  |  Payment    : " << left << setw(32) << method << "|\n";
        cout << "  |  Pay Status : " << left << setw(32) << paymentStatus << "|\n";
        if (refundAmount > 0) {
            ostringstream oss;
            oss << "Rs. " << fixed << setprecision(2) << refundAmount;
            cout << "  |  Refund     : " << left << setw(32) << oss.str() << "|\n";
        }
        cout << "  +================================================+\n";
    }
};
