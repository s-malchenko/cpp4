#pragma once

#include <istream>
#include <ostream>
#include <string_view>
#include <vector>

namespace InputParser
{
using namespace std;

string_view GetPart(string_view &src, char delim = ' ');
vector<string_view> ParseStops(string_view &src);
pair<double, double> ParseCoordinates(string_view &src);

}