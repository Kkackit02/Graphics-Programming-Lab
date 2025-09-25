# Assignment 01

## The scene consists of the following four objects:
- Plane P , with equation y = −2.
- Sphere S1, with center at (−4, 0, −7) and radius 1.
- Sphere S2, with center at (0, 0, −7) and radius 2.
- Sphere S3, with center at (4, 0, −7) and radius 1.
## Camera
- Assume a perspective camera
- eye point at e = (0, 0, 0)
- orientation given by u = (1, 0, 0), v = (0, 1, 0) and w = (0, 0, 1).
- (Note that the camera is looking along the direction −w.)
- Assume that the viewing region on the image plane is defined by l = −0.1,
- r = 0.1, b = −0.1, t = 0.1, and d = 0.1.
- Also assume that the image resolution is 512 × 512 (i.e., nx = ny = 512)

### Implement the Following Classes:
- Ray
- Camera
- Plane
- Sphere
- Surface
- Scene
- Any classes if needed

***


- ![Object Allocation](https://github.com/user-attachments/assets/5f2b3f04-d2b7-457f-993a-abd07645b102)  
  **First**, I allocate objects — specifically, **3 spheres**, **1 plane**, and a **camera**.  
  These objects are assigned their respective properties and then **pushed into the scene**.

- ![Ray Tracing](https://github.com/user-attachments/assets/83a5fb84-18fa-4140-b80c-5e30a01a041f)  
  **Next**, for every pixel in the scene (from coordinates **(0, 0)** to **(512, 512)**),  
  a **ray is cast from the camera** through that pixel to check for intersections with the objects.

  - If the ray **hits** an object, the pixel is colored **white**.  
  - If the ray **does not hit** any object, the pixel is colored **black**.

  After processing all pixels, the resulting image is displayed.



***
# Class Description

## Camera

![image](https://github.com/user-attachments/assets/c15ac9a6-cf59-4298-acba-2d6f79fd117b)
![image](https://github.com/user-attachments/assets/638c47be-98c7-425c-9a97-a93532edc428)
![image](https://github.com/user-attachments/assets/4ca99d2f-093c-48bf-a045-9a033b05c8e8)

The `Camera` class provides a `getRay()` method.

This method generates a ray originating from the camera's position and directed toward a target pixel on the image plane.  
It works by simulating a ray shot through the given pixel coordinates, allowing the system to determine what the camera "sees" at that position.

## Ray
![image](https://github.com/user-attachments/assets/c4191d8e-c50b-47ce-9b77-3d699d75e0b8)

The `Ray` class represents a ray with an origin and a direction in 3D space.  
It provides transformation functions that allow it to be transformed into different coordinate spaces if needed.
This ray is primarily used for **intersection tests** with scene objects such as spheres and planes.


## Surface
![image](https://github.com/user-attachments/assets/08a3821d-3062-4d67-9517-7e06071add80)

The `Surface` class is a **virtual base class**, which serves as a common interface for various objects in the scene,  
such as `Sphere` and `Plane`.


## Plane
![image](https://github.com/user-attachments/assets/e760770e-900c-4ba0-85e7-713b6dbe3cde)
![image](https://github.com/user-attachments/assets/478bc693-012c-4154-bfe9-317b9e5ba00e)

The `Plane` class is a **derived class** of the `Surface` base class.  
An instance of this class represents a plane in the 3D world and can interact with rays in the scene.

It implements its own intersection logic, allowing rays to detect and respond to planar surfaces.

## Sphere

![image](https://github.com/user-attachments/assets/0a8f415f-fe39-4472-8864-f8650f6574a7)
![image](https://github.com/user-attachments/assets/f0a4bf29-5753-4b33-9836-476f296e6d9d)
![image](https://github.com/user-attachments/assets/67eb8e7f-b034-4dab-9857-fc09aecd7c63)

The `Sphere` class is a **derived class** of the `Surface` base class.  
An instance of this class represents a **sphere object in 3D space**, and it can interact with rays cast from the camera.

It implements its own **ray-sphere intersection logic**, allowing the rendering system to determine when and where a ray hits a spherical surface.

## Scene

![image](https://github.com/user-attachments/assets/090699a1-061b-4d9b-948d-513fb6c8b61d)

The `Scene` object contains both the **camera** and a list of **renderable objects** (such as spheres and planes).  
Using these, the scene can perform **ray tracing** by generating rays from the camera and testing them against the objects.

Based on the returned data (e.g., the `hit_surface`),  
the scene determines how each pixel should be colored, and constructs the final rendered image.
