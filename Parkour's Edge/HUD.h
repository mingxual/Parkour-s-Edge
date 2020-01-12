#pragma once
#include "UIComponent.h"
#include <string>
#include <vector>

class HUD : public UIComponent
{
public:
	HUD(class Actor* owner);
	~HUD();
	void Update(float deltaTime);
	void Draw(class Shader* shader);
	void IncrementCoin();
	void ShowCheckPoint(std::string text);
	std::string ConvertTimer();
	void UpdateBlip();

private:
	class Font* mFont;
	class Texture* mTimerText;
	class Texture* mCoinCounter;
	class Texture* mCheckText;
	float timer;
	bool init;
	class Texture* mRadarBg;
	class Texture* mRadarAw;
	class Texture* mBlip;
	std::vector<Vector2> mBlipsPos;
	std::vector<float> mBlipsAngle;
};

