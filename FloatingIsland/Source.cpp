
#include <string>
// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
// SOIL2
#include "SOIL2/SOIL2.h"

// Including headers 
#include "Shader.h"
#include "Camera.h"
#include "Model.h"



//  Window Properties
const GLuint WIDTH = 800, HEIGHT = 800;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

					// Camera
Camera camera(glm::vec3(0.0f, 30.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

float movecameraX = 0.0f; // Float value to move camera on the X axis
float movecameraY = 0.0f; // Float value to move camera on the Y axis

float cometFloat = 0.0f; // ADDED Float value to increase and decrease commet size.
float cometMoveX = 0.0f; // ADDED Float value to move the comet around the X Axis.
float cometMoveZ = 0.0f; // ADDED Float value to move the comet around the Z Axis.


int instances = 15; // Int for instancing trees
int instanceComet = 2; // Int for instancing comet
// used for tree and comet instancing 
glm::mat4 * instancing = new glm::mat4[instances]; // istance Tree
glm::mat4 * instancingComet = new glm::mat4[instanceComet]; // Instance  comet


glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // position of directional light.

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "LowPoly Floating Island", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required key callback functions
	glfwSetKeyCallback(window, KeyCallback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(1, 1, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Shaders
	Shader Combinedshader("res/shaders/sceneLighting.vs", "res/shaders/sceneLighting.frag"); // used directional lighting 
	Shader Cometshader("res/shaders/cometLighting.vs", "res/shaders/cometLighting.frag"); 

	// Loading models
	Model Island("res/models/lowpoly.obj");
	Model Palm("res/models/PalmTree.obj");
	Model Comet("res/models/Comet.obj");
	glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	
	
	
	
	
	
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();

		// Clear the colorbuffer
		glClearColor(0.5f, 0.7f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Lighting=============================================================

		GLint lightDirLoc = glGetUniformLocation(Combinedshader.Program, "light.direction"); // calls the light.direction from the combined shader. 

		// Camera ===================================================================================
		//  the view matrix
		glm::mat4 view = camera.GetViewMatrix(); 

	
		//===========================================================================================================


	

		// Island Model ----------------------------------------------------------------------------------------------
		Combinedshader.Use();

		glUniformMatrix4fv(glGetUniformLocation(Combinedshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(Combinedshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform3f(lightDirLoc, 30.0f, 20.0f, 10.0f); //CHANGE direction of Light 
		glUniform3f(glGetUniformLocation(Combinedshader.Program, "light.ambient"), 0.3f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(Combinedshader.Program, "light.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(Combinedshader.Program, "light.specular"), 0.5f, 0.5f, 0.5f);

		glm::mat4 Islandmodel;
		Islandmodel = glm::translate(Islandmodel, glm::vec3(0.0f, 1.75f, 0.0f)); // Traslation of the Island Model 
		Islandmodel = glm::scale(Islandmodel, glm::vec3(5.0f, 5.0f, 5.0f));	// Scaleing the Island Model.
		glUniformMatrix4fv(glGetUniformLocation(Combinedshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(Islandmodel));

		Island.Draw(Combinedshader);

		//Camera Rotation around island -------------------------------------------------------------------------
		// creating a vector called cameraVec and set it to rotate the Y axis with the coordinates of camera position with respect to currentframe * 0.2. 
		glm::vec3 cameraVec = glm::rotateY(glm::vec3(30.0f + movecameraX, 20.0f + movecameraY, 30.0f), 0.2f * currentFrame);
		camera.front = glm::normalize((glm::vec3)Islandmodel[3] - camera.position); // sets the camera, using normalize, to always face the island.
		camera.position = (glm::vec3)Islandmodel[3] + cameraVec; // updating the camera position by adding the island coordinates + the camera vector
		//----------------------------------------------------------------------------
//==============================================================================================================================================

		// Palm Model -------------------------------------------------------------------------------------------------------
		Combinedshader.Use();
		glUniformMatrix4fv(glGetUniformLocation(Combinedshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(Combinedshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));


		for (int i = 0; i <= instances; i++)
		{
			glm::mat4 Palmmodel = instancing[i];
			Palmmodel = glm::translate(Palmmodel, glm::vec3(-5.0f + cos(i * 19), 12.3f, -7.0f + sin(i + 2))); // Use TAN Translating Palm trees using sin and cos with "i" to randomize.
			Palmmodel = glm::scale(Palmmodel, glm::vec3(0.002f, 0.002f, 0.002f));	// model scale
			glUniformMatrix4fv(glGetUniformLocation(Combinedshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(Palmmodel));
			Palm.Draw(Combinedshader);
		}

		//Comet Model ---------------------------------------------

		Cometshader.Use();
		// ADDED
		glUniformMatrix4fv(glGetUniformLocation(Cometshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(Cometshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform3f(glGetUniformLocation(Cometshader.Program, "light.ambient"), 0.5f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(Cometshader.Program, "light.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(Cometshader.Program, "light.specular"), 0.0f, 0.5f, 0.5f);

		glm::mat4 cometmodel;
		
		// instancing the comet
		for (int i = 0; i <= instanceComet; i++)
		{
			glm::mat4 cometmodel = instancingComet[i];
			cometmodel = glm::translate(cometmodel, glm::vec3(5.0f + i*5 + cometMoveX, 20.0f, 1.0f + i*cos(10) + cometMoveZ)); // Translation of commet + a float value to move the comet..
			cometmodel = glm::scale(cometmodel, glm::vec3(1.0f + cometFloat, 1.0f + cometFloat, 1.0f + cometFloat));	// Scaling the comet + float value to make the comet bigger or smaller.
			cometmodel = glm::rotate(cometmodel, glm::radians(90.0f)*currentFrame, glm::vec3(0.0f, 1.0f, .0f));
			glUniformMatrix4fv(glGetUniformLocation(Cometshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(cometmodel));
			Comet.Draw(Cometshader);
		}



		// Swap the buffers
		glfwSwapBuffers(window);


	}

}
// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (keys[GLFW_KEY_R])
	{
		cometFloat += 0.1f;
	}
	if (keys[GLFW_KEY_E])
	{
		cometFloat -= 0.1f;
	}
	if (keys[GLFW_KEY_W])
	{
		cometMoveZ += 0.1f;
	}
	if (keys[GLFW_KEY_A])
	{
		cometMoveX -= 0.1f;
	}
	if (keys[GLFW_KEY_S])
	{
		cometMoveZ -= 0.1f;
	}
	if (keys[GLFW_KEY_D])
	{
		cometMoveX += 0.1f;
	}
	if (keys[GLFW_KEY_RIGHT])
	{
		movecameraX += 0.1f;
	}
	if (keys[GLFW_KEY_LEFT])
	{
		movecameraX -= 0.1f;
	}
	if (keys[GLFW_KEY_UP])
	{
		movecameraY += 0.1f;
	}
	if (keys[GLFW_KEY_DOWN])
	{
		movecameraY -= 0.1f;
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}



}

