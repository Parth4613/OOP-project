#pragma once
#include <string>
#include <set>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include "FileUtils.h"
using namespace std;

/*
 * IDGenerator: Generates unique ticket IDs
 * Format: TRN + (timestamp % 100000000) + random(0-9999)
 * Example: TRN170923451234
 * Demonstrates: Static Members, STL (set)
 */
class IDGenerator {
    // Use a static method with a local static variable instead of
    // inline static member (not supported in GCC 6.3.0)
    static set<string>& getUsedIds() {
        static set<string> usedIds;
        return usedIds;
    }

public:
    // Generate a unique ticket ID that hasn't been used before
    static string generateTicketId() {
        string id;
        do {
            time_t now = time(nullptr);
            int randomNum = rand() % 10000;
            stringstream ss;
            ss << "TRN" << (now % 100000000) << randomNum;
            id = ss.str();
        } while (getUsedIds().find(id) != getUsedIds().end());
        getUsedIds().insert(id);
        return id;
    }

    // Register an existing ID (loaded from file) to prevent duplicates
    static void addUsedId(const string& id) {
        getUsedIds().insert(id);
    }

    // Load all existing ticket IDs from tickets file
    static void loadUsedIds(const string& filename) {
        vector<string> lines = FileUtils::readLines(filename);
        for (const auto& line : lines) {
            vector<string> parts = FileUtils::split(line, '|');
            if (!parts.empty()) getUsedIds().insert(parts[0]);
        }
    }
};
