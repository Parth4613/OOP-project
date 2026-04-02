#include <iostream>
#include <cstdlib>
#include <ctime>
#include "services/LogService.h"
#include "services/AuthService.h"
#include "services/TrainService.h"
#include "services/BookingService.h"
#include "services/ReportService.h"
#include "models/Admin.h"
#include "models/Passenger.h"
using namespace std;

void displayBanner() {
    cout << "\n";
    cout << "  +====================================================+\n";
    cout << "  |                                                    |\n";
    cout << "  |     ONLINE RAILWAY RESERVATION SYSTEM              |\n";
    cout << "  |     Advanced C++ OOP Project                       |\n";
    cout << "  |                                                    |\n";
    cout << "  +====================================================+\n";
    cout << "\n";
}

int showMainMenu() {
    cout << "  +----------------------------------------------------+\n";
    cout << "  |                  MAIN MENU                         |\n";
    cout << "  +----------------------------------------------------+\n";
    cout << "  |  1. Login as Admin                                 |\n";
    cout << "  |  2. Login as Passenger                             |\n";
    cout << "  |  3. Register as New Passenger                      |\n";
    cout << "  |  4. Exit                                           |\n";
    cout << "  +----------------------------------------------------+\n";
    return InputValidator::getValidInteger("  Enter your choice: ", 1, 4);
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    LogService logger;
    AuthService authService(logger);
    TrainService trainService;
    BookingService bookingService(trainService, logger);

    authService.seedAdmin();

    logger.log("SYSTEM", "Railway Reservation System started");

    displayBanner();

    bool running = true;
    while (running) {
        int choice = showMainMenu();

        switch (choice) {
            case 1: {
                cout << "\n  --- Admin Login ---\n";
                string username = InputValidator::getValidString("  Username: ");
                string password = InputValidator::getValidString("  Password: ");

                try {
                    string role = authService.authenticate(username, password);
                    if (role != "admin") {
                        cout << "  [!] Access denied. Not an admin account.\n";
                        break;
                    }
                    User* user = new Admin(username, password, trainService, bookingService, logger);
                    user->login();      
                    user->displayMenu();  
                    delete user;         
                } catch (const AuthException& e) {
                    cout << "  [!] " << e.what() << "\n";
                }
                break;
            }

            case 2: {
                cout << "\n  --- Passenger Login ---\n";
                string username = InputValidator::getValidString("  Username: ");
                string password = InputValidator::getValidString("  Password: ");

                try {
                    string role = authService.authenticate(username, password);
                    if (role != "passenger") {
                        cout << "  [!] Please use Admin Login for admin accounts.\n";
                        break;
                    }
                    User* user = new Passenger(username, password, trainService, bookingService, logger);
                    user->login();       
                    user->displayMenu(); 
                    delete user;          
                } catch (const AuthException& e) {
                    cout << "  [!] " << e.what() << "\n";
                }
                break;
            }

            case 3: {
                cout << "\n  --- Register New Passenger ---\n";
                string username = InputValidator::getValidString("  Choose a username: ");
                string password = InputValidator::getValidString("  Choose a password: ");

                try {
                    if (authService.registerUser(username, password)) {
                        cout << "  [+] Registration successful! You can now login.\n";
                    }
                } catch (const AuthException& e) {
                    cout << "  [!] " << e.what() << "\n";
                }
                break;
            }

            case 4: {
                cout << "\n  Thank you for using the Railway Reservation System!\n";
                cout << "  Goodbye.\n\n";
                logger.log("SYSTEM", "Railway Reservation System shutdown");
                running = false;
                break;
            }
        }
    }
    return 0;
}
