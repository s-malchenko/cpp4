#pragma once

#include <cmath>

inline bool doublesEqual(double lhs, double rhs, double precision = 0.0001)
{
    return abs(lhs - rhs) <= precision;
}
