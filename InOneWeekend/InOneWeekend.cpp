#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"


#include <iostream>
#include <chrono>
#include <future>

color ray_color(const ray& r, const hittable& world, int depth){
    hit_record rec;

    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)){
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        
        return color(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = .5* (unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0,1.0,1.0) + t*color(.5,.7,1.0);
}

color get_ray_color(
        const int image_width, 
        const int image_height, 
        const camera& cam, 
        const hittable_list& world, 
        const int max_depth,
        int i, int j
    ){
    auto u = (i + random_double()) / (image_width - 1);
    auto v = (j + random_double()) / (image_height - 1);
    ray r = cam.get_ray(u, v);
    return ray_color(r, world, max_depth);
}

hittable_list random_scene(){
    hittable_list world;

    auto ground_material = make_shared<lambertain>(color(0.5,0.5,0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++){
        for (int b = -11; b < 11; b++){
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9){
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8){
                    // Diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertain>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }else if (choose_mat < 0.95){
                    // Metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }else{
                    // Glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertain>(color(0.1, 0.2, 0.5));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

int main(int argc, char const *argv[])
{
    // Time how long it takes
    auto start = std::chrono::steady_clock::now();

    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1280;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    // World
    auto world = random_scene();
    
    // Camera
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--){
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; i++){
            color pixel_color(0,0,0);
            std::vector<std::future<color>> colors;

            for (int s = 0; s < samples_per_pixel; s++){
                colors.push_back(std::async(&get_ray_color, image_width , image_height, cam, world, max_depth, i, j));
                //pixel_color += get_ray_color(image_width , image_height, cam, world, max_depth, i, j);
            }
            for (auto & c : colors)
                pixel_color += c.get();
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    
    auto end = std::chrono::steady_clock::now();
    std::cerr << "\nDone in : " 
    << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()/1000.0 
    << " seconds\n";

    return 0;
}
