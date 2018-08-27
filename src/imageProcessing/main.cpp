#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const int WIDTH = 800;
const int HEIGHT = 400;

typedef unsigned int u32;

#define PI 3.14159265359f
#define ASSERT(test) if (!(test)) { *(int*)0 = 0; }

bool createShader(const char* shaderSource, GLuint shaderType, int& outShader) 
{

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

	Pipeline(const char* vertexSource, const char* fragmentSource) 
	{

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

	void use() 
	{
		glUseProgram(this->id);
	}

	void setUniform(const char* name, int value) 
	{
		u32 location = glGetUniformLocation(this->id, name);
		glUniform1i(location, value);
	}

	void setUniform(const char* name, bool value) 
	{
		this->setUniform(name, (int)value);
	}

	void setUniform(const char* name, float value) 
	{
		u32 location = glGetUniformLocation(this->id, name);
		u32 error = glGetError();
		glUniform1f(location, value);
		u32 error1 = glGetError();
	}
};

struct Image 
{
	int width, height, channels;
	bool rgb;
	float* data;
};

Image loadImage(const char* file, int desiredChannels) 
{
	Image image = {};
	image.channels = desiredChannels;
	image.rgb = true;

	char imagePath[255];
	sprintf_s(imagePath, "%s%s", DATA_DIR, file);

	int unused;
	image.data = stbi_loadf(imagePath, &image.width, &image.height, &unused, desiredChannels);
	ASSERT(image.data);

	return image;
}

Image toHSI(Image sourceImage)
{
	ASSERT(sourceImage.rgb);
	Image destImage = sourceImage;
	destImage.data = (float*)malloc(sizeof(float) * destImage.height * destImage.width * sourceImage.channels);
	destImage.rgb = false;

	for (int y = 0; y < sourceImage.height; y++) {
		for (int x = 0; x < sourceImage.width; x++) {
			float* dest = &destImage.data[x + y * sourceImage.channels];
			float* source = &sourceImage.data[x + y * sourceImage.channels];

			if (sourceImage.channels == 4) {
				// flat copy of the alpha
				dest[3] = source[3];
			}

			float min = source[0];
			// ignore alpha, dont compare r with r.
			for (int i = 1; i < 3; i++) {
				if (min > source[i]) {
					min = source[i];
				}
			}
			float r = source[0];
			float g = source[1];
			float b = source[2];

			float rgbSum = (r + g + b);
			float intensity = rgbSum / 3.0f;

			float saturation = 1.0f - (3.0f / rgbSum) * min;
			double angle = (r - 0.5f * g - 0.5f * b) / sqrt((r - g) * (r - g) + (r - b) * (g - b));
			float hue = (float)acos(angle);
			
			if (hue > 2.0f * PI) {
				hue = 2.0f * PI - hue;
			}
			
			dest[0] = hue;
			dest[1] = saturation;
			dest[2] = intensity;
		}
	}

	return destImage;
}

Image toRGB(Image sourceImage) {
	ASSERT(!sourceImage.rgb);
	Image destImage = sourceImage;
	destImage.data = (float*)malloc(sizeof(float) * destImage.height * destImage.width * sourceImage.channels);
	destImage.rgb = true;

	for (int y = 0; y < sourceImage.height; y++) {
		for (int x = 0; x < sourceImage.width; x++) {
			float* dest = &destImage.data[x * y * sourceImage.channels];
			float* source = &sourceImage.data[x * y * sourceImage.channels];

			if (sourceImage.channels == 4) {
				// flat copy of the alpha
				dest[3] = source[3];
			}

			float hue = source[0];
			float saturation = source[1];
			float intensity = source[2];

			float r, g, b;

			float epsilon = 0.05f;
			if (intensity <= epsilon) {
				//black
				r = 0.0f;
				g = 0.0f;
				b = 0.0f;
			}
			else if (saturation <= epsilon) {
				// grey scale
				r = intensity;
				g = intensity;
				b = intensity;
			}
			else {
				if (hue < 0.0f) {
					hue += 2.0f * PI;
				}

				float scale = 3.0f * intensity;
							// 120deg
				if (hue <= PI * 2.0f / 3.0f) {
					float angle1 = hue;
									// 60deg
					float angle2 = (PI / 3.0f - hue);
					b = (1.0f - saturation) / 3.0f * scale;
					r = (1.0f + (saturation * cos(angle1) / cos(angle2))) / 3.0f * scale;
					g = (1.0f - r - b) * scale;
				}				// 120deg					// 240deg
				else if (hue > PI * 2.0f / 3.0f && hue <= PI * 4.0f / 3.0f) {
					hue -= PI * 2.0f / 3.0f;
					float angle1 = hue;
					float angle2 = (PI / 3.0f - hue);

					r = (1.0f - saturation) / 3.0f * scale;
					g = (1.0f + (saturation * cos(angle1) / cos(angle2))) / 3.0f * scale;
					b = (1.0f - r - g) * scale;
				}
				else {
					hue -= PI * 4.0f / 3.0f;
					float angle1 = hue;
					float angle2 = (PI / 3.0f - hue);

					g = (1.0f - saturation) / 3.0f * scale;
					b = (1.0f + (saturation * cos(angle1) / cos(angle2))) / 3.0f * scale;
					r = (1.0f - g - b) * scale;
				}
			}
			
			dest[0] = r;
			dest[1] = g;
			dest[2] = b;
		}
	}

	return destImage;
}

struct Texture 
{
	u32 id;
	int width, height;

	Texture(Image image) 
	{
		this->height = image.height;
		this->width = image.width;

		glGenTextures(1, &this->id);
		glBindTexture(GL_TEXTURE_2D, this->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		u32 format = GL_RGBA;
		if (image.channels == 3) {
			format = GL_RGB;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, format, GL_FLOAT, image.data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Texture(const char* file, u32 channels) 
	{
		Image image = loadImage(file, channels);
		*this = Texture(image);
		stbi_image_free(image.data);
	}


};

int singleTexture(GLFWwindow* window) 
{

	glClearColor(0.7f, 0.3f, 0.7f, 1.0f);

	float offset = 0.5f;
	float vertices[] = {
		// left texture
		// positions					// texture coords
		0.5f - offset,  1.0f, 0.0f,		1.0f, 1.0f,   // top right
		0.5f - offset, -1.0f, 0.0f,		1.0f, 0.0f,   // bottom right
		-0.5f - offset, -1.0f, 0.0f,	0.0f, 0.0f,   // bottom left
		-0.5f - offset,  1.0f, 0.0f,	0.0f, 1.0f,    // top left 

		// right texture
		0.5f + offset,  1.0f, 0.0f,		1.0f, 1.0f,   // top right
		0.5f + offset, -1.0f, 0.0f,		1.0f, 0.0f,   // bottom right
		-0.5f + offset, -1.0f, 0.0f,	0.0f, 0.0f,   // bottom left
		-0.5f + offset,  1.0f, 0.0f,	0.0f, 1.0f    // top left 
	};

	u32 indices[] = {
		0, 1, 3,
		1, 2, 3,

		4, 5, 7,
		5, 6, 7
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
		layout (location = 1) in vec2 texCoords;

		out vec2 vCoord;

		void main() 
		{
			gl_Position = vec4(pos.xyz, 1.0);
			vCoord = texCoords;
		}	
	)";

	// fragment shader
	const char* normalFragmentShaderSource = R"(
		#version 330 core

		in vec2 vCoord;
		uniform sampler2D uTexture;
		uniform int uWidth;
		uniform int uHeight;
		out vec4 fColor;

		void main() 
		{
			fColor = texelFetch(uTexture, ivec2(vCoord.x * uWidth, vCoord.y * uHeight), 0);
		}
	)";

	// fragment shader
	const char* imageProcessingFragmentSource = R"(
		#version 330 core

		in vec2 vCoord;
		uniform sampler2D uTexture;
		uniform int uWidth;
		uniform int uHeight;
		out vec4 fColor;

		void main() 
		{
			fColor = 1.0 - texelFetch(uTexture, ivec2(vCoord.x * uWidth, vCoord.y * uHeight), 0);
		}
	)";

	Pipeline normalPipeline = Pipeline(vertexShaderSource, normalFragmentShaderSource);
	Pipeline processingPipeline = Pipeline(vertexShaderSource, imageProcessingFragmentSource);

	if (!normalPipeline.id) {
		return -5;
	}

	if (!processingPipeline.id) {
		return -5;
	}

	// setup vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Image image = loadImage("/color-face.jpg", 4);
	Image hsiImage = toHSI(image);
	Image rgbImage = toRGB(hsiImage); // should be same as original

	Texture texture = Texture(image);
	Texture hsiTexture = Texture(hsiImage);
	Texture rgbTexture = Texture(rgbImage);
	
	// main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		normalPipeline.use();

		normalPipeline.setUniform("uWidth", texture.width);
		normalPipeline.setUniform("uHeight", texture.height);
		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.id);

		//draw
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//processingPipeline.use();
		//processingPipeline.setUniform("uWidth", texture.width);
		//processingPipeline.setUniform("uHeight", texture.height);
		glBindTexture(GL_TEXTURE_2D, rgbTexture.id);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(int)));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	stbi_image_free(image.data);
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
