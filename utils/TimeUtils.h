#pragma once
#include <string>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <sstream>
using namespace std;

/*
 * TimeUtils: Static helper methods for time operations
 * Uses <ctime> for system time access and parsing
 * Demonstrates: Encapsulation, Static Methods
 */
class TimeUtils {
public:
    static time_t getCurrentTime() {
        return time(nullptr);
    }

    // Format: "YYYY-MM-DD HH:MM:SS"
    static string getCurrentTimeString() {
        time_t now = getCurrentTime();
        char buf[64];
        struct tm* tmInfo = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmInfo);
        return string(buf);
    }

    static string formatTime(time_t t) {
        char buf[64];
        struct tm* tmInfo = localtime(&t);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmInfo);
        return string(buf);
    }

    static string getCurrentDate() {
        time_t now = getCurrentTime();
        char buf[32];
        struct tm* tmInfo = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d", tmInfo);
        return string(buf);
    }

    // Parse date (YYYY-MM-DD) and time (HH:MM) strings into time_t
    static time_t parseDateTime(const string& date, const string& timeStr) {
        struct tm tm;
        memset(&tm, 0, sizeof(tm));
        int year, month, day, hour, minute;
        sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);
        sscanf(timeStr.c_str(), "%d:%d", &hour, &minute);
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = minute;
        tm.tm_isdst = -1;
        return mktime(&tm);
    }

    // Get the difference in hours between two time_t values
    static double getHoursDifference(time_t from, time_t to) {
        return difftime(to, from) / 3600.0;
    }

    // Check if the given date/time is in the past
    static bool isPast(const string& date, const string& timeStr) {
        time_t departure = parseDateTime(date, timeStr);
        time_t now = getCurrentTime();
        return difftime(departure, now) <= 0;
    }
};
