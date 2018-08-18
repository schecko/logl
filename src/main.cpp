#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <stdio.h>

const int WIDTH = 800;
const int HEIGHT = 400;

typedef unsigned int u32;

bool createShader(const char* shaderSource, GLuint shaderType, int& outShader) {

	outShader = glCreateShader(shaderType);
	glShaderSource(outShader, 1, &shaderSource, nullptr);
	glCompileShader(outShader);

	int success = 0;
	char infoLog[512];
	glGetShaderiv(outShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(outShader, 512, nullptr, infoLog);
		glDeleteShader(outShader);
		printf("failed to compile vertex shader: \n%s", infoLog);
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	int result = 0;

	glfwSetErrorCallback(
		[](int error, const char* description) {
		printf("GLFW error %i: %s\n", error, description);
	}
	);

	if (!glfwInit()) {
		printf("Failed to initialize GLFW\n");
		result = -1;
		goto glfwInitFail;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WIDTH,
											HEIGHT,
											"LOGL",
											nullptr,
											nullptr);
	if (!window) {
		printf("Failed to create GLFW window\n");
		result = -2;
		goto glfwCreateWindowFail;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize glad\n");
		result = -3;
		goto gladLoadGLFail;
	}

	glClearColor(0.7f, 0.3f, 0.7f, 1.0f);


	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	//vertex array object
	u32 vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex buffer
	u32 vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex shader
	const char* vertexShaderSource = R"(
		#version 330 core
		layout (location = 0) in vec3 pos;

		void main() {
			gl_Position = vec4(pos.xyz, 1.0);
		}	
	)";

	int vertexShader = 0;
	if (!createShader(vertexShaderSource, GL_VERTEX_SHADER, vertexShader)) {
		printf("Failed to create and compile vertex shader\n");
		result = -4;
		goto done;
	}

	// fragment shader
	const char* fragmentShaderSource = R"(
		#version 330 core
		out vec4 color;

		void main() {
			color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
		}
	)";

	int fragmentShader = 0;
	if (!createShader(fragmentShaderSource, GL_FRAGMENT_SHADER, fragmentShader)) {
		printf("Failed to create and compile fragment shader\n");
		result = -5;
		goto done;
	}

	// pipeline
	u32 pipeline = glCreateProgram();
	glAttachShader(pipeline, vertexShader);
	glAttachShader(pipeline, fragmentShader);
	glLinkProgram(pipeline);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	int success = 0;
	char infoLog[512];
	glGetProgramiv(pipeline, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(pipeline, 512, nullptr, infoLog);
		printf("failed to compile vertex shader: \n%s", infoLog);
		return false;
	}

	// setup vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(pipeline);
		glBindVertexArray(vao);

		//draw
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// exit
done:
gladLoadGLFail:
	glfwDestroyWindow(window);
glfwCreateWindowFail:
	glfwTerminate();
glfwInitFail:
	return result;
}
