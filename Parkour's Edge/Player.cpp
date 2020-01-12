#include "Player.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PlayerMove.h"
#include "CameraComponent.h"
#include "HUD.h"

Player::Player(Game* game, Actor* parent)
	:Actor(game, parent)
{
	pm = new PlayerMove(this);
	camc = new CameraComponent(this);
	colc = new CollisionComponent(this);
	colc->SetSize(50.0f, 175.0f, 50.0f);
	mHUD = new HUD(this);
}