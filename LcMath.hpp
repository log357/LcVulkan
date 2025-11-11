#pragma once
#include <cmath>
#include <limits>

static bool IsZero(float f) {
    return std::fabs(f) < std::numeric_limits<float>::epsilon();
}
static bool IsZero(double d) {
    return std::fabs(d) < std::numeric_limits<double>::epsilon();
}
