#pragma once
#include <string>
#include <set>
#include <string_view>

static const long double PI = 3.1415926535;
static const unsigned int EARTH_RADIUS = 6'371'000;

struct Coordinates
{
    long double latitude;
    long double longitude;
    long double DistanceTo(const Coordinates &other) const;
};

struct BusStop
{
    std::string name;
    Coordinates location;
    BusStop(const std::string &n = "", Coordinates l = {});
    BusStop(std::string &&n, Coordinates l = {});
    void AddBus(std::string_view bus);
    const std::set<std::string_view> &GetBuses() const;
private:
    std::set<std::string_view> _buses;
};
