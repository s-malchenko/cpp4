#include "transport_test.h"
#include "test_runner.h"
#include "input_parser_test.h"
#include "bus_stop_test.h"

void TestTransport()
{
    TestRunner tr;
    RUN_TEST(tr, InputParserTest_GetPart);
    RUN_TEST(tr, InputParserTest_ParseStops);
    RUN_TEST(tr, InputParserTest_ParseCoordinates);
    RUN_TEST(tr, BusStopTest_Distance);
}
