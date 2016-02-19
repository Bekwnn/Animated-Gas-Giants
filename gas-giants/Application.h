#pragma once
#include <vector>

#include "Scene.h"
#include "Window.h"

class Window;

class Application {

public:
	Application();
	~Application();

	void AddWindow(Window* window);
	void RemoveWindow(Window * window);
	void CloseApplication();
	void RunApplication();

protected:
	std::vector<std::pair<std::shared_ptr<Window>, bool>> windows; //paired with flag which signals for window deletion
	bool closeApplicationFlag; //closes at end of update loop

};
