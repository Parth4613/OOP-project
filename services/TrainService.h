#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "../models/Train.h"
#include "../utils/FileUtils.h"
#include "../utils/TimeUtils.h"
using namespace std;

/*
 * TrainService: CRUD operations and search for trains
 * Demonstrates: Encapsulation, STL (vector, algorithm)
 */
class TrainService {
    string trainsFile;

public:
    TrainService(const string& file = "data/trains.txt") : trainsFile(file) {}

    // Load all trains from file
    vector<Train> loadTrains() const {
        vector<Train> trains;
        vector<string> lines = FileUtils::readLines(trainsFile);
        for (const auto& line : lines) {
            try {
                trains.push_back(Train::deserialize(line));
            } catch (...) { /* Skip malformed entries */ }
        }
        return trains;
    }

    // Save all trains back to file (full rewrite)
    void saveTrains(const vector<Train>& trains) const {
        vector<string> lines;
        for (const auto& t : trains) lines.push_back(t.serialize());
        FileUtils::writeLines(trainsFile, lines);
    }

    // Add a new train
    bool addTrain(const Train& train) {
        vector<Train> trains = loadTrains();
        // Check for duplicate train number
        for (const auto& t : trains) {
            if (t.getTrainNumber() == train.getTrainNumber()) return false;
        }
        trains.push_back(train);
        saveTrains(trains);
        return true;
    }

    // Delete a train by number
    bool deleteTrain(const string& trainNumber) {
        vector<Train> trains = loadTrains();
        auto it = remove_if(trains.begin(), trains.end(),
            [&](const Train& t) { return t.getTrainNumber() == trainNumber; });
        if (it == trains.end()) return false;
        trains.erase(it, trains.end());
        saveTrains(trains);
        return true;
    }

    // Get a single train by number (throws if not found)
    Train getTrainByNumber(const string& trainNumber) const {
        vector<Train> trains = loadTrains();
        for (const auto& t : trains) {
            if (t.getTrainNumber() == trainNumber) return t;
        }
        throw BookingException("Train " + trainNumber + " not found!");
    }

    // Update a train (replace existing entry with same train number)
    bool updateTrain(const Train& updated) {
        vector<Train> trains = loadTrains();
        for (auto& t : trains) {
            if (t.getTrainNumber() == updated.getTrainNumber()) {
                t = updated;
                saveTrains(trains);
                return true;
            }
        }
        return false;
    }

    // Search trains by source, destination, and date
    vector<Train> searchTrains(const string& source, const string& dest,
                                     const string& date) const {
        vector<Train> results;
        vector<Train> trains = loadTrains();
        for (const auto& t : trains) {
            // Case-insensitive comparison for source and destination
            string tSrc = t.getSource(), tDst = t.getDestination();
            string qSrc = source, qDst = dest;
            transform(tSrc.begin(), tSrc.end(), tSrc.begin(), ::tolower);
            transform(tDst.begin(), tDst.end(), tDst.begin(), ::tolower);
            transform(qSrc.begin(), qSrc.end(), qSrc.begin(), ::tolower);
            transform(qDst.begin(), qDst.end(), qDst.begin(), ::tolower);

            if (tSrc == qSrc && tDst == qDst && t.getSchedule().getDepartureDate() == date)
                results.push_back(t);
        }
        return results;
    }

    // Advanced search with filters: type, price range, departure time
    vector<Train> advancedSearch(const string& source, const string& dest,
                                       const string& date, const string& typeFilter,
                                       double minPrice, double maxPrice) const {
        vector<Train> base = searchTrains(source, dest, date);
        vector<Train> filtered;
        for (const auto& t : base) {
            // Filter by type
            if (!typeFilter.empty() && typeFilter != "ALL") {
                if (Train::trainTypeToString(t.getType()) != typeFilter) continue;
            }
            // Filter by price range
            if (t.getBaseFare() < minPrice || t.getBaseFare() > maxPrice) continue;
            filtered.push_back(t);
        }
        return filtered;
    }

    // Get all trains (for admin view)
    vector<Train> getAllTrains() const { return loadTrains(); }
};
