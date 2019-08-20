#include "transport_response_test.h"
#include "transport_response.h"
#include "test_runner.h"
#include <sstream>

using namespace std;

#define TEST_PROCEED_RESPONSE(resp, result) \
{ \
    stringstream out; \
    resp.Proceed(out); \
    ASSERT_EQUAL(out.str(), result); \
}

void TransportResponseTest_BusProceed()
{
    auto response = BusResponse("654", 0, true);
    TEST_PROCEED_RESPONSE(response,
                          "Bus 654: not found\n");

    response = BusResponse("33").Length(5658).Curvature(1.56).Stops(89).UniqueStops(56);
    TEST_PROCEED_RESPONSE(response,
                          "Bus 33: 89 stops on route, 56 unique stops, 5658 route length, 1.56 curvature\n");
}

void TransportResponseTest_StopProceed()
{
    auto response = StopResponse("654", 0, true);
    TEST_PROCEED_RESPONSE(response,
                          "Stop 654: not found\n");

    response = StopResponse("99");
    TEST_PROCEED_RESPONSE(response,
                          "Stop 99: no buses\n");

    set<string_view> buses = {"bus3", "bus1", "the coolest bus"};
    response = StopResponse("Pyater ochka").Buses(buses);
    TEST_PROCEED_RESPONSE(response,
                          "Stop Pyater ochka: buses bus1 bus3 the coolest bus\n");
}

