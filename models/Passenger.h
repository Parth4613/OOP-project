#pragma once
#include "User.h"
#include "Train.h"
#include "../services/TrainService.h"
#include "../services/BookingService.h"
#include "../services/LogService.h"
#include "../utils/InputValidator.h"
using namespace std;
class Passenger : public User {
    TrainService& trainService;
    BookingService& bookingService;
    LogService& logger;

public:
    Passenger(const std::string& u, const std::string& p,
              TrainService& ts, BookingService& bs, LogService& log)
        : User(u, p, "passenger"), trainService(ts), bookingService(bs), logger(log) {}

    void login() override {
        cout << "\n  ============================================\n";
        cout << "    Welcome, Passenger: " << username << "\n";
        cout << "  ============================================\n";
    }

    void displayMenu() override {
        int choice;
        do {
            cout << "\n  ============================================\n";
            cout << "        PASSENGER PANEL - " << username << "\n";
            cout << "  ============================================\n";
            cout << "  1. Search Trains\n";
            cout << "  2. Book Ticket\n";
            cout << "  3. Book Tatkal Ticket\n";
            cout << "  4. Cancel Ticket\n";
            cout << "  5. View Booking History\n";
            cout << "  6. Advanced Search (Filters)\n";
            cout << "  0. Logout\n";
            cout << "  ============================================\n";

            choice = InputValidator::getValidInteger("  Enter choice: ", 0, 6);

            try {
                switch (choice) {
                    case 1: searchTrains(); break;
                    case 2: bookTicket(false); break;
                    case 3: bookTicket(true); break;
                    case 4: cancelTicket(); break;
                    case 5: viewHistory(); break;
                    case 6: advancedSearch(); break;
                    case 0: std::cout << "  Logging out...\n"; break;
                }
            } catch (const std::exception& e) {
                cout << "  [ERROR] " << e.what() << "\n";
            }
        } while (choice != 0);
    }

    ~Passenger() override = default;

private:
    void searchTrains() {
        cout << "\n  --- Search Trains ---\n";
        string source = InputValidator::getValidString("  Source Station: ");
        string dest = InputValidator::getValidString("  Destination Station: ");
        string date = InputValidator::getValidDate("  Travel Date (YYYY-MM-DD): ");

        auto results = trainService.searchTrains(source, dest, date);
        if (results.empty()) {
            cout << "  No trains found for this route and date.\n";
            return;
        }
        cout << "\n  Found " << results.size() << " train(s):\n";
        for (const auto& t : results) t.displayInfo();
    }

    void bookTicket(bool isTatkal) {
        cout << "\n  --- " << (isTatkal ? "Tatkal " : "") << "Book Ticket ---\n";
        string trainNo = InputValidator::getValidString("  Enter Train Number: ");

        try {
            BookingRecord record = bookingService.bookTicket(username, trainNo, isTatkal);
            cout << "\n  [+] Ticket booked successfully!\n";
            cout << static_cast<const Ticket&>(record);  // Use operator<< overload
        } catch (const BookingException& e) {
            cout << "  [!] " << e.what() << "\n";
        }
    }

    void cancelTicket() {
        cout << "\n  --- Cancel Ticket ---\n";

        auto history = bookingService.getBookingHistory(username);
        vector<BookingRecord> active;
        for (const auto& r : history) {
            if (r.getStatus() == "CONFIRMED") active.push_back(r);
        }
        if (active.empty()) {
            cout << "  No active bookings to cancel.\n";
            return;
        }

        cout << "\n  Your active bookings:\n";
        for (size_t i = 0; i < active.size(); i++) {
            cout << "  [" << (i + 1) << "] " << active[i].getTicketId()
                      << " | Train: " << active[i].getTrainNumber()
                      << " | Seat: " << active[i].getSeatNumber() << "\n";
        }

        string ticketId = InputValidator::getValidString("  Enter Ticket ID to cancel: ");
        double refund = bookingService.cancelTicket(ticketId, username);
        cout << "\n  [+] Ticket cancelled. Refund: Rs. " << std::fixed
                  << std::setprecision(2) << refund << "\n";
    }

    void viewHistory() {
        auto history = bookingService.getBookingHistory(username);
        if (history.empty()) {
            cout << "\n  No booking history found.\n";
            return;
        }
        cout << "\n  --- Booking History (" << history.size() << " records) ---\n";
        for (const auto& r : history) {
            cout << static_cast<const Ticket&>(r);  // Use operator<< overload
            if (r.getStatus() == "CANCELLED" && r.getRefundAmount() > 0) {
                cout << "  Refund: Rs. " << std::fixed << std::setprecision(2)
                          << r.getRefundAmount() << "\n";
            }
        }
    }

    void advancedSearch() {
        cout << "\n  --- Advanced Search with Filters ---\n";
        string source = InputValidator::getValidString("  Source Station: ");
        string dest = InputValidator::getValidString("  Destination Station: ");
        string date = InputValidator::getValidDate("  Travel Date (YYYY-MM-DD): ");

        cout << "  Filter by type (1=AC, 2=Sleeper, 3=All): ";
        int typeChoice = InputValidator::getValidInteger("", 1, 3);
        string typeFilter = "ALL";
        if (typeChoice == 1) typeFilter = "AC";
        else if (typeChoice == 2) typeFilter = "Sleeper";

        double minPrice = InputValidator::getValidDouble("  Min Price (Rs.): ", 0, 50000);
        double maxPrice = InputValidator::getValidDouble("  Max Price (Rs.): ", minPrice, 50000);

        auto results = trainService.advancedSearch(source, dest, date, typeFilter, minPrice, maxPrice);
        if (results.empty()) {
            cout << "  No trains match your filters.\n";
            return;
        }
        cout << "\n  Found " << results.size() << " train(s):\n";
        for (const auto& t : results) t.displayInfo();
    }
};
