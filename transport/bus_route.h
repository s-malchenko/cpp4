#pragma once

#include "bus_stop.h"

#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <utility>

template <typename StopsContainer>
class BusRoute
{
public:
    BusRoute(int num, bool ring = false) : _number(num), _ring(ring) {}

    void AssignStops(std::vector<std::string_view>::const_iterator first,
                     std::vector<std::string_view>::const_iterator last)
    {
        for (auto it = first; it != last; it = next(it))
        {
            auto stop = std::string(*it);
            auto [stopIt, inserted] = _uniqueStops.insert(move(stop));
            _routeStops.push_back(&(*stopIt));
        }

        if (_ring && *_routeStops.back() == *_routeStops.front())
        {
            _routeStops.pop_back();
        }
    }

    size_t GetUniqueStopsCount() const
    {
        return _uniqueStops.size();
    }

    double GetDistance(const StopsContainer &stopsBase) const
    {
        if (!_distance)
        {
            _distance = computeDistance(_routeStops.begin(), _routeStops.end(), stopsBase);
        }

        return _distance.value();
    }

    int GetNumber() const
    {
        return _number;
    }

protected:
    const int _number;
    const bool _ring;
    mutable std::optional<double> _distance;
    std::vector<const std::string *> _routeStops;
    std::unordered_set<std::string> _uniqueStops;

    static double distanceBetween(const std::string &stop1,
                                  const std::string &stop2,
                                  const StopsContainer &stopsBase)
    {
        return stopsBase.at(stop1).location.DistanceTo(stopsBase.at(stop2).location);
    }

    double computeDistance(std::vector<const std::string *>::const_iterator first,
                                  std::vector<const std::string *>::const_iterator last,
                                  const StopsContainer &stopsBase) const
    {
        if (first == last)
        {
            return 0;
        }

        double result = 0;
        auto second = next(first);
        auto start = first;

        for (auto second = next(first);
                second != last;
                first = second, second = next(second))
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
};
