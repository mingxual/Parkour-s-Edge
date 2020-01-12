#pragma once
#include "Actor.h"
class SecurityCone : public Actor
{
public:
	SecurityCone(class Game* game, Actor* parent);
	void OnUpdate(float deltaTime);
	int GetMeshIndex();

private:
	class MeshComponent* mc;
	float time_count;
	float max = Math::Pi / 6.0f;
	float height = 100.0f;
	int channel;
	bool start = false;
	float entertime = 2.0f;
	float dangertime = 2.5f;
};

