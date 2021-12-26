#ifndef SCENES_H
#define SCENES_H

#include "hittable_list.h"
#include "material.h"
#include "bvh.h"
#include "sphere.h"

hittable_list simple_scene(){
    hittable_list world;

    auto ground_material = make_shared<lambertain>(color(0.5,0.5,0.5));
    world.add(make_shared<sphere>(point3(0, -500, 0), 500, ground_material));

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertain>(color(0.1, 0.2, 0.5));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
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
 
                if (choose_mat < 0.5){
                    // Diffuse on a moving sphere
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertain>(albedo);
                    // Allow for motion blur
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                }else if (choose_mat < 0.8){
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

    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}

#endif