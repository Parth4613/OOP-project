#pragma once
#include <string>
using namespace std;
class Schedule {
    string departureDate;   
    string departureTime;   
    string arrivalTime;         

public:
    Schedule() = default;
    Schedule(const string& depDate, const string& depTime, const string& arrTime)
        : departureDate(depDate), departureTime(depTime), arrivalTime(arrTime) {}

    string getDepartureDate() const { return departureDate; }
    string getDepartureTime() const { return departureTime; }
    string getArrivalTime() const { return arrivalTime; }

    void setDepartureDate(const string& d) { departureDate = d; }
    void setDepartureTime(const string& t) { departureTime = t; }
    void setArrivalTime(const string& t) { arrivalTime = t; }
};
