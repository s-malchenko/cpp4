#pragma once
#include <string>

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
};
