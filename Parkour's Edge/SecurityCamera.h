#pragma once
#include "Actor.h"
class SecurityCamera : public Actor
{
public:
	SecurityCamera(class Game* game, Actor* parent);
	~SecurityCamera();
	Quaternion startQ;
	Quaternion endQ;
	float interpTime;
	float pauseTime;
	void OnUpdate(float deltaTime);

private:
	class MeshComponent* mc;
	float time_count;
	int order = 1;
	class SecurityCone* sc;
	int channel;
	bool start = false;
	float distfar = 1500.0f;
	float distnear = 500.0f;
	float distportion = distfar - distnear;
};

