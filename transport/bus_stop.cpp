#include "bus_stop.h"
#include <cmath>
#include <utility>

using namespace std;

static inline long double degreeToRad(long double deg)
{
    return deg * PI / 180;
}

long double Coordinates::DistanceTo(const Coordinates &other) const
{
    long double lonDelta = abs(degreeToRad(longitude) - degreeToRad(other.longitude));
    long double lat1 = degreeToRad(latitude);
    long double lat2 = degreeToRad(other.latitude);
    return acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lonDelta)) * EARTH_RADIUS;
}

Distance &Distance::operator+=(const Distance &other)
{
    real += other.real;
    straight += other.straight;
    return *this;
}

Distance &Distance::operator*=(long coeff)
{
    real *= coeff;
    straight *= coeff;
    return *this;
}

BusStop::BusStop(const string &n, Coordinates l) : name(n), location(l)
{
}

BusStop::BusStop(string &&n, Coordinates l) : name(move(n)), location(l)
{
}

void BusStop::AddBus(std::string_view bus)
{
    _buses.insert(bus);
}

const std::set<std::string_view> &BusStop::GetBuses() const
{
    return _buses;
}

Distance BusStop::DistanceTo(const BusStop &other) const
{
    auto it = _distances.find(other.name);
    unsigned long dist;

    if (it != _distances.end())
    {
        dist = it->second;
    }
    else
    {
        dist = other._distances.at(name);
    }

    return {location.DistanceTo(other.location), dist};
}

void BusStop::AssignDistances(std::unordered_map<std::string, unsigned int> &&distances)
{
    _distances = move(distances);
}

void AssignDistances(DistanceTable &distances, StopsTable &stopsBase)
{
    for (auto &[stop, table] : distances)
    {
        stopsBase.at(stop).AssignDistances(move(table));
    }
}
