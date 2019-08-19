#include "input_parser.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace InputParser
{
using namespace std;

string_view GetPart(string_view &src, char delim)
{
    string_view result = src;
    size_t pos = src.find(delim);

    if (pos != string_view::npos)
    {
        result.remove_suffix(result.size() - pos);
        src.remove_prefix(pos + 1);
    }
    else
    {
        src.remove_prefix(src.size());
    }

    return result;
}

static inline void trimChars(string_view &src, string_view chars)
{
    src.remove_prefix(min(src.size(), src.find_first_not_of(chars)));
    size_t pos = src.find_last_not_of(chars);

    if (pos == string_view::npos)
    {
        src.remove_suffix(src.size());
    }
    else
    {
        src.remove_suffix(src.size() - pos - 1);
    }
}

vector<string_view> ParseStops(string_view &src)
{
    trimChars(src, " \n");
    static const string_view possibleDelims(">-");
    char delim = src[src.find_first_of(possibleDelims)];
    vector<string_view> result;

    do
    {
        string_view stop = GetPart(src, delim);
        trimChars(stop, " ");
        trimChars(src, " ");

        if (stop.size() > 0)
        {
            result.push_back(stop);
        }
    }
    while (src.size() > 0);

    return result;
}

pair<double, double> ParseCoordinates(string_view &src)
{
    pair<double, double> result;

    trimChars(src, " \n");
    auto longitude = GetPart(src, ',');
    stringstream(move(string(longitude))) >> result.first;
    trimChars(src, " \n");
    stringstream(move(string(src))) >> result.second;
    return result;
}

}
