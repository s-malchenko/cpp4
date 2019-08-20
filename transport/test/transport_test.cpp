#include "transport_test.h"
#include "test_runner.h"
#include "input_parser_test.h"
#include "bus_stop_test.h"
#include "bus_route_test.h"
#include "transport_processor_test.h"

void TestTransport()
{
    TestRunner tr;
    RUN_TEST(tr, InputParserTest_GetPart);
    RUN_TEST(tr, InputParserTest_ParseStops);
    RUN_TEST(tr, InputParserTest_ParseCoordinates);
    RUN_TEST(tr, BusStopTest_Distance);
    RUN_TEST(tr, BusRouteTest_Constructor);
    RUN_TEST(tr, BusRouteTest_GetDistance);
    RUN_TEST(tr, BusRouteTest_GetUniqueStopsCount);
    RUN_TEST(tr, BusRouteTest_GetStopsCount);
    RUN_TEST(tr, TransportProcessorTest_Run);
}
