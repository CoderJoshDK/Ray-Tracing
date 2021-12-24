#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>
#include <chrono>

color ray_color(const ray& r, const hittable& world);

int main(int argc, char const *argv[])
{
    // Time how long it takes
    auto start = std::chrono::steady_clock::now();

    // Image
    const auto aspect_ration = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ration);

    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));
    
    // Camera
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ration * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0,0,0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--){
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; i++){
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v*vertical - origin);
            color pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::cerr << "\nDone in : " 
    << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()/1000.0 
    << " seconds\n";

    return 0;
}


color ray_color(const ray& r, const hittable& world){
    hit_record rec;
    if (world.hit(r, 0, infinity, rec)){
        return .5 * (rec.normal + color(1,1,1));
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = .5* (unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0,1.0,1.0) + t*color(.5,.7,1.0);
}