#include "Application.h"
#include "Flow2DScene.h"

int main(void)
{
	Application app;
	Window* w = Window::MakeWindow(&app);
	w->SetScene(new Flow2DScene);
	app.AddWindow(w);
	app.RunApplication();
	return 0;
}