#pragma once
#include <string>

static const double PI = 3.1415926535;
static const unsigned int EARTH_RADIUS = 6'371'000;

struct Coordinates
{
    double latitude;
    double longitude;
    double DistanceTo(const Coordinates &other) const;
};

struct BusStop
{
    std::string name;
    Coordinates location;
};
