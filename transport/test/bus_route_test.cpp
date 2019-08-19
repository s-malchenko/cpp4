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
    const optional<double> &DistanceOpt() { return _distance; }
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

void BusRouteTest_GetDistance()
{
    unordered_map<string, BusStop> base;
    string name = "stop1";
    base[name] = {name, {0, 0}};
    name = "stop2";
    base[name] = {name, {45, 0}};

    vector<string_view> stops = {"stop1", "stop2", "stop1", "stop2"};

    BusRouteTest busNotRing(1);
    busNotRing.AssignStops(stops.begin(), stops.end());
    ASSERT(doublesEqual(busNotRing.GetDistance(base), PI * EARTH_RADIUS * 3 / 2));

    BusRouteTest busRing1(2, true);
    stops = {"stop1", "stop2", "stop1", "stop2", "stop1"};
    busRing1.AssignStops(stops.begin(), stops.end());
    ASSERT(doublesEqual(busRing1.GetDistance(base), PI * EARTH_RADIUS));

    BusRouteTest busRing2(3, true);
    stops = {"stop1", "stop2", "stop1", "stop2"};
    busRing2.AssignStops(stops.begin(), stops.end());
    ASSERT(doublesEqual(busRing2.GetDistance(base), PI * EARTH_RADIUS));
}

void BusRouteTest_GetUniqueStopsCount()
{
    BusRouteTest busRing2(3, true);
    vector<string_view> stops = {"stop 1", "stop 2", "stop 1", "stop 2"};
    ASSERT_EQUAL(busRing2.GetUniqueStopsCount(), 0);
    busRing2.AssignStops(stops.begin(), stops.end());
    ASSERT_EQUAL(busRing2.GetUniqueStopsCount(), 2);
}