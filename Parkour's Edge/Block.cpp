#include "Block.h"
#include "Game.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "CollisionComponent.h"

Block::Block(Game* game, Actor* parent)
	:Actor(game, parent), reflect(false), rotatable(false)
{
	SetScale(64.0f);
	mc = new MeshComponent(this);
	mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	cc = new CollisionComponent(this);
	cc->SetSize(1.0f, 1.0f, 1.0f);
	mGame->mBlocks.push_back(this);
}

Block::~Block()
{
	for (unsigned int i = 0; i < mGame->mBlocks.size(); i++)
	{
		if (mGame->mBlocks[i] == this)
		{
			mGame->mBlocks.erase(mGame->mBlocks.begin() + i);
			break;
		}
	}
}

void Block::OnUpdate(float deltaTime)
{
	if (rotatable)
	{
		mRotation += rotatespeed * deltaTime;
	}
}