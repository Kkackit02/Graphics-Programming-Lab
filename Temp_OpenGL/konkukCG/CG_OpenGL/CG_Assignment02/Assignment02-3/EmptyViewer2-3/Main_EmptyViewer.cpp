#include <Windows.h>
#include <math.h> 
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Ray.h"
#include "Camera.h"
#include "Light.h"

#include <cstdlib>
#include <ctime>
using namespace glm;

// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 512;
int Height = 512;
std::vector<float> OutputImage;
// -------------------------------------------------




// Function Signatures
glm::vec3 applyGammaCorrection(const glm::vec3& color, float gamma = 2.2f) {
	return glm::pow(color, glm::vec3(1.0f / gamma));
}


void render()
{

	//Create our image. We don't want to do this in 
	//the main loop since this may be too slow and we 
	//want a responsive display of our beautiful image.
	//Instead we draw to another buffer and copy this to the 
	//framebuffer using glDrawPixels(...) every refresh
	OutputImage.clear();


	Material m_P1 = Material(glm::vec3(0.2, 0.2, 0.2), glm::vec3(1,1,1), glm::vec3(0.0, 0.0, 0.0), 0.0);
	Material m_S1 = Material(glm::vec3(0.2, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0.0, 0.0, 0.0), 0.0);
	Material m_S2 = Material(glm::vec3(0, 0.2, 0), glm::vec3(0, 0.5, 0), glm::vec3(0.5, 0.5, 0.5), 32);
	Material m_S3 = Material(glm::vec3(0, 0, 0.2), glm::vec3(0, 0, 1), glm::vec3(0,0,0), 0);

	Surface* S1 = new Sphere(m_S1, vec3(-4.0, 0.0, -7.0), 1.0);
	Surface* S2 = new Sphere(m_S2, vec3(0.0, 0.0, -7.0), 2.0);
	Surface* S3 = new Sphere(m_S3, vec3(4.0, 0.0, -7.0), 1.0);
	Surface* P = new Plane(m_P1, glm::vec3(0.0, -2.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	Light* L = new Light(glm::vec3(-4, 4, -3), glm::vec3(1, 1, 1));

	float gammaValue = 2.2f;
	const int samples = 64;
	// lookAt 방향 수정
	Camera camera(
		glm::vec3(0, 0, 0), // eye point
		glm::vec3(0, 0, -1),  // W direction, look -w
		glm::vec3(0, 1, 0), //V direction
		-0.1f, 0.1f, -0.1f, 0.1f, // l , r, b ,t
		0.1f, Width, Height //d, nx, ny
	);

	Scene scene(camera);
	scene.surfaces.push_back(P);
	scene.surfaces.push_back(S1);
	scene.surfaces.push_back(S2);
	scene.surfaces.push_back(S3);


	for (int j = 0; j < Height; ++j) 
	{
		for (int i = 0; i < Width; ++i) 
		{
			// ---------------------------------------------------
			// --- Implement your code here to generate the image
			// ---------------------------------------------------

			

			//Ray ray = camera.getRay(i,j);

			vec3 hit_point, normal;
			Surface* hit_surface = nullptr;

			float t_hit = 0.0f;

			vec3 color = glm::vec3(0,0,0); 
			for (int s = 0; s < samples; ++s) {

				float offset_i = static_cast<float>(rand()) / RAND_MAX; 
				float offset_j = static_cast<float>(rand()) / RAND_MAX;

				Ray ray = camera.getRay(i+offset_i, j+offset_j);
				color += scene.trace(ray, hit_point, normal, hit_surface, L, t_hit);
			}
			color /= float(samples); // 평균

			 
			color = applyGammaCorrection(color, gammaValue);
			// set the color
			OutputImage.push_back(color.x); // R
			OutputImage.push_back(color.y); // G
			OutputImage.push_back(color.z); // B
		}
	}
}



void resize_callback(GLFWwindow*, int nw, int nh) 
{
	//This is called in response to the window resizing.
	//The new width and height are passed in so we make 
	//any necessary changes:
	Width = nw;
	Height = nh;
	//Tell the viewport to use all of our screen estate
	glViewport(0, 0, nw, nh);

	//This is not necessary, we're just working in 2d so
	//why not let our spaces reflect it?
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, static_cast<double>(Width)
		, 0.0, static_cast<double>(Height)
		, 1.0, -1.0);

	//Reserve memory for our render so that we don't do 
	//excessive allocations and render the image
	OutputImage.reserve(Width * Height * 3); //rgb 때문에 *3
	render();
}


int main(int argc, char* argv[])
{
	// -------------------------------------------------
	// Initialize Window
	// -------------------------------------------------

	GLFWwindow* window;

	srand(static_cast<unsigned int>(time(0)));
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(Width, Height, "OpenGL Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//We have an opengl context now. Everything from here on out 
	//is just managing our window or opengl directly.

	//Tell the opengl state machine we don't want it to make 
	//any assumptions about how pixels are aligned in memory 
	//during transfers between host and device (like glDrawPixels(...) )
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//We call our resize function once to set everything up initially
	//after registering it as a callback with glfw


	//------------------------------------------------
	glfwSetFramebufferSizeCallback(window, resize_callback);
	resize_callback(NULL, Width, Height);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// -------------------------------------------------------------
		//Rendering begins!
		glDrawPixels(Width, Height, GL_RGB, GL_FLOAT, &OutputImage[0]);
		//하나하나 찍 어내면서 OUTPUTIMAGE에 저장
		//and ends.
		// ---------------------------------------	----------------------

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		//Close when the user hits 'q' or escape
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
			|| glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}




