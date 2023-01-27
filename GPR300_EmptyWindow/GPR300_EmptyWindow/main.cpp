#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <stdio.h>

struct Vec3
{
	float x, y, z;
};

struct Color
{
	float r, g, b, a;
};

struct Vertex
{
	Vec3 position;
	Color color;
};

Vertex vertices[] =
{
	// triangle 1
	Vertex{Vec3{-0.5, -0.5, 0.0}, Color{1.0, 0.0, 0.0, 1.0}},
	Vertex{Vec3{+0.0, -0.5, 0.0}, Color{0.0, 1.0, 0.0, 1.0}},
	Vertex{Vec3{-0.25, 0.0, 0.0}, Color{0.0, 0.0, 1.0, 1.0}},

	// triangle 2
	Vertex{Vec3{0.0, -0.25, 0.0}, Color{0.0, 1.0, 0.0, 1.0}},
	Vertex{Vec3{0.5, -0.25, 0.0}, Color{0.0, 0.0, 1.0, 1.0}},
	Vertex{Vec3{0.25, 0.25, 0.0}, Color{1.0, 0.0, 0.0, 1.0}},
};

//void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);

// Vertex shader source code
const char* vertexShaderSource =
"#version 450							\n"
"layout (location = 0) in vec3 vPos;	\n"
"layout (location = 1) in vec4 vCol;	\n"
"out vec4 Color;						\n"
"uniform float _Time;					\n"
"void main() {							\n"
"	Color = vCol;						\n"
"	gl_Position = vec4(vPos.x * abs(sin(_Time * 0.25)), vPos.y * abs(cos(_Time * 0.25)), vPos.z, 1.0);		\n"
"}										\0";

// Fragment shader source code
const char* fragmentShaderSource =
"#version 450							\n"
"out vec4 FragColor;					\n"
"in vec4 Color;							\n"
"uniform float _Time;					\n"
"void main() {							\n"
"	FragColor = abs(sin(_Time))*Color;	\n"
"}										\0";

int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGLExample", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);


	// Create and compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Get vertex shader compilation status and output info log
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		// Dump logs into a char array - 512 is an arbitrary length
		GLchar infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Failed to compile vertex shader %s", infoLog);
	}

	// Create and compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Get fragment shader compilation status and output info log
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		// Dump logs into a char array - 512 is an arbitrary length
		GLchar infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("Failed to compile vertex shader %s", infoLog);
	}

	// Create shader program
	GLuint shaderProgram = glCreateProgram();

	// Attach vertex and fragment shaders to shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Link shader program
	glLinkProgram(shaderProgram);

	// Check for link status and output errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}

	// Delete vertex + fragment shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//TODO: Create and bind Vertex Array Object (VAO)
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Create and bind Vertex Buffer Object (VBO), fill with vertexData
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// POSITION
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);

	// COLOR
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
	glEnableVertexAttribArray(1);

	int timeLocation = glGetUniformLocation(shaderProgram, "_Time");

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use shader program
		glUseProgram(shaderProgram);

		// Set uniforms
		float time = (float)glfwGetTime();
		glUniform1f(timeLocation, time);

		// Draw triangle (3 vertices!)
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

