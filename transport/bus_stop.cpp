#include "bus_stop.h"
#include <cmath>

static const double PI = 3.1415926535;
static const unsigned int EARTH_RADIUS = 6'371'000;

static inline double degreeToRad(double deg)
{
    return deg * PI / 180.0;
}

double Coordinates::DistanceTo(const Coordinates &other)
{
    double lonDelta = degreeToRad(abs(longitude - other.longitude));
    double lat1 = degreeToRad(latitude);
    double lat2 = degreeToRad(other.latitude);

    return EARTH_RADIUS * acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lonDelta));
}
