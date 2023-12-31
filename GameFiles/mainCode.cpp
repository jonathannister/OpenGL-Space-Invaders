// Completed with research done from Learn OpenGL by Joey DeVries - https://learnopengl.com/
// Utility functions such as image loading and structure of the camera class are taken from the textbook.

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "camera.h"
#include "stb_image.h"
#include "shaderClass.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <time.h>


using namespace std; 


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); 
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);


float deltaTime = 0.0f; 
float lastFrame = 0.0f; 

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
bool firstMouse = true;
//float yaw = -90.0f;	
//float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

bool fireball = false;
glm::vec3 fireballCoords = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 fireballDirection = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 shipCoords = camera.Position + glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 lightPos(0.0f, 5.0f, 20.0f);
bool lost = false;



int main()
{
	// set up GLFW, set OpenGL version to 3, etc.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// make the window
	// width, height, window name, ignore last 2 params
	GLFWwindow* window = glfwCreateWindow(800, 600, "IP Game", NULL, NULL);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback); 
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
	Shader lightCubeShader("lightShader.vs", "lightShader.fs"); 


	float vertices[] = {
		// positions          // normal vectors       // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// configure light's VAO
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	
	// load textures from images
	unsigned int diffuseMap = loadTexture("asteroid.jpg");
	unsigned int diffuseMapStars = loadTexture("stars.jpg");
	unsigned int diffuseMapShip = loadTexture("falcon.jpg");
	unsigned int diffuseMapFireball = loadTexture("fireball.jpg");


	glEnable(GL_DEPTH_TEST);

	ourShader.use();
	int transLocation = glGetUniformLocation(ourShader.ID, "material.diffuse");
	glUniform1i(transLocation, 0);


	const int numCubes = 25; 
	// set random cube coordinates
	glm::vec3 cubeCoords[numCubes] = {}; 
	for (int i = 0; i < numCubes; i++)
	{
		float x = (-10.0f + (rand() % (-10 - 10 + 1)));
		float y = (-10.0f + (rand() % (-10 - 10 + 1)));
		float z = (-50.0f + (rand() % (-50 + 10 + 1)));
		cout << z << endl; 
		cubeCoords[i] = glm::vec3(x, y, z);
	}
	
	glm::vec3 cubeDirs[numCubes] = {}; 
	int cubeSpeeds[numCubes] = {};
	srand(time(NULL));
	// set random cube directions and speeds
	for (int i = 0; i < numCubes; i++)
	{
		glm::vec3 adjustvec = glm::vec3((-10.0f + (rand() % (-10 - 10 + 1))),
			(-10.0f + (rand() % (-10 - 10 + 1))),
			(-10.0f + (rand() % (-10 - 10 + 1))));
		cubeDirs[i] = glm::normalize(shipCoords - cubeCoords[i] + adjustvec); 
		cout << "x: " << adjustvec.x << "y: " << adjustvec.y << "z: " << adjustvec.z << endl;
		cubeSpeeds[i] = (6.0f + (rand() % (1 + 15 + 1)));
	}
	

	// loop variables
	float boxloc = 0.0f; 
	float tottime = 0.0f; 

	float acceleration = 1.0f; 
	float maxVelocity = 0.075f;
	float HVelocity = 0.0f; 
	float VVelocity = 0.0f; 
	const float xclip = 15.0f; 
	const float yclip = 15.0f; 
	const float maxRoll = 0.15f; 
	const float maxPitch = 0.2f; 
	const float fireballVelocity = 100.0f;

	glfwSetWindowShouldClose(window, false);
	while (!glfwWindowShouldClose(window))
	{
		// update times
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		tottime += deltaTime; 

		processInput(window);

		// if game not over, update asteroid velocities 
		if (!lost)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			{
				VVelocity = VVelocity + acceleration * deltaTime;
				if (VVelocity > maxVelocity)
				{
					VVelocity = maxVelocity;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			{
				VVelocity = VVelocity - acceleration * deltaTime;
				if (VVelocity < -maxVelocity)
				{
					VVelocity = -maxVelocity;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				HVelocity = HVelocity + acceleration * deltaTime;
				if (HVelocity > maxVelocity)
				{
					HVelocity = maxVelocity;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				HVelocity = HVelocity - acceleration * deltaTime;
				if (HVelocity < -maxVelocity)
				{
					HVelocity = -maxVelocity;
				}
			}
		}
		cout << "acceleration: " << acceleration << endl;
		cout << "horizontal velocity: " << HVelocity << endl; 
		cout << "vertical velocity: " << VVelocity << endl;

		// also update the ship's coordinates
		if (!lost) 
		{
			shipCoords.y = shipCoords.y + VVelocity * deltaTime * 100;
			shipCoords.x = shipCoords.x + HVelocity * deltaTime * 100;
		}

		// bounds in space
		if (shipCoords.x >= xclip) shipCoords.x = xclip; 
		if (shipCoords.y >= yclip) shipCoords.y = yclip; 
		if (shipCoords.x <= -xclip) shipCoords.x = -xclip;
		if (shipCoords.y <= -yclip) shipCoords.y = -yclip;

		// maximum camera rotation angles
		camera.upTheta = HVelocity * 0.5f;
		if (camera.upTheta >= maxRoll) camera.upTheta = maxRoll; 
		if (camera.upTheta <= -maxRoll) camera.upTheta = -maxRoll;

		camera.pitchAlpha = VVelocity * 0.05f;
		if (camera.upTheta >= maxPitch) camera.upTheta = maxPitch;
		if (camera.upTheta <= -maxPitch) camera.upTheta = -maxPitch;
		
		// move fireball
		fireballCoords = fireballCoords + glm::normalize(fireballDirection) * (fireballVelocity * deltaTime); 


		camera.ProcessFrame(deltaTime, shipCoords);

		// rendering commands here

		// RGB values, and alpha (transparancy)
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// clear the color and depth from the last frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// shaders
		ourShader.use();
		glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

		transLocation = glGetUniformLocation(ourShader.ID, "viewPos");
		glUniform3fv(transLocation, 1, &camera.Position[0]);


		glm::vec3 ambient = glm::vec3(2.0f, 1.0f, 0.62f);
		glm::vec3 diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
		glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
		glm::vec3 lspecular = glm::vec3(1.0f, 1.0f, 1.0f);
		float shininess = 64.0f;

		transLocation = glGetUniformLocation(ourShader.ID, "material.specular");
		glUniform3fv(transLocation, 1, &specular[0]);
		transLocation = glGetUniformLocation(ourShader.ID, "material.shininess");
		glUniform1f(transLocation, shininess);

		glm::vec3 ldiffuse = lightColor * glm::vec3(1.0f);
		glm::vec3 lambient = ldiffuse * glm::vec3(0.3f);

		transLocation = glGetUniformLocation(ourShader.ID, "light.ambient");
		glUniform3fv(transLocation, 1, &lambient[0]);
		transLocation = glGetUniformLocation(ourShader.ID, "light.diffuse");
		glUniform3fv(transLocation, 1, &ldiffuse[0]);
		transLocation = glGetUniformLocation(ourShader.ID, "light.specular");
		glUniform3fv(transLocation, 1, &lspecular[0]);




		glm::mat4 projMat = glm::mat4(1.0f);
		projMat = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		//ourShader.setMat4("view", view);

		transLocation = glGetUniformLocation(ourShader.ID, "view");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, &view[0][0]);
		transLocation = glGetUniformLocation(ourShader.ID, "projection");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, &projMat[0][0]);


		transLocation = glGetUniformLocation(ourShader.ID, "light.position");
		glUniform3fv(transLocation, 1, &lightPos[0]);

		//cube dir is the direction that the cube is currently moving (and will continue moving until boundary 15)
		//cube change are the change of coordinates since the last frame
		//cube change then gets applied to cube shift, which is the cube's coordinates (or shift from 0)

		for (int i = 0; i < numCubes; i++)
		{
			//ship collision
			glm::vec3 distanceVec = shipCoords - cubeCoords[i];
			float distance = glm::length(distanceVec);
			if (distance <= 1.0f) lost = true;

			//fireball collision
			distanceVec = fireballCoords - cubeCoords[i];
			distance = glm::length(distanceVec);
			if (distance <= 1.0f)
			{
				fireball = false; 

				float x = (-10.0f + (rand() % (-10 - 10 + 1)));
				float y = (-10.0f + (rand() % (-10 - 10 + 1)));
				float z = (-50.0f + (rand() % (-50 + 50 + 1)));
				cubeCoords[i] = glm::vec3(x, y, z);
				glm::vec3 adjustvec = glm::vec3((-10.0f + (rand() % (-10 - 10 + 1))),
					(-10.0f + (rand() % (-10 - 10 + 1))),
					(-10.0f + (rand() % (-10 - 10 + 1))));
				cubeDirs[i] = glm::normalize(shipCoords - cubeCoords[i] + adjustvec);
				cubeSpeeds[i] = (6.0f + (rand() % (1 + 15 + 1)));
			}

			// respawning
			if(cubeCoords[i].z > 5.0)
			{
				float x = (-10.0f + (rand() % (-10 - 10 + 1)));
				float y = (-10.0f + (rand() % (-10 - 10 + 1)));
				float z = (-50.0f + (rand() % (-50 + 50 + 1)));
				cubeCoords[i] = glm::vec3(x, y, z);
				glm::vec3 adjustvec = glm::vec3((-10.0f + (rand() % (-10 - 10 + 1))),
					(-10.0f + (rand() % (-10 - 10 + 1))),
					(-10.0f + (rand() % (-10 - 10 + 1))));
				cubeDirs[i] = glm::normalize(shipCoords - cubeCoords[i] + adjustvec);
				cubeSpeeds[i] = (6.0f + (rand() % (1 + 15 + 1)));
			}

			//moving (asteroids don't move in first five seconds)
			if (!lost && static_cast<float>(glfwGetTime()) >= 5.0)
			{
				glm::vec3 cubeChange = deltaTime * cubeSpeeds[i] * cubeDirs[i];
				cout << cubeChange.z << endl; 
				cubeCoords[i] = cubeCoords[i] + cubeChange;
			}
		}
		
		// asteroid rotations and translations
		for (int i = 0; i < numCubes; i++)
		{
			glm::mat4 modelMat = glm::mat4(1.0f);
			modelMat = glm::translate(modelMat, cubeCoords[i]);
			if (!lost)
			{
				modelMat = glm::rotate(modelMat, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			}
			transLocation = glGetUniformLocation(ourShader.ID, "model");
			glUniformMatrix4fv(transLocation, 1, GL_FALSE, &modelMat[0][0]);

			
			glBindTexture(GL_TEXTURE_2D, diffuseMap);

			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::scale(modelMat, glm::vec3(100.0f));
		transLocation = glGetUniformLocation(ourShader.ID, "model");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, &modelMat[0][0]);


		glBindTexture(GL_TEXTURE_2D, diffuseMapStars);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		//player 
		modelMat = glm::mat4(1.0f); 
		modelMat = glm::translate(modelMat, shipCoords);
		//modelMat = glm::rotate(modelMat, glm::radians(3.14159f), glm::vec3(0.5f, 0.0f, 0.0f));
		transLocation = glGetUniformLocation(ourShader.ID, "model");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, &modelMat[0][0]);
		glBindTexture(GL_TEXTURE_2D, diffuseMapShip);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		

		// fireball
		if (fireball)
		{
			modelMat = glm::mat4(1.0f);
			modelMat = glm::translate(modelMat, fireballCoords);
			modelMat = glm::scale(modelMat, glm::vec3(0.5f));
			modelMat = glm::rotate(modelMat, (float)glfwGetTime() * 20 * glm::radians(50.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			transLocation = glGetUniformLocation(ourShader.ID, "model");
			glUniformMatrix4fv(transLocation, 1, GL_FALSE, &modelMat[0][0]);
			glBindTexture(GL_TEXTURE_2D, diffuseMapFireball);
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		//lamp object
		lightCubeShader.use();
		transLocation = glGetUniformLocation(lightCubeShader.ID, "projection");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, &projMat[0][0]);

		transLocation = glGetUniformLocation(lightCubeShader.ID, "view");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, &view[0][0]);

		modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, lightPos);

		transLocation = glGetUniformLocation(lightCubeShader.ID, "model");
		glUniformMatrix4fv(transLocation, 1, GL_FALSE, &modelMat[0][0]);


		glBindVertexArray(lightCubeVAO); 
		glDrawArrays(GL_TRIANGLES, 0, 36);


		glfwSwapBuffers(window); 
		// check for events and call callback functions
		glfwPollEvents(); 
	}

	//end glfw
	glfwTerminate(); 
	return 0; 
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	//no movement out of the plane
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F && action == GLFW_PRESS) camera.ProcessKeyboard(VIEW, deltaTime, shipCoords);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		fireball = true; 
		fireballCoords = shipCoords; 
		fireballDirection = camera.Front;
	}
}


unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
