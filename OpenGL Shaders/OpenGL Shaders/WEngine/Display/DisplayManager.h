#ifndef _C_WINDOW_
#define _C_WINDOW_

extern GLFWwindow* window;

class DisplayManager
{
public:
	static int createDisplay(int screenWidth, int screenHeight, char *title, bool fullscreen);
	static void setDisplayTitle(const char *title);

	static glm::vec2 getWindowPosition();
	static void setWindowPosition(glm::vec2 pos);

	static glm::vec2 getWindowSize();
	static void setWindowSize(glm::vec2 size);

	static void setWindowLimitSize(glm::vec2 minSize, glm::vec2 maxSize);

	static void minimize();
	static void maximize();

	static void showWindow();
	static void hideWindow();

	static void setCursorOption(int value);


	static int updateDisplay();

	static void closeDisplay();

	static void enableVsync();
	static void disableVsync();

	static bool supports_AnisotropicFiltering;
private:

};

#endif
