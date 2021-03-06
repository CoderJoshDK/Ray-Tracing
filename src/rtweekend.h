#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include<cmath>
#include<limits>
#include<memory>
#include <cstdlib>

using std::shared_ptr, std::make_shared, std::sqrt;

#define pi 3.1415926535897932385

inline double degrees_to_radians(double degrees){
    return degrees * pi / 180.0;
}

// [0, 1)
inline double random_double(){
    return rand() / (RAND_MAX + 1.0);
}
// [min, max)
inline double random_double(double min, double max){
    return min + (max-min)*random_double();
}
// [min, max]
inline int random_int(int min, int max){
    return static_cast<int>(random_double(min, max + 1));
}

inline double clamp(double x, double min, double max){
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

#include "ray.h"
#include "vec3.h"

#endif