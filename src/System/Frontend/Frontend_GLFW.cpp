/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Frontend_GLFW.h"

#include "glad/glad.h"

#include "Main.h"

namespace System
{
	namespace Frontend
	{
		// GLFW callbacks
		static void GLFW_FramebufferSizeCallback(GLFWwindow *window, int fb_width, int fb_height)
		{
			(void)window;
			
			// Use framebuffer viewport
			glViewport(0, 0, fb_width, fb_height);
		}
		
		// Frontend GLFW class
		// Constructor and destructor
		Frontend_GLFW::Frontend_GLFW()
		{
			// Inititialize GLFW
			glfwInit();
			
			// Set window hints
			// OpenGL 3.2 Core Profile
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
			
			// Set window to be non-resizable
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			
			// Create window
			if ((glfw_window = glfwCreateWindow(1, 1, "", nullptr, nullptr)) == nullptr)
				throw PaperPup::Exception("[System::Frontend::Frontend_GLFW::SetWindow] Failed to create window");
			glfwMakeContextCurrent(glfw_window);
			
			// Define callback for window resizing
			glfwSetFramebufferSizeCallback(glfw_window, GLFW_FramebufferSizeCallback);
			
			// Enable vsync
			if (glfwExtensionSupported("GLX_EXT_swap_control_tear") || glfwExtensionSupported("WGL_EXT_swap_control_tear"))
				glfwSwapInterval(-1);
			else
				glfwSwapInterval(1);
			
			// Initialize GLAD
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				throw PaperPup::Exception("[System::Frontend::Frontend_GLFW::SetWindow] Failed to initialize GLAD");
			if (!GLAD_GL_VERSION_3_2)
				throw PaperPup::Exception("[System::Frontend::Frontend_GLFW::SetWindow] OpenGL 3.2 is not available");
		}
		
		Frontend_GLFW::~Frontend_GLFW()
		{
			// Destroy window
			if (glfw_window != nullptr)
				glfwDestroyWindow(glfw_window);
			
			// Deinitialize GLFW
			glfwTerminate();
		}
		
		// Window interface
		void Frontend_GLFW::Window_Set(std::string title, unsigned int width, unsigned int height)
		{
			// Use given title and dimensions
			window_title = title;
			window_width = width;
			window_height = height;
			
			// Get monitor video mode
			GLFWmonitor *monitor = glfwGetPrimaryMonitor();
			
			const GLFWvidmode *mode;
			if (monitor != NULL)
				mode = glfwGetVideoMode(monitor);
			else
				mode = NULL;
			
			// Hide window
			glfwHideWindow(glfw_window);
			
			// Update window properties
			glfwSetWindowTitle(glfw_window, title.c_str());
			glfwSetWindowSize(glfw_window, window_width, window_height);
			
			// Center window
			if (mode != NULL)
				glfwSetWindowPos(glfw_window, (mode->width - window_width) / 2, (mode->height - window_height) / 2);
			glfwShowWindow(glfw_window);
		}
		
		void Frontend_GLFW::Window_SwapBuffers()
		{
			// Swap window buffers
			glfwSwapBuffers(glfw_window);
		}
		
		void Frontend_GLFW::Input_HandleEvents()
		{
			// Process GLFW events
			glfwPollEvents();
		}
		
		bool Frontend_GLFW::Input_ShouldClose()
		{
			// Check if GLFW has processed an event to close the window
			return glfwWindowShouldClose(glfw_window);
		}
	}
}
