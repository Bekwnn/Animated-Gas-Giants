#pragma once

struct Time {
	float deltaTime;
	float currentTime;	// freezes during pause
	float totalTime;	// continues during pause

	Time() :
		deltaTime(0.0f),
		currentTime(0.0f),
		totalTime(0.0f)
	{}
};