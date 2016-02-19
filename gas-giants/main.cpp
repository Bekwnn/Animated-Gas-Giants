#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "Application.h"

int main(void)
{
	Application app;
	Window* w = Window::CreateWindow(&app);
	w->SetScene(new Scene);
	app.AddWindow(w);
	app.RunApplication();
	return 0;
}