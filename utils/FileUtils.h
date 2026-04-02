#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "InputValidator.h"
using namespace std;

/*
 * FileUtils: Static helper methods for file I/O operations
 * Demonstrates: Encapsulation, Static Methods, Exception Handling
 */
class FileUtils {
public:
    // Read all non-empty lines from a file
    static vector<string> readLines(const string& filename) {
        ifstream file(filename);
        vector<string> lines;
        if (!file.is_open()) return lines;  // Return empty if file doesn't exist yet
        string line;
        while (getline(file, line)) {
            if (!line.empty()) lines.push_back(line);
        }
        file.close();
        return lines;
    }

    // Overwrite a file with the given lines
    static void writeLines(const string& filename, const vector<string>& lines) {
        ofstream file(filename);
        if (!file.is_open())
            throw FileException("Cannot open file for writing: " + filename);
        for (const auto& line : lines) file << line << "\n";
        file.close();
    }

    // Append a single line to a file
    static void appendLine(const string& filename, const string& line) {
        ofstream file(filename, ios::app);
        if (!file.is_open())
            throw FileException("Cannot open file for appending: " + filename);
        file << line << "\n";
        file.close();
    }

    // Split a string by a delimiter character
    static vector<string> split(const string& str, char delimiter) {
        vector<string> tokens;
        stringstream ss(str);
        string token;
        while (getline(ss, token, delimiter)) tokens.push_back(token);
        return tokens;
    }

    // Check if a file exists
    static bool fileExists(const string& filename) {
        ifstream file(filename);
        return file.good();
    }
};
