#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaderClass.h"
#include "stb_image.h"

#include <iostream>
using namespace std; 
//#include <GL/gl.h>

// resize the viewport for rendering whenever the window itself is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window);

int main()
{
	// set up GLFW, set OpenGL version to 3, etc.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// make the window
	// width, height, window name, ignore last 2 params
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// make current context on current thread
	glfwMakeContextCurrent(window);

	// make this void function called every time the window is resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// register callback functions (such as for errors, input changes, etc. after window creation and before render loop

	// GLAD stuff, set right OS
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// set rendering size (top left corner, bottom right corner)
	glViewport(0, 0, 800, 600); 

	Shader shaderProgram("shaderB.vs", "shaderB.fs");

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int VAOID, VBOID, EBOID; 
	glGenVertexArrays(1, &VAOID);
	glGenBuffers(1, &EBOID);
	glGenBuffers(1, &VBOID);

	glBindVertexArray(VAOID);

	// bind it to the target, now calls to the array buffer will configure VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	// copies the vertex data into the buffer memory (stream = set once used little, static = set once used lots, dynamic = changed and used lots)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, EBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// params: 0 for location 0 given in vertex shader, 3 for 3 data attributes per vertex, floating data values
	//		don't normalize the data, we have 3 floats as the size of the storage between vertexes, and the offset from the array start is 0.
	// LOCATIONS
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// vertex attributes disabled by default
	glEnableVertexAttribArray(0);

	// COLORS
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//TEXTURES
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	unsigned int texture; 
	// can use array instead as second parameter
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// texturing wrapping options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// get the image as the filter
	int width, height, nrChannels;
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		cout << "loaded data" << endl; 
		// on currently bound texture of 2D (other dimension textures not affected), mipmap level, RBG value image,
		// width, height, always 0, image loaded with RGB values, stored as characters or bytes, actual data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl; 
	}
	stbi_image_free(data);

	// RENDER LOOP - every iteration is equal to one frame
	glfwSetWindowShouldClose(window, false);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// rendering commands here

		// RGB values, and alpha (transparancy)
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
		// clear the color
		glClear(GL_COLOR_BUFFER_BIT);


		glBindTexture(GL_TEXTURE_2D, texture); 
		shaderProgram.use();
		glBindVertexArray(VAOID);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
		
		glfwSwapBuffers(window); 
		// check for events and call callback functions
		glfwPollEvents(); 
	}

	glfwTerminate(); 
	return 0; 
}

void processInput(GLFWwindow* window)
{
	// GLFW function to get actual keypresses
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}