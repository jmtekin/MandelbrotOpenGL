#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader_class.h>

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader_class.h>

#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void countFPS();
std::vector<float> find_ranges(std::vector<float>& data);

const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 1080;

int num_frames = 0;
float last_time = 0.0f;

float center_x = 0.0f;
float center_y = 0.0f;
float zoom = 1.0;

int main()
{
	// Initialize opengl
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Grej", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Setup shaders
	Shader shaderProgram("res/vertex.shader", "res/fragment.shader");


	float vertices[] =
	{
		//    x      y      z   
			-1.0f, -1.0f, -0.0f,
			 1.0f,  1.0f, -0.0f,
			-1.0f,  1.0f, -0.0f,
			 1.0f, -1.0f, -0.0f
	};

	unsigned int indices[] =
	{
		//  2---,1
		//  | .' |
		//  0'---3
			0, 1, 2,
			0, 3, 1
	};

	unsigned int VBO, VAO, IBO;

	// Generate storage for VBO layout specification
	glGenVertexArrays(1, &VAO);

	// Generate GPU memory space
	glGenBuffers(1, &VBO);

	// Now using vertex array with id VAO
	glBindVertexArray(VAO);

	// Now using GPU memory space with id VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertex data from vertices to VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Something with index buffers
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// Specify layout for VBO and store it in VAO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	last_time = glfwGetTime();

	glEnable(GL_DEPTH_TEST);

	std::vector<float> pixel_data(SCR_WIDTH * SCR_HEIGHT, 0.0f);
	std::vector<float> ranges = { 0.0001f, 0.33333f, 0.66667f, 1.00f };

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.0f, 0.3f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(window);
		countFPS();

		shaderProgram.use();
		shaderProgram.setFloat("zoom", zoom);
		shaderProgram.setFloat("center_x", center_x);
		shaderProgram.setFloat("center_y", center_y);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "color_ranges"), ranges[0], ranges[1], ranges[2], ranges[3]);


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, pixel_data.data());
		ranges = find_ranges(pixel_data);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

	glfwTerminate();
	return 0;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		center_y = center_y + 0.005f * zoom;
		if (center_y > 1.0f)
		{
			center_y = 1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		center_y = center_y - 0.005f * zoom;
		if (center_y < -1.0f)
		{
			center_y = -1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		center_x = center_x - 0.005f * zoom;
		if (center_x < -1.0f)
		{
			center_x = -1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		center_x = center_x + 0.005f * zoom;
		if (center_x > 1.0f)
		{
			center_x = 1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		zoom = zoom * 1.02f;
		if (zoom > 1.0f)
		{
			zoom = 1.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		zoom = zoom * 0.98f;
		if (zoom < 0.00001f)
		{
			zoom = 0.00001f;
		}
	}
}

void countFPS()
{
	double current_time = glfwGetTime();
	num_frames++;
	if (current_time - last_time >= 1.0)
	{
		std::cout << 1000.0 / num_frames << "ms / frame\n";
		num_frames = 0;
		last_time += 1.0;
	}
}


std::vector<float> find_ranges(std::vector<float>& data)
{
	std::sort(data.begin(), data.end());
	int lowest = 0;
	while (data[lowest] == 0.0f)
	{
		++lowest;
	}

	int size = data.size();
	int length = size - lowest;
	std::vector<float> ranges = { data[lowest], data[lowest + length * 3 / 4 - 1], data[lowest + length * 7 / 8 - 1], data[size - 1] };
	return ranges;
}