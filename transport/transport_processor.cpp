#include "transport_processor.h"
#include "input_parser.h"

#include <iomanip>
#include <iterator>

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
        _stopsBase.insert(make_pair(move(name), BusStop{name, site}));
    }
}

void TransportProcessor::PrepareDatabase()
{
    for (const auto &[k, bus] : _busesBase)
    {
        bus.FillStopsInfo(_stopsBase);
    }
}

void TransportProcessor::ProcessReadingRequest(const string &request)
{
    string_view line(request);
    auto cmd = GetPart(line);
    auto arg = string(line);

    if (cmd == "Bus")
    {
        auto it = _busesBase.find(arg);

        _out << "Bus " << arg << ": ";

        if (it == _busesBase.end())
        {
            _out << "not found";
        }
        else
        {
            auto &bus = it->second;
            _out << bus.GetStopsCount() << " stops on route, " <<
                 bus.GetUniqueStopsCount() << " unique stops, " <<
                 setprecision(6) << bus.GetDistance(_stopsBase) << " route length";
        }
    }
    else if (cmd == "Stop")
    {
        auto it = _stopsBase.find(arg);

        _out << "Stop " << arg << ": ";

        if (it == _stopsBase.end())
        {
            _out << "not found";
        }
        else
        {
            const auto &buses = it->second.GetBuses();

            if (buses.empty())
            {
                _out << "no buses";
            }
            else
            {
                _out << "buses ";

                for (auto it = buses.begin(); it != buses.end(); it = next(it))
                {
                    _out << *it;

                    if (next(it) != buses.end())
                    {
                        _out << " ";
                    }
                }
            }
        }
    }

    _out << endl;
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

    tp.PrepareDatabase();
    in >> num;
    getline(in, request);

    for (int i = 0; i < num; ++i)
    {
        getline(in, request);
        tp.ProcessReadingRequest(request);
    }
}