# Ray Tracing

[![language](https://img.shields.io/badge/-c++17-black?logo=c%2B%2B&17style=social)](https://en.cppreference.com/w/cpp/17)
![commit](https://img.shields.io/github/last-commit/CoderJoshDK/Ray-Tracing?label=last%20update)

---

## Impilmented Topics
 - Objects:  
   - Sphere
 - Materials:  
   - Diffuse (lambertian)
   - Metal
   - Dielectrics
   - Textures
     - Perlin Noise
     - Image Mapping
 - Camera
   - Orientation
   - Defocus blur (depth of field)
   - Motion Blur
 - Optimization
   - Bounding Volume Hierarchy (BVH)
   - Multithreading

 
## Output
Output is in .ppm format. Right now, to get an output image, run build. The exe will output `image.ppm`.  
To change the quality of the image, change `samples_per_pixel` or `image_width`.  

Output from *random_scene*  
![sampleRender2](Images/sampleRender2.png)  
It is intresting to note that glass spheres do not cast shadows. This is physically correct.


### To-Do
 - ***Lights***
 - Rectangles
 - Volumes  

----------
![WIP](https://img.shields.io/static/v1?label=Status&message=WIP&color=red)
