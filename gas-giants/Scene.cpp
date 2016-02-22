#include "Scene.h"

Scene::Scene() :
	firstUpdateCall(true),
	time(Time()),
	sceneFrozen(false),
	fixedUpdatesPerSecond(60),
	timeSinceLastFixedUpdate(1/60.f)
{
}

Scene::~Scene()
{
}

void Scene::InternalUpdate()
{
	if (firstUpdateCall)
	{
		RenderScene();
		firstUpdateCall = false;
	}

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