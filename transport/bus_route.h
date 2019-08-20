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
    BusRoute(const std::string &num, bool ring = false) : _number(num), _ring(ring) {}

    void AssignStops(std::vector<std::string_view>::const_iterator first,
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

    void AssignStops(const std::vector<std::string_view> &stops)
    {
        AssignStops(stops.begin(), stops.end());
    }

    size_t GetStopsCount() const
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

    size_t GetUniqueStopsCount() const
    {
        return _uniqueStops.size();
    }

    const std::unordered_set<std::string> &GetUniqueStops() const
    {
        return _uniqueStops;
    }

    void FillStopsInfo(StopsContainer &stopsBase) const
    {
        for (const auto &i : _uniqueStops)
        {
            stopsBase[i].AddBus(_number);
        }
    }

    double GetDistance(const StopsContainer &stopsBase) const
    {
        if (!_distance)
        {
            _distance = computeDistance(_routeStops.begin(), _routeStops.end(), stopsBase);
        }

        return _distance.value();
    }

    const std::string &GetNumber() const
    {
        return _number;
    }

protected:
    const std::string _number;
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
};
