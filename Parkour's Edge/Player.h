#pragma once
#include "Actor.h"

class Player : public Actor
{
public:
	Player(class Game* mgame, Actor* parent);
	class PlayerMove* pm;
	void SetRespawnPos(Vector3 pos) { respawnPos = pos; }
	Vector3 GetRespawnPos() { return respawnPos; }
	class HUD* GetHUD() { return mHUD; }

private:
	class CameraComponent* camc;
	class CollisionComponent* colc;
	Vector3 respawnPos;
	class HUD* mHUD;

};

