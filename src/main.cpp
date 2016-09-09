#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <sstream>

#include "..\include\simulation.h"
#include "..\include\utils.h"

namespace {
	struct setup_status {
		uint16_t code;
		std::string msg;
		GLFWwindow* window;
	};

	setup_status setup() {
		// GLFW Init
		if (!glfwInit()) 
			return {1, "GLFW failed to initialise", nullptr};
		
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		GLFWwindow* window = glfwCreateWindow(1366, 768, "Vehicles", NULL, NULL);
		if (!window) {
			glfwTerminate();
			return {1, "GFLW failed to create window", nullptr};
		} else {
			glfwMakeContextCurrent(window);
		}

		// Glew Init
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			glfwTerminate();
			return {1, "Glew failed to initialise", nullptr};
		}
	
		// GL Config
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glClearColor(0.f, 0.f, 0.f, 0.f);
	
		// Flush GL errors;
		err = glGetError();

		return {
			0,
			[]() -> std::string {
				std::stringstream ss;
				ss << "Device Vendor: "   << glGetString(GL_VENDOR)                   << std::endl;
				ss << "Device Renderer: " << glGetString(GL_RENDERER)                 << std::endl;
				ss << "OpenGL Version: "  << glGetString(GL_VERSION)                  << std::endl;
				ss << "GLSL Version: "    << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
				return ss.str();
			}(),
			window
		};
	}

	bool check_running(GLFWwindow* window, int duration) {
		return !glfwWindowShouldClose(window) && utils::elapsed_time() < duration;
	}
}

int main() {
	setup_status status = setup();
	std::cout << status.msg << std::endl;

	simulation::init();

	float elapsed = 0.f;
	float fps = 0.f;
	float start = utils::elapsed_time();

	while (check_running(status.window, 6000)) {
		{ // Per-frame updating and drawing here
			simulation::update();

			glClear(GL_COLOR_BUFFER_BIT);
			simulation::draw(fps);
			glfwPollEvents();
			glfwSwapBuffers(status.window);
		}

		{ // Frame metrics and logging here
			elapsed = utils::elapsed_time();
			fps = 1.f / (elapsed - start);
			start = elapsed;
		}
	}

	simulation::destroy();
	glfwTerminate();

	return 0;
}