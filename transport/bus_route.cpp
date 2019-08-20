#include "bus_route.h"

using namespace std;

BusRoute::BusRoute(const std::string &num, bool ring) : _number(num), _ring(ring) {}

void BusRoute::AssignStops(std::vector<std::string_view>::const_iterator first,
                 std::vector<std::string_view>::const_iterator last)
{
    for (auto it = first; it != last; it = next(it))
    {
        auto stop = std::string(*it);
        auto [stopIt, inserted] = _uniqueStops.insert(move(stop));
        _routeStops.push_back(&(*stopIt));
    }

    if (_ring && (*_routeStops.back() == *_routeStops.front()))
    {
        _routeStops.pop_back();
    }
}

void BusRoute::AssignStops(const std::vector<std::string_view> &stops)
{
    AssignStops(stops.begin(), stops.end());
}

size_t BusRoute::GetStopsCount() const
{
    if (_routeStops.size() < 2)
    {
        return _routeStops.size();
    }

    if (_ring)
    {
        return _routeStops.size() + 1;
    }
    else
    {
        return (_routeStops.size() * 2) - 1;
    }
}

size_t BusRoute::GetUniqueStopsCount() const
{
    return _uniqueStops.size();
}

const std::unordered_set<std::string> &BusRoute::GetUniqueStops() const
{
    return _uniqueStops;
}

void BusRoute::FillStopsInfo(StopsTable &stopsBase) const
{
    for (const auto &i : _uniqueStops)
    {
        stopsBase[i].AddBus(_number);
    }
}

int BusRoute::GetDistance(const StopsTable &stopsBase) const
{
    if (!_distance)
    {
        _distance = computeDistance(_routeStops.begin(), _routeStops.end(), stopsBase);
    }

    return _distance.value().real;
}

double BusRoute::GetCurvature(const StopsTable &stopsBase) const
{
    GetDistance(stopsBase);
    return _distance.value().real / _distance.value().straight;
}

const std::string &BusRoute::GetNumber() const
{
    return _number;
}

Distance BusRoute::distanceBetween(const std::string &stop1,
                                const std::string &stop2,
                                const StopsTable &stopsBase)
{
    return stopsBase.at(stop1).DistanceTo(stopsBase.at(stop2));
}

Distance BusRoute::computeDistance(std::vector<const std::string *>::const_iterator first,
                         std::vector<const std::string *>::const_iterator last,
                         const StopsTable &stopsBase) const
{
    if (first == last)
    {
        return {};
    }

    Distance result = {};
    auto second = next(first);
    auto start = first;

    for (; second != last; first = second, second = next(second))
    {
        result += distanceBetween(**first, **second, stopsBase);
    }

    if (_ring)
    {
        result += distanceBetween(**first, **start, stopsBase);
    }
    else
    {
        result *= 2;
    }

    return result;
}
