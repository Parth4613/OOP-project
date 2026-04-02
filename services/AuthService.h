#pragma once
#include <string>
#include <iostream>
#include "../models/User.h"
#include "../utils/FileUtils.h"
#include "../utils/InputValidator.h"
#include "LogService.h"
using namespace std;

/*
 * AuthService: Handles user registration, login, and password hashing
 * Uses simple XOR-based hashing for demo purposes (STL-only constraint)
 * Demonstrates: Encapsulation, Polymorphism (returns User*), Dynamic Memory (new)
 */

// Forward declarations — Admin and Passenger are defined later
class Admin;
class Passenger;

class AuthService {
    LogService& logger;
    string usersFile;

    // Simple XOR hash for password obfuscation (demo-grade, NOT production-safe)
    static string hashPassword(const string& password) {
        const char KEY = 0x5A;  // XOR key
        string hashed = password;
        for (char& c : hashed) c ^= KEY;
        // Convert to hex string for safe file storage
        string hex;
        for (unsigned char c : hashed) {
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", c);
            hex += buf;
        }
        return hex;
    }

public:
    AuthService(LogService& log, const string& file = "data/users.txt")
        : logger(log), usersFile(file) {}

    // Seed default admin account if it doesn't exist
    void seedAdmin() {
        vector<string> lines = FileUtils::readLines(usersFile);
        for (const auto& line : lines) {
            vector<string> parts = FileUtils::split(line, '|');
            if (parts.size() >= 3 && parts[0] == "admin") return;  // Already exists
        }
        // Create default admin: admin / admin123
        string entry = "admin|" + hashPassword("admin123") + "|admin";
        FileUtils::appendLine(usersFile, entry);
        logger.log("SYSTEM", "Default admin account seeded");
    }

    // Register a new passenger account
    bool registerUser(const string& username, const string& password) {
        // Check if username already exists
        vector<string> lines = FileUtils::readLines(usersFile);
        for (const auto& line : lines) {
            vector<string> parts = FileUtils::split(line, '|');
            if (!parts.empty() && parts[0] == username) {
                throw AuthException("Username '" + username + "' already exists!");
                return false;
            }
        }
        string entry = username + "|" + hashPassword(password) + "|passenger";
        FileUtils::appendLine(usersFile, entry);
        logger.log("REGISTER", "New passenger registered: " + username);
        return true;
    }

    // Authenticate user and return role ("admin" or "passenger"), empty string on failure
    string authenticate(const string& username, const string& password) {
        vector<string> lines = FileUtils::readLines(usersFile);
        string hashed = hashPassword(password);

        for (const auto& line : lines) {
            vector<string> parts = FileUtils::split(line, '|');
            if (parts.size() >= 3 && parts[0] == username && parts[1] == hashed) {
                logger.log("LOGIN", "Successful login: " + username + " (" + parts[2] + ")");
                return parts[2];  // Return role
            }
        }
        logger.log("LOGIN", "Failed login attempt: " + username);
        throw AuthException("Invalid username or password!");
        return "";
    }
};
