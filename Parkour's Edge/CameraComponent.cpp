#include "CameraComponent.h"
#include "Actor.h"
#include "Renderer.h"
#include "Game.h"

CameraComponent::CameraComponent(Actor* owner)
	:Component(owner), mPitchAngle(0.0f), mPitchSpeed(0.0f), actualup(Vector3::UnitZ), idealup(Vector3::UnitZ), timer(0.0f), prevup(Vector3::UnitZ)
{

}

void CameraComponent::Update(float deltaTime)
{
	mPitchAngle += mPitchSpeed * deltaTime;
	if (mPitchAngle < -Math::PiOver2 / 2.0f) mPitchAngle = -Math::PiOver2 / 2.0f;
	else if (mPitchAngle > Math::PiOver2 / 2.0f) mPitchAngle = Math::PiOver2 / 2.0f;
	Matrix4 pitchrotation = Matrix4::CreateRotationY(mPitchAngle);
	Matrix4 yawrotation = Matrix4::CreateRotationZ(mOwner->GetRotation());
	Matrix4 combination = pitchrotation * yawrotation;
	Vector3 transform = Vector3::Transform(Vector3::UnitX, combination);

	Vector3 camerapos = mOwner->GetPosition();
	Vector3 targetpos = mOwner->GetPosition() + transform * targetoffset;
	
	timer += deltaTime;
	if (timer >= 0.5f) timer = 0.5f;
	actualup = prevup + timer * 2.0f * (idealup - prevup);

	Matrix4 view = Matrix4::CreateLookAt(camerapos, targetpos, actualup);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(view);
}

void CameraComponent::SetRotation(CollSide curr)
{
	if (curr == CollSide::SideX1) {
		idealup = Vector3(-sin(angle), 0.0f, cos(angle));
	}
	else if (curr == CollSide::SideX2) {
		idealup = Vector3(sin(angle), 0.0f, cos(angle));
	}
	else if (curr == CollSide::SideY1) {
		idealup = Vector3(0.0f, -sin(angle), cos(angle));
	}
	else if (curr == CollSide::SideY2) {
		idealup = Vector3(0.0f, sin(angle), cos(angle));
	}
	else idealup = Vector3::UnitZ;
	prevup = actualup;
	timer = 0.0f;
}