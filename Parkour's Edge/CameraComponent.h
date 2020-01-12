#pragma once
#include "Component.h"
#include "SDL/SDL.h"
#include "Math.h"
#include "PlayerMove.h"

class CameraComponent : public Component
{
public:
	CameraComponent(class Actor* owner);
	void Update(float deltaTime);
	float GetPitchSpeed() { return mPitchSpeed; }
	void SetPitchSpeed(float pitchspeed) { mPitchSpeed = pitchspeed; }
	void SetRotation(CollSide curr);

private:
	float targetoffset = 50.0f;
	float mPitchAngle;
	float mPitchSpeed;
	Vector3 actualup;
	Vector3 idealup;
	Vector3 prevup;
	float timer;
	float angle = Math::Pi / 9.0f;
};

