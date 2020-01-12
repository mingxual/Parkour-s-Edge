#include "SecurityCamera.h"
#include "Game.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "SecurityCone.h"
#include "Player.h"

SecurityCamera::SecurityCamera(Game* game, Actor* parent)
	:Actor(game, parent), time_count(0.0f), interpTime(0.0f), pauseTime(0.0f), channel(-2)
{
	mc = new MeshComponent(this);
	mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Camera.gpmesh"));
	sc = new SecurityCone(mGame, this);
	mGame->mCameras.push_back(this);
}

SecurityCamera::~SecurityCamera()
{
	if (channel != -1) Mix_HaltChannel(channel);
}

void SecurityCamera::OnUpdate(float deltaTime)
{
	float distance = (mGame->mPlayer->GetPosition() - mPosition).Length();
	
	if (channel != -1)
	{
		if (distance > distfar) Mix_Volume(channel, 0);
		else if (distance < distnear) Mix_Volume(channel, 128);
		else
		{
			float portion = (distfar - distance) / distportion;
			Mix_Volume(channel, static_cast<int>(portion * 128));
		}
	}
	
	if (sc->GetMeshIndex() != 0)
	{
		if (start)
		{
			start = false;
			Mix_HaltChannel(channel);
			channel = -1;
		}
		return;
	}

	time_count += deltaTime;
	float frac = time_count / interpTime;
	if (frac > 1.0f)
	{
		frac = 1.0f;
		if (start)
		{
			start = false;
			Mix_HaltChannel(channel);
			channel = -1;
			Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotorStop.wav"), 0);
		}
	}
	else if (frac < 1.0f && !start)
	{
		start = true;
		channel = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotor.wav"), 0);
	}

	if (order == 1) q = Quaternion::Lerp(startQ, endQ, frac);
	else q = Quaternion::Lerp(endQ, startQ, frac);
	
	if (frac == 1.0f && time_count >= interpTime + pauseTime)
	{
		if (order == 1) order = 0;
		else order = 1;
		time_count = 0.0f;
	}
}