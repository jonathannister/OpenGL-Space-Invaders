#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "stb_image.h"
#include "shaderClass.h"

using namespace std; 
//#include <GL/gl.h>

#include <iostream>

// resize the viewport for rendering whenever the window itself is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// process input function to keep key press inputs organized
void processInput(GLFWwindow* window)
{
	// GLFW function to get actual keypresses
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

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

	Shader ourShader("shaderB.vs", "shaderB.fs");

	

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indexes[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBOID, EBOID, VAOID; 
	glGenBuffers(1, &VBOID);
	glGenBuffers(1, &EBOID);
	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);


	// bind it to the target, now calls to the array buffer will configure VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	// copies the vertex data into the buffer memory (stream = set once used little, static = set once used lots, dynamic = changed and used lots)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// EBOs or element buffer objects tell what order we want to draw vertexes in so that we can guarantee there is no overlap
	// aka indexed drawing
	glGenBuffers(1, &EBOID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOID); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);


	// now we have to specify how to handle the vertex data
	// params: 0 for location 0 given in vertex shader, 3 for 3 data attributes per vertex, floating data values
	//		don't normalize the data, we have 3 floats as the size of the storage between vertexes, and the offset from the array start is 0.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	unsigned int texture1, texture2;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);



	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	glfwSetWindowShouldClose(window, false);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// rendering commands here

		// RGB values, and alpha (transparancy)
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
		// clear the color
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		
		ourShader.use(); 
		glBindVertexArray(VAOID);
		// draw elements used for EBOs, 6 for total number of indexes and 0 for array offset
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		

		// swap the back buffer to the front
		// rendering commands always draw to the back buffer since drawing goes one pixel at a time and would look strange otherwise
		glfwSwapBuffers(window); 
		// check for events and call callback functions
		glfwPollEvents(); 
	}

	glfwTerminate(); 
	return 0; 
}