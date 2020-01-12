#include "HUD.h"
#include "Actor.h"
#include "Font.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Player.h"
#include "SecurityCamera.h"

HUD::HUD(Actor* owner)
	:UIComponent(owner), timer(0.0f), init(true)
{
	mFont = new Font();
	mFont->Load("Assets/Inconsolata-Regular.ttf");
	mTimerText = mFont->RenderText("00:00.00");
	mCoinCounter = mFont->RenderText(std::to_string(mOwner->GetGame()->mCoin) + "/55");
	mCheckText = mFont->RenderText(" ");
	mRadarBg = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Radar.png");
	mRadarAw = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/RadarArrow.png");
	mBlip = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Blip.png");
}

HUD::~HUD()
{
	mFont->Unload();
}

void HUD::Update(float deltaTime)
{
	mOwner->GetGame()->mTimer += deltaTime;
	mTimerText->Unload();
	delete mTimerText;
	timer -= deltaTime;

	mTimerText = mFont->RenderText(ConvertTimer());

	if (timer <= 0.0f && !init)
	{
		init = true;
		mCheckText->Unload();
		delete mCheckText;
		mCheckText = mFont->RenderText(" ");
	}
}

void HUD::Draw(Shader* shader)
{
	DrawTexture(shader, mTimerText, Vector2(-420.0f, -325.0f));
	DrawTexture(shader, mCoinCounter, Vector2(-450.0f, -290.0f));
	DrawTexture(shader, mCheckText, Vector2::Zero);
	DrawTexture(shader, mRadarBg, Vector2(385.0f, -295.0f));
	DrawTexture(shader, mRadarAw, Vector2(385.0f, -295.0f));
	UpdateBlip();
	for (unsigned int i = 0; i < mBlipsPos.size(); i++)
	{
		DrawTexture(shader, mBlip, mBlipsPos[i], 1, mBlipsAngle[i]);
	}
}

void HUD::IncrementCoin()
{
	mOwner->GetGame()->mCoin += 1;
	mCoinCounter->Unload();
	delete mCoinCounter;
	mCoinCounter = mFont->RenderText(std::to_string(mOwner->GetGame()->mCoin) + "/55");
}

void HUD::ShowCheckPoint(std::string text)
{
	timer = 5.0f;
	init = false;
	mCheckText->Unload();
	delete mCheckText;
	mCheckText = mFont->RenderText(text);
}

std::string HUD::ConvertTimer()
{
	int integral = static_cast<int>(mOwner->GetGame()->mTimer * 100);
	int fraction = integral % 100;
	int min = (integral / 100) / 60;
	int second = (integral / 100) % 60;
	std::string time = "";
	if (min == 0) time += "00";
	else if (min < 10) time += "0" + std::to_string(min);
	else time += std::to_string(min);
	time += ':';
	if (second == 0) time += "00";
	else if (second < 10) time += "0" + std::to_string(second);
	else time += std::to_string(second);
	time += '.';
	if (fraction < 10) time += "0" + std::to_string(fraction);
	else time += std::to_string(fraction);
	return time;
}

void HUD::UpdateBlip()
{
	// Convert player forward and calculate the rotation matrix
	Vector2 forward = Vector2(mOwner->GetGame()->mPlayer->GetForward().x, mOwner->GetGame()->mPlayer->GetForward().y);
	float angle = atan2(forward.y, forward.x);
	Matrix3 rotation = Matrix3::CreateRotation(angle);
	// Convert object pos into 2d
	Vector2 pos = Vector2(mOwner->GetGame()->mPlayer->GetPosition().y, mOwner->GetGame()->mPlayer->GetPosition().x);
	std::vector<SecurityCamera*> mCameras = mOwner->GetGame()->mCameras;
	mBlipsPos.clear();
	mBlipsAngle.clear();
	for (auto camera : mCameras)
	{
		Vector2 curr_pos = Vector2(camera->GetPosition().y, camera->GetPosition().x);
		Vector2 pos_diff = curr_pos - pos;
		if (pos_diff.Length() < 1500.0f)
		{
			pos_diff = pos_diff * (92.0f / 1500.0f);
			Vector2 transformed = Vector2::Transform(pos_diff, rotation);
			Vector2 final = transformed + Vector2(385.0f, -295.0f);
			mBlipsPos.push_back(final);
			Vector2 curr_forward = Vector2(camera->GetWorldForward().y, camera->GetWorldForward().x);
			Vector2 curr_trans = Vector2::Transform(curr_forward, rotation);
			float curr_angle = atan2(curr_trans.y, curr_trans.x);
			mBlipsAngle.push_back(curr_angle);
		}
	}
}

