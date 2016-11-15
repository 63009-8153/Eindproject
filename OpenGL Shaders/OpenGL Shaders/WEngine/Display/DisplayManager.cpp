#include "../header.h"

#include "DisplayManager.h"

bool DisplayManager::supports_AnisotropicFiltering = false;

//Create a new display
int DisplayManager::createDisplay(int screenWidth, int screenHeight, char *title, bool fullscreen)
{
	//If a display already exists, close it.
	if (window != NULL) DisplayManager::closeDisplay();

	//glfwWindowHint(GLFW_SAMPLES, 8); // 8x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); //We don't want the old OpenGL

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(screenWidth, screenHeight, title, ((fullscreen) ? glfwGetPrimaryMonitor() : NULL), NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}
	if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	//Enable multisampling in OpenGL
	glEnable(GL_MULTISAMPLE);

	//FILE *file;
	//fopen_s(&file, "supportedExtensions.txt", "w");

	GLint num = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num);
	while (0<--num) {
		std::string t = (const char*)glGetStringi(GL_EXTENSIONS, num - 1);
		//fprintf(file, "%s\n", t.c_str());

		if(t == "GL_EXT_texture_filter_anisotropic") supports_AnisotropicFiltering = true;
	}

	//fclose(file);

	return 0;
}
//Close the display
void DisplayManager::closeDisplay()
{
	glfwDestroyWindow(window);
}

//Set display title
void DisplayManager::setDisplayTitle(const char *title)
{
	glfwSetWindowTitle(window, title);
}

//Get the position of the window
glm::vec2 DisplayManager::getWindowPosition()
{
	int x, y;
	glfwGetWindowPos(window, &x, &y);
	return glm::vec2(x, y);
}
//Set the position of the window
void DisplayManager::setWindowPosition(glm::vec2 pos)
{
	glfwSetWindowPos(window, (int)pos.x, (int)pos.y);
}

//Get the size of the window
glm::vec2 DisplayManager::getWindowSize()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return glm::vec2(width, height);
}
//Set the size of the window
void DisplayManager::setWindowSize(glm::vec2 size)
{
	glfwSetWindowSize(window, (int)size.x, (int)size.y);
}

//Set window resize limits
void DisplayManager::setWindowLimitSize(glm::vec2 minSize, glm::vec2 maxSize)
{
	glfwSetWindowSizeLimits(window, minSize.x, minSize.y, maxSize.x, maxSize.y);
}

//Minimize display
void DisplayManager::minimize()
{
	glfwIconifyWindow(window);
}
//Maximize display
void DisplayManager::maximize()
{
	glfwMaximizeWindow(window);
}

//Show window
void DisplayManager::showWindow()
{
	glfwShowWindow(window);
}
//Hide window
void DisplayManager::hideWindow()
{
	glfwHideWindow(window);
}

//Set the value for the cursor
//  - `GLFW_CURSOR_NORMAL` makes the cursor visible and behaving normally.
//  - `GLFW_CURSOR_HIDDEN` makes the cursor invisible when it is over the client
//     area of the window but does not restrict the cursor from leaving.
//  - `GLFW_CURSOR_DISABLED` hides and grabs the cursor, providing virtual
//     and unlimited cursor movement.This is useful for implementing for
//     example 3D camera controls.
void DisplayManager::setCursorOption(int value)
{
	glfwSetInputMode(window, GLFW_CURSOR, value);
}

//Update the display
int DisplayManager::updateDisplay()
{
	if (glfwWindowShouldClose(window) != 0) return -1;


	return 0;
}

void DisplayManager::enableVsync()
{
	//Enable vsync. Will limit fps to screen refresh rate!
	glfwSwapInterval(1);
}
void DisplayManager::disableVsync()
{
	//Set vsync off for max fps :)!
	glfwSwapInterval(0);
}