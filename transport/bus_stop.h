#pragma once
#include <string>

struct Coordinates
{
    double latitude;
    double longitude;
    double DistanceTo(const Coordinates &other);
};

struct BusStop
{
    std::string name;
    Coordinates location;
};
