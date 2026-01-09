#pragma once
#include <string>
#include <vector>
#include <map>

using namespace std;
struct ZoneCount {
    string zone;
    long long count;
};

struct SlotCount {
    string zone;
    int hour;              // 0–23
    long long count;
};

class TripAnalyzer {
public:
    void ingestFile(const string& csvPath);
    vector<ZoneCount> topZones(int k = 10) const;
        vector<SlotCount> topBusySlots(int k = 10) const;

private:
    vector<ZoneCount> m_zoneCounts;
    vector<SlotCount> m_slotCounts;
    map<string, long long> zone_map;
    map<pair<string,int>,long long>slot_map;
};