#include "Checkpoint.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "HUD.h"

Checkpoint::Checkpoint(Game* game, Actor* parent)
	:Actor(game, parent), active(false), mLevelString("")
{
	mc = new MeshComponent(this);
	mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Checkpoint.gpmesh"));
	mc->SetTextureIndex(1);

	cc = new CollisionComponent(this);
	cc->SetSize(25.0f, 25.0f, 25.0f);
}

void Checkpoint::OnUpdate(float deltaTime)
{
	if (active && mState != ActorState::Destroy)
	{
		if (cc->Intersect(mGame->mPlayer->GetComponent<CollisionComponent>()))
		{
			mGame->mPlayer->GetHUD()->ShowCheckPoint(mText);
			Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Checkpoint.wav"), 0);
			mGame->mPlayer->SetRespawnPos(mPosition);
			mGame->mCheckpoints.pop();
			if (mLevelString.length() != 0)
			{
				mGame->SetNextLevel(mLevelString);
				return;
			}
			if (!mGame->mCheckpoints.empty())
			{
				mGame->mCheckpoints.front()->SetActive(true);
			}
			SetState(ActorState::Destroy);
		}
	}
}

void Checkpoint::SetActive(bool temp) 
{
	active = temp;
	if (active) mc->SetTextureIndex(0);
	else mc->SetTextureIndex(1);
}