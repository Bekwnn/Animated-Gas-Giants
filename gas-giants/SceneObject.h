#pragma once

// an object which exists within the scene
class SceneObject {
public:
	SceneObject();
	~SceneObject();

	void Update(double deltaTime);
};