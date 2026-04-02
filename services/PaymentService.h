#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include "../models/Train.h"
using namespace std;

/*
 * Polymorphic Pricing System using FareCalculator hierarchy
 * Demonstrates: Abstraction (pure virtual), Polymorphism, Inheritance,
 *               Virtual Destructor, Dynamic Memory (new/delete)
 */

// Abstract base class for fare calculation
class FareCalculator {
public:
    virtual double calculate(double baseFare) const = 0;
    virtual string getTypeName() const = 0;
    virtual ~FareCalculator() {}   // Virtual destructor
};

// Sleeper class fare: 1.0x multiplier
class SleeperFare : public FareCalculator {
public:
    double calculate(double baseFare) const override { return baseFare * 1.0; }
    string getTypeName() const override { return "Sleeper"; }
};

// AC class fare: 1.8x multiplier
class ACFare : public FareCalculator {
public:
    double calculate(double baseFare) const override { return baseFare * 1.8; }
    string getTypeName() const override { return "AC"; }
};

// Tatkal surcharge: 1.3x on top of base fare type
class TatkalFare : public FareCalculator {
    FareCalculator* baseCalc;   // Decorator pattern
public:
    TatkalFare(FareCalculator* base) : baseCalc(base) {}
    double calculate(double baseFare) const override {
        return baseCalc->calculate(baseFare) * 1.3;
    }
    string getTypeName() const override { return "Tatkal " + baseCalc->getTypeName(); }
    ~TatkalFare() override { delete baseCalc; }  // Clean up owned pointer
};

/*
 * PaymentService: Fare calculation and refund processing
 * Demonstrates: Polymorphism, Dynamic Memory, Encapsulation
 */
class PaymentService {
public:
    // Create the appropriate fare calculator based on train type
    // Caller owns the returned pointer (must delete)
    static FareCalculator* createCalculator(TrainType type, bool isTatkal = false) {
        FareCalculator* calc = nullptr;
        if (type == TrainType::AC)
            calc = new ACFare();
        else
            calc = new SleeperFare();

        if (isTatkal)
            calc = new TatkalFare(calc);  // Decorator wraps the base

        return calc;
    }

    // Calculate fare using polymorphic dispatch
    static double calculateFare(double baseFare, TrainType type, bool isTatkal = false) {
        FareCalculator* calc = createCalculator(type, isTatkal);
        double fare = calc->calculate(baseFare);
        delete calc;   // Clean up dynamic memory
        return fare;
    }

    /*
     * Calculate refund based on time before departure
     * > 24 hrs  -> 90% refund
     * 12-24 hrs -> 70% refund
     * < 12 hrs  -> 50% refund
     */
    static double calculateRefund(double fare, time_t bookingTime,
                                   const string& depDate, const string& depTime) {
        time_t departure = TimeUtils::parseDateTime(depDate, depTime);
        time_t now = TimeUtils::getCurrentTime();
        double hoursLeft = TimeUtils::getHoursDifference(now, departure);

        double refundPercent;
        if (hoursLeft > 24.0)
            refundPercent = 0.90;
        else if (hoursLeft > 12.0)
            refundPercent = 0.70;
        else
            refundPercent = 0.50;

        double refund = fare * refundPercent;

        cout << "\n  Refund Calculation:\n";
        cout << "  Hours before departure : " << fixed << setprecision(1) << hoursLeft << "\n";
        cout << "  Refund percentage      : " << (int)(refundPercent * 100) << "%\n";
        cout << "  Original fare          : Rs. " << setprecision(2) << fare << "\n";
        cout << "  Refund amount          : Rs. " << setprecision(2) << refund << "\n";

        return refund;
    }

    // Display payment method selection and get user choice
    static string selectPaymentMethod() {
        cout << "\n  Select Payment Method:\n";
        cout << "  1. CASH\n";
        cout << "  2. CARD\n";
        cout << "  3. UPI\n";
        int choice = InputValidator::getValidInteger("  Enter choice: ", 1, 3);
        switch (choice) {
            case 1: return "CASH";
            case 2: return "CARD";
            case 3: return "UPI";
            default: return "CASH";
        }
    }
};
