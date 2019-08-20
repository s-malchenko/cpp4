#include "input_parser.h"
#include "input_parser_test.h"
#include "test_runner.h"
#include "test_double_precision.h"

#include <string>

using namespace InputParser;
using namespace std;

#define TEST_GET_PART_DEFAULT_DELIM(src, part, remainder) \
{ \
    string_view test(src); \
    string_view actualPart = GetPart(test); \
    ASSERT_EQUAL(actualPart, part); \
    ASSERT_EQUAL(test, remainder); \
}

#define TEST_GET_PART(src, part, remainder, delim) \
{ \
    string_view test(src); \
    string_view actualPart = GetPart(test, delim); \
    ASSERT_EQUAL(actualPart, part); \
    ASSERT_EQUAL(test, remainder); \
}

void InputParserTest_GetPart()
{
    TEST_GET_PART_DEFAULT_DELIM("for honor", "for", "honor");
    TEST_GET_PART_DEFAULT_DELIM("1236854", "1236854", "");
    TEST_GET_PART("1236854", "1236854", "", ':');
    TEST_GET_PART("1236854", "12368", "4", '5');
    TEST_GET_PART("12358: gfjdls;gj", "12358", " gfjdls;gj", ':');
    TEST_GET_PART("kk3fgfdgrdfg4", "", "k3fgfdgrdfg4", 'k');
}

#define TEST_PARSE_STOPS(src, expected) \
{ \
    auto stops = ParseStops(src); \
    ASSERT_EQUAL(stops, expected); \
}

void InputParserTest_ParseStops()
{
    vector<string_view> expected;
    string_view src(" Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye");
    expected = {"Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"};
    TEST_PARSE_STOPS(src, expected);

    src = "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye";
    TEST_PARSE_STOPS(src, expected);

    src = "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n";
    TEST_PARSE_STOPS(src, expected);

    expected = {"Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"};
    src = "Biryusinka - Universam - Biryulyovo Tovarnaya - Biryulyovo Passazhirskaya - Biryulyovo Zapadnoye\n";
    TEST_PARSE_STOPS(src, expected);

    src = "Biryusinka - Universam - Biryulyovo Tovarnaya - Biryulyovo Passazhirskaya - Biryulyovo Zapadnoye";
    TEST_PARSE_STOPS(src, expected);

    src = "   Biryusinka - Universam - Biryulyovo Tovarnaya - Biryulyovo Passazhirskaya - Biryulyovo Zapadnoye";
    TEST_PARSE_STOPS(src, expected);
}

#define TEST_PARSE_COORDINATES(src, expected1, expected2) \
{ \
    string_view test(src); \
    auto [actual1, actual2] = ParseCoordinates(test); \
    ASSERT(doublesEqual(actual1, expected1)); \
    ASSERT(doublesEqual(actual2, expected2)); \
}

void InputParserTest_ParseCoordinates()
{
    TEST_PARSE_COORDINATES(" 27.3, 44.3265", 27.3, 44.3265);
    TEST_PARSE_COORDINATES("27.3, 44.3265", 27.3, 44.3265);
    TEST_PARSE_COORDINATES("27.3, 44.3265 fds", 27.3, 44.3265);
}

static inline void addTableEntry(DistanceTable &table, string_view s1, string_view s2, unsigned int len)
{
    table[string(s1)][string(s2)] = table[string(s2)][string(s1)] = len;
}

void InputParserTest_ParseStopDistances()
{
    string_view name = "1";
    string_view src = ", 1200m to Biryulyovo Zapadnoye";
    DistanceTable table;
    ParseStopDistances(src, name, table);
    DistanceTable tableRight;
    addTableEntry(tableRight, "1", "Biryulyovo Zapadnoye", 1200);
    ASSERT_EQUAL(table, tableRight);

    name = "2";
    src = "7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam";
    ParseStopDistances(src, name, table);
    addTableEntry(tableRight, "2", "Rossoshanskaya ulitsa", 7500);
    addTableEntry(tableRight, "2", "Biryusinka", 1800);
    addTableEntry(tableRight, "2", "Universam", 2400);
    ASSERT_EQUAL(table, tableRight);

    table = DistanceTable();
    tableRight = table;
    src = ", \n";
    ParseStopDistances(src, name, table);
    ASSERT_EQUAL(table, tableRight);
}
