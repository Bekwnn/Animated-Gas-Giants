#pragma once
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <memory>
#include <string>

#include "Scene.h"
#include "Application.h"

class Application;

class Window {
public:
	virtual ~Window();
protected:
	Window();

public:
	void SetScene(Scene* scene);
	void Update();

	static Window* MakeWindow(Application* app, int xres = 640, int yres = 480, std::string title = "", GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL);
	void Destory();

protected:
	// Scenes exist outside a window so they can be moved, swapped, or temporarily removed
	std::shared_ptr<Scene> currentScene;
	GLFWwindow* glfwWindow;
	Application* application;
};