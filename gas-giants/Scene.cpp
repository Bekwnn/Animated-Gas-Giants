#include <GLFW\glfw3.h>

#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::InternalUpdate()
{
	float getTime = (float)glfwGetTime();
	time.deltaTime = getTime - time.totalTime;
	time.totalTime = getTime;

	if (sceneFrozen) return;

	time.currentTime += time.deltaTime;

	//perform fixed updates
	timeSinceLastFixedUpdate += time.deltaTime;
	while (timeSinceLastFixedUpdate > 1.f / fixedUpdatesPerSecond)
	{
		FixedUpdate();
		timeSinceLastFixedUpdate -= 1.f / fixedUpdatesPerSecond;
	}

	//call variable rate update
	Update();
}

void Scene::Update()
{
}

void Scene::FixedUpdate()
{
}

void Scene::RenderScene()
{
}

void Scene::FreezeScene()
{
	if (sceneFrozen) return;

	timeFrozenAt = time.totalTime;
	sceneFrozen = true;
}

void Scene::UnfreezeScene()
{
	if (!sceneFrozen) return;

	sceneFrozen = false;
}

bool Scene::GetSceneFrozen()
{
	return sceneFrozen;
}