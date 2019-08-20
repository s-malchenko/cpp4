#include "bus_stop_test.h"
#include "bus_stop.h"
#include "test_runner.h"
#include "test_double_precision.h"
#include <vector>

using namespace std;

#define TEST_DISTANCE(start, finish, expected) \
{ \
    ASSERT(doublesEqual(start.DistanceTo(finish), expected)); \
}

void BusStopTest_Distance()
{
    auto from = Coordinates{0, 0};
    auto to = Coordinates{0, 180};
    TEST_DISTANCE(from, to, PI * EARTH_RADIUS);
    TEST_DISTANCE(to, from, PI * EARTH_RADIUS);

    from = {-36.6, 67.4};
    to = {8.4, 67.4};
    TEST_DISTANCE(from, to, PI * EARTH_RADIUS / 4);
    TEST_DISTANCE(to, from, PI * EARTH_RADIUS / 4);

    from = {55.595884, 37.209755};
    to = {55.632761, 37.333324};
    ASSERT(doublesEqual(from.DistanceTo(to), 8800, 100));
    ASSERT(doublesEqual(to.DistanceTo(from), 8800, 100));
}

#define TEST_BUSES(src, expected) \
{ \
    BusStop stop; \
    for (const auto &i : src) \
    { \
        stop.AddBus(i); \
    }\
    ASSERT_EQUAL(stop.GetBuses(), expected); \
}

void BusStopTest_Buses()
{
    vector<string_view> src = {"1", "2", "3"};
    set<string_view> expected = {"1", "2", "3"};
    TEST_BUSES(src, expected);
    src = {"3", "2", "1", "2", "1"};
    TEST_BUSES(src, expected);
}
