#include <Windows.h>
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

using namespace glm;

#include "sphere_scene.h"
#include "Object.h"
#include "viewPipeline.h"
// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 512;
int Height = 512;
std::vector<float> OutputImage;
// -------------------------------------------------


// non optimized version
void rasterize_triangle(
	std::vector<float>& framebuffer,
	glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
	glm::vec3 color,
	int width, int height
) {
	int xmin = std::max(0, (int)std::floor(std::min({ v0.x, v1.x, v2.x })));
	int xmax = std::min(width - 1, (int)std::ceil(std::max({ v0.x, v1.x, v2.x })));
	int ymin = std::max(0, (int)std::floor(std::min({ v0.y, v1.y, v2.y })));
	int ymax = std::min(height - 1, (int)std::ceil(std::max({ v0.y, v1.y, v2.y })));

	for (int y = ymin; y <= ymax; y++) {
		for (int x = xmin; x <= xmax; x++) {
			glm::vec2 p = glm::vec2(x + 0.5f, y + 0.5f);

			glm::vec2 a = glm::vec2(v0.x, v0.y);
			glm::vec2 b = glm::vec2(v1.x, v1.y);
			glm::vec2 c = glm::vec2(v2.x, v2.y);

			float denom = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
			float beta = ((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y)) / denom;
			float gamma = ((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y)) / denom;
			float alpha = 1.0f - beta - gamma;

			if ((beta + gamma) <= 1 && beta >= 0 && gamma >= 0) {
				int idx = (y * width + x) * 3;
				if (x >= 0 && x < width && y >= 0 && y < height && idx + 2 < framebuffer.size()) {
					framebuffer[idx + 0] = color.r;
					framebuffer[idx + 1] = color.g;
					framebuffer[idx + 2] = color.b;
				}
			}
		}
	}

}
void render()
{
	OutputImage.clear();
	OutputImage.resize(Width * Height * 3, 0);  
	ObjectData data = create_scene(Height, Width);
	float model[4][4] = {
	{2, 0, 0,  0},
	{0, 2, 0,  0},
	{0, 0, 2, -7},
	{0, 0, 0,  1}
	};
	Camera camera = {
	{0, 0, 0}, // eye
	{1, 0, 0}, // u
	{0, 1, 0}, // v
	{0, 0, 1}  // w
	};

	float l = -0.1f;
	float r =  0.1f;
	float b = -0.1f;
	float t =  0.1f;
	float n = -0.1f;
	float f = -1000;
	glm::vec3 final;

	std::vector<vec3> screen_vertices;

	for (int i = 0; i < data.numVertices; i++)
	{
		float vertex[4] =
		{
			data.vertexBuffer[3 * i + 0],
			data.vertexBuffer[3 * i + 1],
			data.vertexBuffer[3 * i + 2],
			1.0f
		};

		float* model_v = M_Model(vertex, model);
		float* camera_v = M_Camera(model_v, camera);
		float* persp_v = M_Perspective(camera_v, n, f);
		float* ortho_v = M_Orthograph(persp_v, l, r, b, t, n, f);
		float* screen_v = M_Viewport(ortho_v, Width, Height);
		final = glm::vec3(
			screen_v[0] / screen_v[3],
			screen_v[1] / screen_v[3],
			screen_v[2] / screen_v[3]
		);

		screen_vertices.push_back(final);
	}
	for (int i = 0; i < data.numTriangles; i++)
	{
		//인덱스 범위 체크
		int k0 = data.indexBuffer[3 * i + 0];
		int k1 = data.indexBuffer[3 * i + 1];
		int k2 = data.indexBuffer[3 * i + 2];

		if (k0 < 0 || k1 < 0 || k2 < 0 ||
			k0 >= (int)screen_vertices.size() ||
			k1 >= (int)screen_vertices.size() ||
			k2 >= (int)screen_vertices.size())
		{
			continue;
		}

		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		rasterize_triangle(OutputImage,
			screen_vertices[k0],
			screen_vertices[k1],
			screen_vertices[k2],
			color, Width, Height
		);
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
	OutputImage.reserve(Width * Height * 3);
	render();
}


int main(int argc, char* argv[])
{
	// -------------------------------------------------
	// Initialize Window
	// -------------------------------------------------

	GLFWwindow* window;

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
		//and ends.
		// -------------------------------------------------------------

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
