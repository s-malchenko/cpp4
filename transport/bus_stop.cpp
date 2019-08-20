#include "bus_stop.h"
#include <cmath>

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
