#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <stdio.h>
#include <math.h>

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
		printf("failed to compile vertex shader: \n%s", infoLog);
		glDeleteShader(outShader);
		return false;
	}

	return true;
}

struct Pipeline {
	u32 id;

	void use() {
		glUseProgram(this->id);
	}

	void setUniform(const char* name, int value) {
		u32 location = glGetUniformLocation(this->id, name);
		glUniform1i(location, value);
	}

	void setUniform(const char* name, bool value) {
		this->setUniform(name, (int)value);
	}

	void setUniform(const char* name, float value) {
		u32 location = glGetUniformLocation(this->id, name);
		glUniform1f(this->id, value);
	}
};

Pipeline initPipeline(const char* vertexSource, const char* fragmentSource) {
	Pipeline pipeline = {};

	u32 id;
	int vertexShader = 0;
	if (!createShader(vertexSource, GL_VERTEX_SHADER, vertexShader)) {
		printf("Failed to create and compile vertex shader\n");
		return pipeline;
	}

	int fragmentShader = 0;
	if (!createShader(fragmentSource, GL_FRAGMENT_SHADER, fragmentShader)) {
		printf("Failed to create and compile fragment shader\n");
		return pipeline;
	}

	// pipeline
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	int success = 0;
	char infoLog[512];
	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(id, 512, nullptr, infoLog);
		printf("failed to compile vertex shader: \n%s", infoLog);
		glDeleteProgram(id);
		return pipeline;
	}

	pipeline.id = id;
	return pipeline;
}

int helloShaders(GLFWwindow* window) {
	int result = 0;

	glClearColor(0.7f, 0.3f, 0.7f, 1.0f);

	float vertices[] = {
		// positions         // colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
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
		layout (location = 1) in vec4 color;

		out vec4 vColor;

		void main() {
			gl_Position = vec4(pos.xyz, 1.0);
			vColor = color;
		}	
	)";

	// fragment shader
	const char* fragmentShaderSource = R"(
		#version 330 core

		in vec4 vColor;
		out vec4 fColor;

		void main() {
			fColor = vColor;
		}
	)";

	Pipeline pipeline = initPipeline(vertexShaderSource, fragmentShaderSource);

	// setup vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		
		pipeline.use();
		glBindVertexArray(vao);

		float time = (float)glfwGetTime();
		float color = (float)sin(time) / 2.0f + 0.5f;

		int uColorLocation = glGetUniformLocation(pipeline.id, "uColor");
		glUniform4f(uColorLocation, 0.0f, color, 0.0f, 1.0f);

		//draw
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

done:
	return result;
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


	// turn on the different render outputs by changing the falses to 
	// true. very primitive but quick to prototype.
	if (true) {
		result = helloShaders(window);
	}
	else {
		result = 0;
	}

	// exit
gladLoadGLFail:
	glfwDestroyWindow(window);
glfwCreateWindowFail:
	glfwTerminate();
glfwInitFail:

	return result;
}
