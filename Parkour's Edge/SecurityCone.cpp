#include "SecurityCone.h"
#include "Game.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Player.h"

SecurityCone::SecurityCone(Game* game, Actor* parent)
	:Actor(game, parent), time_count(0.0f), channel(-1)
{
	mc = new MeshComponent(this, true);
	mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cone.gpmesh"));
	mc->SetTextureIndex(0);

	mPosition = Vector3(30.0f, 0.0f, 0.0f);
	mScale = 3.0f;
}

void SecurityCone::OnUpdate(float deltaTime)
{
	Vector3 distance = mGame->mPlayer->GetPosition() - GetWorldPosition();
	Vector3 dir = mGame->mPlayer->GetPosition() - GetWorldPosition();
	dir.Normalize();
	Vector3 forward = GetWorldForward();
	
	float dotproduct = Vector3::Dot(dir, forward);
	float angle = acos(dotproduct);
	float projectHeight = Vector3::Dot(distance, forward);
	if (angle <= max && projectHeight <= mScale * height)
	{
		if (!start)
		{
			start = true;
			channel = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/SecurityDetected.wav"), 0);
		}
		time_count += deltaTime;
		if (time_count <= entertime) mc->SetTextureIndex(1);
		else
		{
			mc->SetTextureIndex(2);
			if (time_count >= dangertime) mGame->mPlayer->SetPosition(mGame->mPlayer->GetRespawnPos());
		}
	}
	else
	{
		start = false;
		if (channel != -1)
		{
			Mix_HaltChannel(channel);
			channel = -1;
		}
		time_count = 0.0f;
		mc->SetTextureIndex(0);
	}
}

int SecurityCone::GetMeshIndex() 
{ 
	return mc->GetTextureIndex(); 
}