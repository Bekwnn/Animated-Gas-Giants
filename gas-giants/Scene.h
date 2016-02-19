#pragma once
#include "Time.h"

class Scene {
public:
	Scene();
	virtual ~Scene();

	void InternalUpdate(); //handles time, update, and when to call fixedupdate
	virtual void Update();
	virtual void FixedUpdate();
	virtual void RenderScene();

	void FreezeScene();
	void UnfreezeScene();
	bool GetSceneFrozen();

	void SetFixedUpdatesPerSecond(int a) {
		if (a < 1) a = 1; fixedUpdatesPerSecond = a;
	}

public:
	Time time;

protected:
	int fixedUpdatesPerSecond;
	float timeSinceLastFixedUpdate;
	float timeFrozenAt;
	bool sceneFrozen;
};