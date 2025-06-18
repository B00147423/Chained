#include <iostream>
#include "GLFW/glfw3.h"

int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(640, 480, "Chained Example", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Make the window's context current
	glfwMakeContextCurrent(window);
	// Main loop
	while (!glfwWindowShouldClose(window)) {
		// Render here (for demonstration purposes, we just clear the screen)
		glClear(GL_COLOR_BUFFER_BIT);
		// Swap front and back buffers
		glfwSwapBuffers(window);
		// Poll for and process events
		glfwPollEvents();
	}
	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}