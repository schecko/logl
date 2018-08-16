#include <glfw/deps/glad/glad.h>
#include <glfw/glfw3.h>

#include <stdio.h>

const int WIDTH = 800;
const int HEIGHT = 400;

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
		goto glfwInitFail;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(	WIDTH, 
											HEIGHT,
											"LOGL",
											nullptr, 
											nullptr);
	if (!window) {
		printf("Failed to create GLFW window\n");
		goto glfwCreateWindowFail;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGL()) {
		printf("Failed to initialize glad\n");
		goto gladLoadGLFail;
	}


   glClearColor(0.7f, 0.3f, 0.7f, 1.0f);

	// main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// exit
gladLoadGLFail:
	glfwDestroyWindow(window);
glfwCreateWindowFail:
	glfwTerminate();
glfwInitFail:
	return result;
}
