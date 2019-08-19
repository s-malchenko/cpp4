#include "bus_stop_test.h"
#include "bus_stop.h"
#include "test_runner.h"
#include "test_double_precision.h"

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
}
