#include "transport_processor_test.h"
#include "transport_processor.h"
#include "test_runner.h"

#include <sstream>
#include <string>

using namespace std;

#define TEST_RUN(in, expected) \
{ \
    stringstream out; \
    stringstream input(in); \
    RunTransportProcessor(input, out); \
    ASSERT_EQUAL(out.str(), expected); \
}

void TransportProcessorTest_Run()
{
    string inStr(R"(10
Stop Tolstopaltsevo: 55.611087, 37.20829
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517
Stop Biryusinka: 55.581065, 37.64839
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751)");
    string outStr(R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751: not found
)");
    TEST_RUN(inStr, outStr);

    inStr = R"(13
Stop Tolstopaltsevo: 55.611087, 37.20829
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517
Stop Biryusinka: 55.581065, 37.64839
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye
)";
    outStr = R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828
)";
    TEST_RUN(inStr, outStr);
}