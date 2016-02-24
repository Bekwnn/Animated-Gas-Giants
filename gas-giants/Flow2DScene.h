#pragma once
#include "Scene.h"

class Flow2DScene : public Scene
{
public:
	Flow2DScene();

	void Update() override;
	void RenderScene() override;

	GLuint shaderObject;
};