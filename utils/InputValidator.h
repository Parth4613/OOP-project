#pragma once
#include <string>
#include <stdexcept>
#include <iostream>
#include <limits>
#include <regex>
using namespace std;

/*
 * Custom Exception Classes for the Railway System
 * Demonstrates: Exception Handling, Inheritance
 */
class AuthException : public runtime_error {
public:
    explicit AuthException(const string& msg) : runtime_error(msg) {}
};

class BookingException : public runtime_error {
public:
    explicit BookingException(const string& msg) : runtime_error(msg) {}
};

class FileException : public runtime_error {
public:
    explicit FileException(const string& msg) : runtime_error(msg) {}
};

class InputException : public runtime_error {
public:
    explicit InputException(const string& msg) : runtime_error(msg) {}
};

/*
 * InputValidator: Validates and sanitizes all user input
 * Demonstrates: Encapsulation, Static Methods
 */
class InputValidator {
public:
    static void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    static int getValidInteger(const string& prompt, int minVal, int maxVal) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value && value >= minVal && value <= maxVal) {
                clearInput();
                return value;
            }
            clearInput();
            cout << "  [!] Invalid input. Enter a number between "
                      << minVal << " and " << maxVal << ".\n";
        }
    }

    static string getValidString(const string& prompt) {
        string value;
        while (true) {
            cout << prompt;
            getline(cin, value);
            if (!value.empty()) return value;
            cout << "  [!] Input cannot be empty.\n";
        }
    }

    static double getValidDouble(const string& prompt, double minVal, double maxVal) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value && value >= minVal && value <= maxVal) {
                clearInput();
                return value;
            }
            clearInput();
            cout << "  [!] Invalid input. Enter a number between "
                      << minVal << " and " << maxVal << ".\n";
        }
    }

    static string getValidDate(const string& prompt) {
        string date;
        regex datePattern("\\d{4}-\\d{2}-\\d{2}");
        while (true) {
            cout << prompt;
            getline(cin, date);
            if (regex_match(date, datePattern)) return date;
            cout << "  [!] Invalid date format. Use YYYY-MM-DD.\n";
        }
    }

    static string getValidTime(const string& prompt) {
        string t;
        regex timePattern("\\d{2}:\\d{2}");
        while (true) {
            cout << prompt;
            getline(cin, t);
            if (regex_match(t, timePattern)) return t;
            cout << "  [!] Invalid time format. Use HH:MM.\n";
        }
    }
};
