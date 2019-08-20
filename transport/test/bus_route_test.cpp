#include "bus_route_test.h"
#include "bus_route.h"
#include "bus_stop.h"
#include "test_runner.h"
#include "test_double_precision.h"
#include <unordered_map>
#include <sstream>

using namespace std;

class BusRouteTest : public BusRoute<unordered_map<string, BusStop>>
{
public:
    BusRouteTest(int num, bool ring = false) : BusRoute(num, ring) {}
    bool GetRing() const { return _ring; }
    const optional<double> &DistanceOpt() const { return _distance; }
};

void BusRouteTest_Constructor()
{
    {
        int num = 78;
        BusRouteTest bus(num);
        ASSERT_EQUAL(num, bus.GetNumber());
        ASSERT_EQUAL(false, bus.GetRing());
        ASSERT(!bus.DistanceOpt());
    }
    {
        int num = 653;
        BusRouteTest bus(num, false);
        ASSERT_EQUAL(num, bus.GetNumber());
        ASSERT_EQUAL(false, bus.GetRing());
        ASSERT(!bus.DistanceOpt());
    }
    {
        int num = 777;
        BusRouteTest bus(num, true);
        ASSERT_EQUAL(num, bus.GetNumber());
        ASSERT_EQUAL(true, bus.GetRing());
        ASSERT(!bus.DistanceOpt());
    }
}

static inline void addStopToBase(unordered_map<string, BusStop> &base,
                                 const string &name,
                                 double lat, double lon)
{
    base[name] = {name, {lat, lon}};
}

#define TEST_GET_DISTANCE(stops, ring, base, expected) \
{ \
    BusRouteTest bus(1, ring); \
    bus.AssignStops(stops.begin(), stops.end()); \
    ASSERT(doublesEqual(bus.GetDistance(base), expected)); \
}

void BusRouteTest_GetDistance()
{
    unordered_map<string, BusStop> base;
    addStopToBase(base, "stop1", 0, 0);
    addStopToBase(base, "stop2", 45, 0);
    addStopToBase(base, "Tolstopaltsevo", 55.611087, 37.20829);
    addStopToBase(base, "Marushkino", 55.595884, 37.209755);
    addStopToBase(base, "Rasskazovka", 55.632761, 37.333324);
    addStopToBase(base, "Biryulyovo Zapadnoye", 55.574371, 37.6517);
    addStopToBase(base, "Biryusinka", 55.581065, 37.64839);
    addStopToBase(base, "Universam", 55.587655, 37.645687);
    addStopToBase(base, "Biryulyovo Tovarnaya", 55.592028, 37.653656);
    addStopToBase(base, "Biryulyovo Passazhirskaya", 55.580999, 37.659164);

    vector<string_view> stops = {"stop1", "stop2", "stop1", "stop2"};
    TEST_GET_DISTANCE(stops, false, base, PI * EARTH_RADIUS * 3 / 2);

    stops = {"stop1", "stop2", "stop1", "stop2", "stop1"};
    TEST_GET_DISTANCE(stops, true, base, PI * EARTH_RADIUS);

    stops = {"stop1", "stop2", "stop1", "stop2"};
    TEST_GET_DISTANCE(stops, true, base, PI * EARTH_RADIUS);

    stops = {"Biryulyovo Zapadnoye", "Biryusinka", "Universam",
             "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya",
             "Biryulyovo Zapadnoye"
            };
    TEST_GET_DISTANCE(stops, true, base, 4371.02);
}

void BusRouteTest_GetUniqueStopsCount()
{
    BusRouteTest busRing2(3, true);
    vector<string_view> stops = {"stop 1", "stop 2", "stop 1", "stop 2"};
    ASSERT_EQUAL(busRing2.GetUniqueStopsCount(), 0);
    busRing2.AssignStops(stops.begin(), stops.end());
    ASSERT_EQUAL(busRing2.GetUniqueStopsCount(), 2);
}

void BusRouteTest_GetStopsCount()
{
    vector<string_view> stops = {"stop 1", "stop 2", "stop 1", "stop 2"};

    BusRouteTest busNotRing(1);
    ASSERT_EQUAL(busNotRing.GetStopsCount(), 0);
    busNotRing.AssignStops(stops.begin(), stops.end());
    ASSERT_EQUAL(busNotRing.GetStopsCount(), 7);

    BusRouteTest busRing2(3, true);
    busRing2.AssignStops(stops.begin(), stops.end());
    ASSERT_EQUAL(busRing2.GetStopsCount(), 5);
}