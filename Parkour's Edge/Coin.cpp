#include "Coin.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "HUD.h"

Coin::Coin(Game* game, Actor* parent)
	:Actor(game, parent)
{
	mc = new MeshComponent(this);
	mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Coin.gpmesh"));

	cc = new CollisionComponent(this);
	cc->SetSize(100.0f, 100.0f, 100.0f);
}

void Coin::OnUpdate(float deltaTime)
{
	mRotation += Math::Pi * deltaTime;

	if (cc->Intersect(mGame->mPlayer->GetComponent<CollisionComponent>()))
	{
		mGame->mPlayer->GetHUD()->IncrementCoin();
		Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Coin.wav"), 0);
		SetState(ActorState::Destroy);
	}
}