#pragma once
#include "Actor.h"
#include <string>

class Checkpoint : public Actor
{
public:
	Checkpoint(class Game* game, Actor* parent);
	void OnUpdate(float deltaTime);
	void SetActive(bool temp);
	void SetLevelString(std::string str) { mLevelString = str; }
	void SetText(std::string text) { mText = text; }
	std::string GetText() { return mText; }

private:
	class MeshComponent* mc;
	class CollisionComponent* cc;
	bool active;
	std::string mLevelString;
	std::string mText;
};

