#include "Application.h"
#include <GL\glew.h>
#include <GLFW\glfw3.h>

Application::Application()
{
	/* Initialize the glfw library */
	if (!glfwInit())
		return;
}

Application::~Application()
{
	/* terminate the glfw library */
	glfwTerminate();
}

void Application::AddWindow(Window* window)
{
	windows.push_back({ std::shared_ptr<Window>(window), false });
}

void Application::RemoveWindow(Window* window)
{
	auto iter = windows.begin();
	while (iter != windows.end())
	{
		if (iter->first.get() == window)
		{
			iter->second = true;
			if (iter->second) printf("Closing window.\n");
		}
		++iter;
	}
}

void Application::CloseApplication()
{
	closeApplicationFlag = true;
}

void Application::RunApplication()
{
	while (!windows.empty() && !closeApplicationFlag)
	{
		auto iter = windows.begin();
		while (iter != windows.end())
		{
			if (iter->second)
			{
				iter = windows.erase(iter);
				continue;
			}
			iter->first.get()->Update();
			++iter;
		}
	}

	printf("Closing Application.\n");

	if (!windows.empty()) windows.clear();
}
