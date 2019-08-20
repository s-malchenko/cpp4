#pragma once

#include "bus_stop.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class BusRoute
{
public:
    BusRoute(const std::string &num, bool ring = false);
    void AssignStops(std::vector<std::string_view>::const_iterator first,
                     std::vector<std::string_view>::const_iterator last);
    void AssignStops(const std::vector<std::string_view> &stops);
    size_t GetStopsCount() const;
    size_t GetUniqueStopsCount() const;
    const std::unordered_set<std::string> &GetUniqueStops() const;
    void FillStopsInfo(StopsTable &stopsBase) const;
    int GetDistance(const StopsTable &stopsBase) const;
    double GetCurvature(const StopsTable &stopsBase) const;
    const std::string &GetNumber() const;
protected:
    const std::string _number;
    const bool _ring;
    mutable std::optional<Distance> _distance;
    std::vector<const std::string *> _routeStops;
    std::unordered_set<std::string> _uniqueStops;
    static Distance distanceBetween(const std::string &stop1,
                                  const std::string &stop2,
                                  const StopsTable &stopsBase);
    Distance computeDistance(std::vector<const std::string *>::const_iterator first,
                                  std::vector<const std::string *>::const_iterator last,
                                  const StopsTable &stopsBase) const;
};

using BusTable = std::unordered_map<std::string, BusRoute>;
