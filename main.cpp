#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shaders.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
//#include "camera.h"


using namespace std;

const unsigned int width = 1600; //all initial and global values, most are used for camera control and window definition
const unsigned int height = 1600;
int firstMouse = 1;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;
float lastX = width / 2, lastY = height / 2;
const float radius = 5.0f;
//camera set up
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
glm::vec3 direction;

void mouseInput(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse){
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void loadTexture(Shader& mainshader, GLuint& text) {
	int wid, hig, numcol;
	stbi_set_flip_vertically_on_load(true); //opengl by default will read image in upside down because of how it scans images
	unsigned char* data = stbi_load("bl.png", &wid, &hig, &numcol, 0);
	glGenTextures(1, &text); //stores in unsigned int array
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text); //binds texture to be set

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, hig, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); //scales image size based on distance from camera
	}
	else {
		printf("could not load texture\n");
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	GLuint tex0Uni = glGetUniformLocation(mainshader.ID, "tex0");
	mainshader.Activate();

	glUniform1i(tex0Uni, 0);
}

int main() {
	//initialize glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //specify what version is in use
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //specify what profile is in use
	
	const unsigned int width = 1600;
	const unsigned int height = 1600;
	//gl's float type
	/*GLfloat vertices[] =
	{
	   -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, 0.5f, 0.2f, 0.7f,//vertex coordinates lower left along with colors
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, 0.3f, 0.5f, 0.2f,
		0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f, 0.5f, 0.3f, 0.8f,
	   -0.25,  0.5f * float(sqrt(3)) / 6, 0.0f, 0.2f, 0.4f, 0.2f,//inner left
		0.25,  0.5f * float(sqrt(3)) / 6, 0.0f, 0.6f, 0.3f, 0.7f,
		0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f, 0.7f, 0.2f, 0.4f//bottom middle
	};

	GLuint indices[] = {
		0, 3, 5,
		3, 2, 4,
		5, 4, 1
	};*/

	GLfloat vertices[] =
	{
	   -0.5f,  0.0f, 0.5f,    0.83f, 0.70f, 0.44f,  0.0f, 0.0f,
	   -0.5f,  0.0f, -0.5f,    0.83f, 0.70f, 0.44f,  5.0f, 0.0f,
		0.5f,  0.0f, -0.5f,    0.83f, 0.70f, 0.44f,  1.0f, 0.0f,
	    0.5f,  0.0f, 0.5f,    0.83f, 0.70f, 0.44f, 5.0f, 0.0f,
		0.0f, 0.8f, 0.0f,      0.92f, 0.86f, 0.76f, 2.5f, 5.0f
	};

	GLuint indices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 1, 4,
		1, 2, 4,
		2, 3, 4,
		3, 0, 4
	};

	GLFWwindow* window = glfwCreateWindow(width, height, "Capstone", NULL, NULL); //set dimensions and name for window

	if (window == NULL)
		cout << "error \n";

	glfwMakeContextCurrent(window); //introduce window into context (current resources to utilize)
	gladLoadGL(); //loading glad configures opengl

	glViewport(0, 0, width, height); //this is opengl's domain in the window, for the full window just match the dimensions
	//void framebuffer_size_callback(GLFWwindow* window, int width, int height);   callback to dynamically change as user modifies window size
	//gl's unsigned intiger and creating shader object before getting reference

	glfwSetCursorPosCallback(window, mouseInput);

	Shader mainshader("default.vert", "default.frag");

	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
	glm::mat4 view;
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),       //look at vector multiplies against camera vector to change viewing position
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	//create reference containers for vertex array and buffer objects
	VAO VAOini;
	VAOini.Bind();
	VBO VBOini(vertices, sizeof(vertices));
	EBO EBOini(indices, sizeof(indices));
	VAOini.Link_atrib(VBOini, 0, 3, GL_FLOAT, 8*sizeof(float), (void*)0);
	VAOini.Link_atrib(VBOini, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3*sizeof(float)));
	VAOini.Link_atrib(VBOini, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	VAOini.Unbind();
	VBOini.Unbind();
	EBOini.Unbind();

	GLuint uniID = glGetUniformLocation(mainshader.ID, "scale");


	//PROCESS KEYBOARD PICTURE YAYYYYY   YYYYYYAYYY I LIKE KEYBOARRDS
	//------------------------------------------------------------------------------------------------image processing
	GLuint text;
	loadTexture(mainshader, text);


	glEnable(GL_DEPTH_TEST); // sets depth rendering

	//camera maincamera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	while (!glfwWindowShouldClose(window)) {

		processInput(window);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); //navy in back buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //swaps buffer to change frame to pull back buffer. current frame gets sent to back buffer?
		mainshader.Activate();

		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;


		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		proj = glm::perspective(glm::radians(45.0f), (float)(width / height), 0.1f, 100.0f); ///sets field of view, window size, and near and far plane which control render distance and clipping

		int mlocation = glGetUniformLocation(mainshader.ID, "model");
		glUniformMatrix4fv(mlocation, 1, GL_FALSE, glm::value_ptr(model));
		int vlocation = glGetUniformLocation(mainshader.ID, "view");
		glUniformMatrix4fv(vlocation, 1, GL_FALSE, glm::value_ptr(view));
		int plocation = glGetUniformLocation(mainshader.ID, "proj");
		glUniformMatrix4fv(plocation, 1, GL_FALSE, glm::value_ptr(proj));
		
		glUniform1f(GL_TEXTURE_2D, text);

		glBindTexture(GL_TEXTURE_2D, text);
		VAOini.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	VAOini.Delete();
	VBOini.Delete();
	EBOini.Delete();
	glDeleteTextures(1, &text);
	mainshader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

