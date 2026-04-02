#pragma once
#include "User.h"
#include "Train.h"
#include "../services/TrainService.h"
#include "../services/BookingService.h"
#include "../services/LogService.h"
#include "../services/ReportService.h"
#include "../utils/InputValidator.h"
using namespace std;

class Admin : public User {
    TrainService& trainService;
    BookingService& bookingService;
    LogService& logger;

public:
    Admin(const string& u, const string& p,
          TrainService& ts, BookingService& bs, LogService& log)
        : User(u, p, "admin"), trainService(ts), bookingService(bs), logger(log) {}

    void login() override {
        cout << "\n  ============================================\n";
        cout << "    Welcome, Admin: " << username << "\n";
        cout << "  ============================================\n";
    }

    void displayMenu() override {
        int choice;
        do {
            cout << "\n  ============================================\n";
            cout << "              ADMIN PANEL\n";
            cout << "  ============================================\n";
            cout << "  1. Add Train\n";
            cout << "  2. Delete Train\n";
            cout << "  3. Modify Train Schedule\n";
            cout << "  4. Update Seat Capacity\n";
            cout << "  5. View All Trains\n";
            cout << "  6. View All Bookings\n";
            cout << "  7. Revenue Report\n";
            cout << "  8. Train Occupancy Report\n";
            cout << "  9. View System Logs\n";
            cout << "  0. Logout\n";
            cout << "  ============================================\n";
            choice = InputValidator::getValidInteger("  Enter choice: ", 0, 9);
            try {
                switch (choice) {
                    case 1: addTrain(); break;
                    case 2: deleteTrain(); break;
                    case 3: modifySchedule(); break;
                    case 4: updateCapacity(); break;
                    case 5: viewAllTrains(); break;
                    case 6: viewAllBookings(); break;
                    case 7: revenueReport(); break;
                    case 8: occupancyReport(); break;
                    case 9: logger.displayLogs(); break;
                    case 0: std::cout << "  Logging out...\n"; break;
                }
            } catch (const std::exception& e) {
                cout << "  [ERROR] " << e.what() << "\n";
            }
        } while (choice != 0);
    }

    ~Admin() override = default;

private:
    void addTrain() {
        cout << "\n  --- Add New Train ---\n";
        string trainNo = InputValidator::getValidString("  Train Number: ");
        string name = InputValidator::getValidString("  Train Name: ");
        string source = InputValidator::getValidString("  Source Station: ");
        string dest = InputValidator::getValidString("  Destination Station: ");
        cout << "  Train Type (1=AC, 2=Sleeper): ";
        int typeChoice = InputValidator::getValidInteger("", 1, 2);
        TrainType type = (typeChoice == 1) ? TrainType::AC : TrainType::SLEEPER;
        int seats = InputValidator::getValidInteger("  Total Seats: ", 1, 500);
        double fare = InputValidator::getValidDouble("  Base Fare (Rs.): ", 1.0, 50000.0);
        string depDate = InputValidator::getValidDate("  Departure Date (YYYY-MM-DD): ");
        string depTime = InputValidator::getValidTime("  Departure Time (HH:MM): ");
        string arrTime = InputValidator::getValidTime("  Arrival Time (HH:MM): ");
        Schedule sched(depDate, depTime, arrTime);
        Train train(trainNo, name, source, dest, type, seats, fare, sched);
        if (trainService.addTrain(train)) {
            cout << "  [+] Train " << trainNo << " added successfully!\n";
            logger.log("ADMIN", "Train " + trainNo + " added by " + username);
        } else {
            cout << "  [!] Train number already exists!\n";
        }
    }

    void deleteTrain() {
        cout << "\n  --- Delete Train ---\n";
        string trainNo = InputValidator::getValidString("  Enter Train Number to delete: ");
        if (trainService.deleteTrain(trainNo)) {
            cout << "  [+] Train " << trainNo << " deleted successfully!\n";
            logger.log("ADMIN", "Train " + trainNo + " deleted by " + username);
        } else {
            cout << "  [!] Train not found!\n";
        }
    }

    void modifySchedule() {
        cout << "\n  --- Modify Train Schedule ---\n";
        string trainNo = InputValidator::getValidString("  Enter Train Number: ");
        try {
            Train train = trainService.getTrainByNumber(trainNo);
            cout << "  Current: Date=" << train.getSchedule().getDepartureDate()
                      << " Dep=" << train.getSchedule().getDepartureTime()
                      << " Arr=" << train.getSchedule().getArrivalTime() << "\n";
            string newDate = InputValidator::getValidDate("  New Departure Date (YYYY-MM-DD): ");
            string newDepTime = InputValidator::getValidTime("  New Departure Time (HH:MM): ");
            string newArrTime = InputValidator::getValidTime("  New Arrival Time (HH:MM): ");
            train.setSchedule(Schedule(newDate, newDepTime, newArrTime));
            trainService.updateTrain(train);
            cout << "  [+] Schedule updated successfully!\n";
            logger.log("ADMIN", "Schedule modified for train " + trainNo);
        } catch (const std::exception& e) {
            cout << "  [!] " << e.what() << "\n";
        }
    }

    void updateCapacity() {
        cout << "\n  --- Update Seat Capacity ---\n";
        string trainNo = InputValidator::getValidString("  Enter Train Number: ");
        try {
            Train train = trainService.getTrainByNumber(trainNo);
            cout << "  Current capacity: " << train.getTotalSeats() << " seats\n";
            int newCap = InputValidator::getValidInteger("  New capacity: ", 1, 500);
            train.setTotalSeats(newCap);
            train.initializeSeats();
            trainService.updateTrain(train);
            cout << "  [+] Capacity updated to " << newCap << " seats!\n";
            logger.log("ADMIN", "Capacity updated for train " + trainNo);
        } catch (const std::exception& e) {
            cout << "  [!] " << e.what() << "\n";
        }
    }

    void viewAllTrains() {
        auto trains = trainService.getAllTrains();
        if (trains.empty()) { std::cout << "\n  No trains in the system.\n"; return; }
        cout << "\n  --- All Trains (" << trains.size() << ") ---\n";
        for (const auto& t : trains) t.displayInfo();
    }

    void viewAllBookings() {
        auto bookings = bookingService.getAllBookings();
        ReportService::allBookingsReport(bookings);
    }

    void revenueReport() {
        auto bookings = bookingService.getAllBookings();
        auto trains = trainService.getAllTrains();
        ReportService::revenueReport(bookings, trains);
    }

    void occupancyReport() {
        auto trains = trainService.getAllTrains();
        ReportService::occupancyReport(trains);
    }
};
