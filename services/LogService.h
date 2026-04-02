#pragma once
#include <string>
#include "../utils/FileUtils.h"
#include "../utils/TimeUtils.h"
using namespace std;

/*
 * LogService: Maintains a timestamped event log in data/logs.txt
 * Events: login attempts, bookings, cancellations, admin actions
 * Demonstrates: Encapsulation, File I/O
 */
class LogService {
    string logFile;

public:
    LogService(const string& file = "data/logs.txt") : logFile(file) {}

    // Log an event with timestamp and event type
    void log(const string& eventType, const string& message) {
        string entry = "[" + TimeUtils::getCurrentTimeString() + "] ["
                          + eventType + "] " + message;
        try {
            FileUtils::appendLine(logFile, entry);
        } catch (...) {
            // Silently fail — logging should never crash the app
        }
    }

    // Read and display all log entries
    void displayLogs() const {
        vector<string> lines = FileUtils::readLines(logFile);
        cout << "\n  ============ SYSTEM LOGS ============\n";
        if (lines.empty()) {
            cout << "  No log entries found.\n";
            return;
        }
        for (const auto& line : lines)
            cout << "  " << line << "\n";
        cout << "  =====================================\n";
    }
};
