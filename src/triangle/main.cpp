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

int oneTri(GLFWwindow* window) {
	int result = 0;

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

done:
	return result;
}

int oneQuad(GLFWwindow* window) {
	int result = 0;

	glClearColor(0.7f, 0.3f, 0.7f, 1.0f);

	float vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	u32 indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	//vertex array object
	u32 vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	u32 ebo;
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

done:
	return result;
}

int twoTri(GLFWwindow* window) {
	int result = 0;

	glClearColor(0.7f, 0.3f, 0.7f, 1.0f);

	float vertices[] = {
		-1.0f, -0.5f, 0.0f,
		0.0f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,

		0.0f, -0.5f, 0.0f,
		1.0f, -0.5f, 0.0f,
		0.5f,  0.5f, 0.0f
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
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

done:
	return result;
}


int twoVAO(GLFWwindow* window) {
	int result = 0;

	glClearColor(0.7f, 0.3f, 0.7f, 1.0f);

	float vertices[2][9] = {

		{	-1.0f, -0.5f, 0.0f,
			0.0f, -0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		},
		{	0.0f, -0.5f, 0.0f,
			1.0f, -0.5f, 0.0f,
			0.5f,  0.5f, 0.0f
		}
	};

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

	//vertex array object
	u32 vao[2];
	glGenVertexArrays(2, vao);

	//vertex buffer object
	u32 vbo[2];
	glGenBuffers(2, vbo);

	for (u32 i = 0; i < 2; i++) {

		glBindVertexArray(vao[i]);

		// vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]), vertices[i], GL_STATIC_DRAW);

		// setup vertex attributes
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	// main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(pipeline);

		for (int i = 0; i < 2; i++) {
			glBindVertexArray(vao[i]);

			//draw
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

done:
	return result;
}

int twoFrag(GLFWwindow* window) {
	int result = 0;

	glClearColor(0.7f, 0.3f, 0.7f, 1.0f);

	float vertices[] = {	
		-1.0f, -0.5f, 0.0f,
		0.0f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,

		0.0f, -0.5f, 0.0f,
		1.0f, -0.5f, 0.0f,
		0.5f,  0.5f, 0.0f
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
	const char fragmentShaderSourceRaw[] = R"(
		#version 330 core
		out vec4 color;

		void main() {
			color = vec4(%f, %f, %f, 1.0f);
		}
	)";

	char fragmentShaderSource1[sizeof(fragmentShaderSourceRaw) * 2];
	char fragmentShaderSource2[sizeof(fragmentShaderSourceRaw) * 2];
	sprintf(fragmentShaderSource1, fragmentShaderSourceRaw, 0.2f, 0.3f, 0.5f);
	sprintf(fragmentShaderSource2, fragmentShaderSourceRaw, 0.5f, 0.3f, 0.f);

	int fragmentShader[2];
	if (!createShader(fragmentShaderSource1, GL_FRAGMENT_SHADER, fragmentShader[0])) {
		printf("Failed to create and compile fragment shader\n");
		result = -5;
		goto done;
	}

	if (!createShader(fragmentShaderSource2, GL_FRAGMENT_SHADER, fragmentShader[1])) {
		printf("Failed to create and compile fragment shader\n");
		result = -5;
		goto done;
	}

	// pipeline
	u32 pipeline[2];
	for (int i = 0; i < 2; i++) {
		pipeline[i] = glCreateProgram();
		glAttachShader(pipeline[i], vertexShader);
		glAttachShader(pipeline[i], fragmentShader[i]);
		glLinkProgram(pipeline[i]);
		glDeleteShader(fragmentShader[i]);

		int success = 0;
		char infoLog[512];
		glGetProgramiv(pipeline[i], GL_LINK_STATUS, &success);

		if (!success) {
			glGetProgramInfoLog(pipeline[i], 512, nullptr, infoLog);
			printf("failed to compile vertex shader: \n%s", infoLog);
			return false;
		}
	}
	glDeleteShader(vertexShader);


	// setup vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vao);

		glUseProgram(pipeline[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUseProgram(pipeline[1]);
		glDrawArrays(GL_TRIANGLES, 3, 3);

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
	if (false) {
		result = oneTri(window);
	}
	else if (false) {
		result = oneQuad(window);
	}
	else if (false) {
		result = twoTri(window);
	}
	else if (false) {
		result = twoVAO(window);
	}
	else {
		result = twoFrag(window);
	}

	// exit
gladLoadGLFail:
	glfwDestroyWindow(window);
glfwCreateWindowFail:
	glfwTerminate();
glfwInitFail:

	return result;
}
