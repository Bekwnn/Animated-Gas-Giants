#include "Window.h"

Window::Window()
{
}

Window::~Window()
{
	glfwDestroyWindow(glfwWindow);
}

void Window::SetScene(Scene* scene)
{
	currentScene = std::shared_ptr<Scene>(scene);
}

void Window::Update()
{
	if (glfwWindowShouldClose(glfwWindow))
	{
		Destory();
		return;
	}

	currentScene->InternalUpdate();

	/* Swap front and back buffers */
	glfwSwapBuffers(glfwWindow);

	/* Poll for and process events */
	glfwPollEvents();
}

Window * Window::CreateWindow(Application* app, int xres, int yres, std::string title, GLFWmonitor * monitor, GLFWwindow * share)
{
	/* Create a windowed mode window and its OpenGL context */
	Window* w = new Window();
	w->application = app;
	w->glfwWindow = glfwCreateWindow(xres, yres, title.c_str(), monitor, share);
	if (!w->glfwWindow)
	{
		return nullptr;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(w->glfwWindow);
	return w;
}

void Window::Destory()
{
	application->RemoveWindow(this);
}
