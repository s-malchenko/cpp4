#include "transport_processor.h"
#include "input_parser.h"

#include <iomanip>
#include <iostream>

using namespace std;
using namespace InputParser;

TransportProcessor::TransportProcessor(ostream &out) : _out(out) {}

void TransportProcessor::ProcessDatabaseRequest(const string &request)
{
    string_view line(request);
    string_view cmd = GetPart(line);
    string_view arg = GetPart(line, ':');

    if (cmd == "Bus")
    {
        bool ring = false;

        if (line[line.find_first_of(">-")] == '>')
        {
            ring = true;
        }

        auto stops = ParseStops(line);
        string num(arg);
        _busesBase.insert({num, {num, ring}}).first->second.AssignStops(stops.begin(), stops.end());
    }
    else if (cmd == "Stop")
    {
        string name(arg);
        auto [latitude, longitude] = ParseCoordinates(line);
        Coordinates site{latitude, longitude};
        _stopsBase.insert({move(name), {name, site}});
    }
}

void TransportProcessor::ProcessReadingRequest(const string &request)
{
    string_view line(request);
    GetPart(line);
    string num(line);

    auto it = _busesBase.find(num);

    _out << "Bus " << num << ": ";

    if (it == _busesBase.end())
    {
        _out << "not found" << endl;
    }
    else
    {
        auto &bus = it->second;
        _out << bus.GetStopsCount() << " stops on route, " <<
             bus.GetUniqueStopsCount() << " unique stops, " <<
             setprecision(6) << bus.GetDistance(_stopsBase) << " route length" << endl;
    }
}

void RunTransportProcessor(istream &in, ostream &out)
{
    string request;
    int num;
    TransportProcessor tp(out);

    in >> num;
    getline(in, request);

    for (int i = 0; i < num; ++i)
    {
        getline(in, request);
        tp.ProcessDatabaseRequest(request);
    }

    in >> num;
    getline(in, request);

    for (int i = 0; i < num; ++i)
    {
        getline(in, request);
        tp.ProcessReadingRequest(request);
    }
}