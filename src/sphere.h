#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "rtweekend.h"

class sphere : public hittable{
    public:
        sphere(){}
        sphere(point3 cen, float r, shared_ptr<material> m) 
            : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

        point3 center;
        float radius;
        shared_ptr<material> mat_ptr;

    private:
        static void get_sphere_uv(const point3& p, double& u, double& v){
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;

            u = phi/ (2*pi);
            v = theta / pi;
        }
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    
    auto discriminant = half_b*half_b - a*c;
    
    if (discriminant < 0) return false;
    
    auto sqrtd = sqrt(discriminant);
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root){
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool sphere::bounding_box(double time0, double time1, aabb& output_box) const{
    output_box = aabb(
        center - vec3(radius),
        center + vec3(radius)
    );
    return true;
}


class moving_sphere : public hittable{
    public:
        moving_sphere(){}
        moving_sphere(
            point3 cen0, point3 cen1,
            double ti0, double ti1,
            double r, shared_ptr<material> m) 
            : center0(cen0), center1(cen1), time0(ti0), time1(ti1), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;


        point3 center(double time) const;

        point3 center0, center1;
        double time0, time1;
        double radius;
        shared_ptr<material> mat_ptr;
};

point3 moving_sphere::center(double time) const{
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center(r.time());
    
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    
    auto discriminant = half_b*half_b - a*c;
    
    if (discriminant < 0) return false;
    
    auto sqrtd = sqrt(discriminant);
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root){
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool moving_sphere::bounding_box(double _time0, double _time1, aabb& output_box) const{
    aabb box0(center(_time0) - vec3(radius), center(_time0) + vec3(radius));
    aabb box1(center(_time1) - vec3(radius), center(_time1) + vec3(radius));
    output_box = surrounding_box(box0, box1);
    return true;
}


#endif