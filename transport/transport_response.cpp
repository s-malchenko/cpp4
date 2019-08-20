#include "transport_response.h"
#include <iomanip>

using namespace std;
const string TransportResponse::ERROR_MESSAGE("not found");

BusResponse::BusResponse(const string &name,
                         unsigned int id,
                         bool error) : TransportResponse(name, id, error)
{
}

void BusResponse::Proceed(ostream &out) const
{
    out << "Bus " << _name << ": ";

    if (_error)
    {
        out << ERROR_MESSAGE;
    }
    else
    {
        out << _stopCount << " stops on route, " <<
            _uniqueStopCount << " unique stops, " <<
            _routeLength << " route length, " <<
            setprecision(7) << _curvature << " curvature";
    }

    out << endl;
}

void BusResponse::ProceedJSON(ostream &out) const
{
    out << endl;
}

BusResponse &BusResponse::Length(unsigned int len)
{
    _routeLength = len;
    return *this;
}

BusResponse &BusResponse::Stops(size_t count)
{
    _stopCount = count;
    return *this;
}

BusResponse &BusResponse::UniqueStops(size_t count)
{
    _uniqueStopCount = count;
    return *this;
}

BusResponse &BusResponse::Curvature(long double value)
{
    _curvature = value;
    return *this;
}

unsigned int BusResponse::Length() const { return _routeLength; }
size_t BusResponse::Stops() const { return _stopCount; }
size_t BusResponse::UniqueStops() const { return _uniqueStopCount; }
long double BusResponse::Curvature() const { return _curvature; }

StopResponse::StopResponse(const std::string &name, unsigned int id, bool error) : TransportResponse(name, id, error)
{
}

void StopResponse::Proceed(ostream &out) const
{
    out << "Stop " << _name << ": ";

    if (_error)
    {
        out << ERROR_MESSAGE;
    }
    else if (_buses.empty())
    {
        out << "no buses";
    }
    else
    {
        out << "buses ";

        for (auto it = _buses.begin(); it != _buses.end(); it = next(it))
        {
            out << *it;

            if (next(it) != _buses.end())
            {
                out << " ";
            }
        }
    }

    out << endl;
}

void StopResponse::ProceedJSON(std::ostream &out) const
{
    out << endl;
}

StopResponse &StopResponse::Buses(std::set<string_view> buses)
{
    _buses = buses;
    return *this;
}

const std::set<string_view> &StopResponse::Buses() const { return _buses; }
