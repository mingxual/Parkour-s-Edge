#pragma once
#include "Actor.h"
class Block : public Actor
{
public:
	Block(class Game* game, Actor* parent);
	~Block();
	void SetReflect(bool curr) { reflect = curr; }
	bool GetReflect() { return reflect; }
	void SetRotate(bool curr) { rotatable = curr; }
	bool GetRotate() { return rotatable; }
	void OnUpdate(float deltaTime);

private:
	class MeshComponent* mc;
	class CollisionComponent* cc;
	bool reflect;
	bool rotatable;
	float rotatespeed = Math::Pi / 4.0f;
};

