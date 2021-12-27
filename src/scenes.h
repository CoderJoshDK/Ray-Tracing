#ifndef SCENES_H
#define SCENES_H

#include "hittable_list.h"
#include "material.h"
#include "bvh.h"
#include "sphere.h"

hittable_list simple_scene(camera& cam, double aspect_ratio){
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5,0.5,0.5));
    world.add(make_shared<sphere>(point3(0, -500, 0), 500, ground_material));

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    // Camera
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto vfov = 20.0;
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera tempCam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    cam = tempCam;

    return world;
}

hittable_list random_scene(camera& cam, double aspect_ratio){
    hittable_list world;

    //auto ground_material = make_shared<checker_texture>(color(0.1,0.1,0.1), color(0.9, 0.9, 0.9));
    auto ground_material = make_shared<marble_texture>(8);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(ground_material)));

    for (int a = -11; a < 11; a++){
        for (int b = -11; b < 11; b++){
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9){
                shared_ptr<material> sphere_material;
 
                if (choose_mat < 0.5){
                    // Diffuse on a moving sphere
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    // Allow for motion blur
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                }else if (choose_mat < 0.8){
                    // Diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
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

    auto material2 = make_shared<lambertian>(make_shared<noise_texture>(2));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    // Camera
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto vfov = 20.0;
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera tempCam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    cam = tempCam;

    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}

hittable_list two_perlin_spheres(camera& cam, double aspect_ratio){
    hittable_list objects;

    auto groundTexture = make_shared<marble_texture>(2);
    auto pertext = make_shared<noise_texture>(2);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(groundTexture)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));


    // Camera
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto vfov = 20.0;
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;

    camera tempCam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    cam = tempCam;

    return objects;
}

hittable_list earth(camera& cam, double aspect_ratio){
    auto earth_texture = make_shared<image_texture>("../Images/earthmap.jpeg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

    // Camera
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto vfov = 20.0;
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;

    camera tempCam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    cam = tempCam;

    return hittable_list(globe);
}

#endif