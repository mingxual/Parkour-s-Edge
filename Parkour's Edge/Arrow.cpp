#include "Arrow.h"
#include "Game.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Player.h"
#include "Checkpoint.h"

Arrow::Arrow(Game* game, Actor* parent)
	:Actor(game, parent)
{
	mc = new MeshComponent(this);
	mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Arrow.gpmesh"));

	mScale = 0.15f;
}

void Arrow::OnUpdate(float deltaTime)
{
	if (mState == ActorState::Destroy) return;

	if (!mGame->mCheckpoints.empty())
	{
		Vector3 dir = mGame->mCheckpoints.front()->GetPosition() - mGame->mPlayer->GetPosition();
		dir.Normalize();

		float dot = Vector3::Dot(Vector3::UnitX, dir);
		if (dot == 1.0f) q = Quaternion::Identity;
		else if (dot == -1.0f) q = Quaternion(Vector3::UnitZ, Math::Pi);
		else {
			float angle = Math::Acos(dot);
			Vector3 axis = Vector3::Cross(Vector3::UnitX, dir);
			axis.Normalize();
			q = Quaternion(axis, angle);
		}
	}
	else q = Quaternion::Identity;

	mPosition = mGame->GetRenderer()->Unproject(Vector3(0.0f, 250.0f, 0.1f));
}
