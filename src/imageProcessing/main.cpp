#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const int WIDTH = 800;
const int HEIGHT = 400;

typedef unsigned int u32;

#define ASSERT(test) if (!(test)) { *(int*)0 = 0; }

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

	Pipeline(const char* vertexSource, const char* fragmentSource) {

		u32 id;
		int vertexShader = 0;
		if (!createShader(vertexSource, GL_VERTEX_SHADER, vertexShader)) {
			printf("Failed to create and compile vertex shader\n");
			return;
		}

		int fragmentShader = 0;
		if (!createShader(fragmentSource, GL_FRAGMENT_SHADER, fragmentShader)) {
			printf("Failed to create and compile fragment shader\n");
			return;
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
			return;
		}

		this->id = id;
	}

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
		u32 error = glGetError();
		glUniform1f(location, value);
		u32 error1 = glGetError();
	}
};

struct Texture {
	u32 id;

	Texture(const char* file, u32 format) {
		glGenTextures(1, &this->id);
		glBindTexture(GL_TEXTURE_2D, this->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		int width, height, channels;
		char imagePath[255];
		sprintf_s(imagePath, "%s%s", DATA_DIR, file);

		u32 desiredChannel = 3;
		if (format == GL_RGBA) {
			desiredChannel = 4;
		}

		unsigned char* data = stbi_load(imagePath, &width, &height, &channels, desiredChannel);

		ASSERT(data);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
};

int singleTexture(GLFWwindow* window) {

	glClearColor(0.7f, 0.3f, 0.7f, 1.0f);

	float vertices[] = {
		// positions			// colors           // texture coords
		0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,		1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	u32 indices[] = {
		0, 1, 3,
		1, 2, 3
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

	u32 ebo;
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// vertex shader
	const char* vertexShaderSource = R"(
		#version 330 core
		layout (location = 0) in vec3 pos;
		layout (location = 1) in vec4 color;
		layout (location = 2) in vec2 texCoords;

		out vec4 vColor;
		out vec2 vCoord;

		void main() 
		{
			gl_Position = vec4(pos.xyz, 1.0);
			vColor = color;
			vCoord = texCoords;
		}	
	)";

	// fragment shader
	const char* fragmentShaderSource = R"(
		#version 330 core

		in vec4 vColor;
		in vec2 vCoord;
		uniform sampler2D uTexture;
		out vec4 fColor;

		void main() 
		{
			fColor = texture(uTexture, vCoord);
		}
	)";

	Pipeline pipeline = Pipeline(vertexShaderSource, fragmentShaderSource);

	if (!pipeline.id) {
		return -5;
	}

	// setup vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// setup texture state
	u32 texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, channels;
	char imageName[] = "/wall.jpg";
	char imagePath[sizeof(DATA_DIR) + sizeof(imageName)];
	sprintf_s(imagePath, "%s%s", DATA_DIR, imageName);
	unsigned char* data = stbi_load(imagePath, &width, &height, &channels, 0);

	ASSERT(data);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	// main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		pipeline.use();
		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		float time = (float)glfwGetTime();
		float color = (float)sin(time) / 2.0f + 0.5f;

		//draw
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

int main(int argc, char* argv[])
{
	int result = 0;
	stbi_set_flip_vertically_on_load(true);

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
		result = singleTexture(window);
	}
	else if (false) {
		result = 0;
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
