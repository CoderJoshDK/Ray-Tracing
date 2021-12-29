#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "scenes.h"


#include <iostream>
#include <chrono>
#include <future>
#include <fstream>

color ray_color(const ray& r, const color& background, const hittable& world, int depth){
    hit_record rec;

    if (depth <= 0)
        return color(0,0,0);


    if (!world.hit(r, 0.001, MAXFLOAT, rec))
        return background;
    
    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;
        
    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
    

    /*
    vec3 unit_direction = unit_vector(r.direction());
    auto t = .5* (unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0,1.0,1.0) + t*color(.5,.7,1.0);
    */
}


struct RayResult{
    unsigned int index;
    color col;
};

int main(int argc, char const *argv[])
{
    // Time how long it takes
    auto start = std::chrono::steady_clock::now();


    // Camera
    camera cam;

    // World
    auto aspect_ratio = 16.0 / 9.0;
    hittable_list world;
    color background(0);
    switch(5){
        case 0:
            world = random_scene(cam, background, aspect_ratio);
            break;
        case 1:
            world = simple_scene(cam, background, aspect_ratio);
            break;
        case 2:
            world = two_perlin_spheres(cam, background, aspect_ratio);
            break;
        case 3:
            world = earth(cam, background, aspect_ratio);
            break;
        case 4:
            world = simple_light(cam, background, aspect_ratio);
            break;
        case 5:
            world = cornell_box(cam, background, aspect_ratio);
            break;
    }


    // Image
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 200;
    const int max_depth = 50;

    color* image = new color[image_width * image_width];
    memset(&image[0], 0, image_width * image_height * sizeof(color));
    
    std::ofstream imageFile;

    // Render


    std::mutex mutex;
    std::condition_variable cvResults;
    std::vector<std::future<RayResult>> m_futures;

    //for (int j = image_height - 1; j >= 0; j--){
    for (int j = 0; j < image_height; j++){
        std::cerr << "\rScanlines remaining: " << image_height - j - 1 << ' ' << std::flush;
        for (int i = 0; i < image_width; i++){
            // Each pixel will be dealt with by a thread
            auto future = std::async(std::launch::async | std::launch::deferred,
                [&cam, &world, &samples_per_pixel, &background, i, j, image_width, image_height, &cvResults]() -> RayResult{
                const unsigned int index = j * image_width + (image_width - i);
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; s++){
                    auto u = (i + random_double()) / (image_width - 1);
                    auto v = (j + random_double()) / (image_height - 1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, background, world, max_depth);
                }
                pixel_color /= float(samples_per_pixel);

                RayResult result;
                result.index = index;
                result.col = color(
                    clamp(sqrt(pixel_color.r()), 0.0, 0.999), 
                    clamp(sqrt(pixel_color.g()), 0.0, 0.999), 
                    clamp(sqrt(pixel_color.b()), 0.0, 0.999));
                return result;
            });
            
            {
                std::lock_guard<std::mutex> lock(mutex);
                m_futures.push_back(std::move(future));
            }
        }
    }

    auto timeout = std::chrono::milliseconds(10);
    
    const int pixelcount = image_width * image_height;
    {
        std::unique_lock<std::mutex> lock(mutex);
        cvResults.wait(lock, [&m_futures, &pixelcount]{
            return m_futures.size() == pixelcount;
        });
    }

    //Make image
    for (auto& rr : m_futures){
        RayResult result = rr.get();
        image[result.index] = result.col;
    }
    
    imageFile.open("image.ppm");
    imageFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (auto i = pixelcount-1; i >= 0; i--){
        out_color(imageFile, image[i]);
    }

    imageFile.close();
    
    auto end = std::chrono::steady_clock::now();
    std::cerr << "\nDone in : " 
    << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()/1000.0 
    << " seconds\n";

    delete [] image;
    return 0;
}
