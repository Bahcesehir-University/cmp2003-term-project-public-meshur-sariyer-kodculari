#include "analyzer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;



static vector<string> splitCSV(const string& line) {
    vector<string> result;
    string cell;
    bool inQuotes = false;

    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            result.push_back(cell);
            cell.clear();
        } else {
            cell.push_back(c);
        }
    }
    result.push_back(cell);
    return result;
}

static bool extractHour(const string& datetime, int& hour) {
    if (datetime.size() < 13) return false;
    try {
        hour = stoi(datetime.substr(11, 2));
        return hour >= 0 && hour <= 23;
    } catch (...) {
        return false;
    }
}



struct SlotKey {
    string zone;
    int hour;

    bool operator==(const SlotKey& other) const {
        return zone == other.zone && hour == other.hour;
    }
};

struct SlotKeyHash {
    size_t operator()(const SlotKey& k) const {
        return hash<string>()(k.zone) ^ hash<int>()(k.hour);
    }
};



void TripAnalyzer::ingestFile(const string& csvPath) {

    zone_map.clear();
    slot_map.clear();

    ifstream file(csvPath);
    if (!file.is_open()) {
    cout<<"hata"<<csvPath<<endl;
     return;
    }
    string line;

    // header atla
    getline(file, line);

    while (getline(file, line)) {

        if (line.empty()) continue;

        vector<string> row = splitCSV(line);

        if (row.size() < 6) continue;

        string zone = row[1];
        int hour;

        if (!extractHour(row[3], hour)) continue;

        zone_map[zone]++;
        slot_map[{zone, hour}]++;
    }
}

vector<ZoneCount> TripAnalyzer::topZones(int k) const {

    vector<ZoneCount> result;

    for (auto const& p : zone_map) {
        result.push_back({p.first, p.second});
    }

    sort(result.begin(), result.end(),
        [](const ZoneCount& a, const ZoneCount& b) {
            if (a.count != b.count)
                return a.count > b.count;
            return a.zone < b.zone;
        });

    if ((int)result.size() > k)
        result.resize(k);

    return result;
}

vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {

    vector<SlotCount> result;

    for (auto const& p : slot_map) {
        result.push_back({p.first.first, p.first.second, p.second});
    }

    sort(result.begin(), result.end(),
        [](const SlotCount& a, const SlotCount& b) {
            if (a.count != b.count)
                return a.count > b.count;
            if (a.zone != b.zone)
                return a.zone < b.zone;
            return a.hour < b.hour;
        });

    if ((int)result.size() > k)
        result.resize(k);

    return result;
}