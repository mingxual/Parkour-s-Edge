#include "PlayerMove.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "CameraComponent.h"
#include "Player.h"
#include "Checkpoint.h"

PlayerMove::PlayerMove(Actor* owner)
	:MoveComponent(owner), mWallClimbTimer(0.0f), mWallRunTimer(0.0f), mCurrentState(MoveState::Falling)
{
	mRunningSFX = Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Running.wav"), -1);
	Mix_Pause(mRunningSFX);
}

PlayerMove::~PlayerMove()
{
	Mix_HaltChannel(mRunningSFX);
}

void PlayerMove::Update(float deltaTime)
{	
	if (mCurrentState == MoveState::Falling) {
		Mix_Pause(mRunningSFX);
		UpdateFalling(deltaTime);
	}
	else if (mCurrentState == MoveState::Jump)
	{
		Mix_Pause(mRunningSFX);
		UpdateJump(deltaTime);
	}
	else if (mCurrentState == MoveState::OnGround) 
		UpdateOnGround(deltaTime);
	else if (mCurrentState == MoveState::WallClimb) 
		UpdateWallClimb(deltaTime);
	else if (mCurrentState == MoveState::WallRun) 
		UpdateWallRun(deltaTime);

	if (mOwner->GetPosition().z < -750.0f)
	{
		Player* curr = (Player*)mOwner;
		mOwner->SetPosition(curr->GetRespawnPos());
		mOwner->SetRotation(0.0f);
		mVelocity = Vector3::Zero;
		mPendingForces = Vector3::Zero;
		mCurrentState = MoveState::Falling;
	}
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	Vector3 forward = mOwner->GetForward();
	forward.Normalize();
	if (keyState[SDL_SCANCODE_W] && keyState[SDL_SCANCODE_S]) 
		SetForwardSpeed(0.0f);
	else if (keyState[SDL_SCANCODE_W]) 
		AddForce(forward * 700.0f);
	else if (keyState[SDL_SCANCODE_S]) 
		AddForce(forward * -700.0f);
	else 
		SetForwardSpeed(0.0f);

	Vector3 right = mOwner->GetRight();
	if (keyState[SDL_SCANCODE_A] && keyState[SDL_SCANCODE_D])
		SetStrafeSpeed(0.0f);
	else if (keyState[SDL_SCANCODE_A]) 
		AddForce(right * -700.0f);
	else if (keyState[SDL_SCANCODE_D])
		AddForce(right * 700.0f);
	else 
		SetStrafeSpeed(0.0f);

	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	float x_con = (x / 500.0f) * Math::Pi * 10.0f;
	SetAngularSpeed(x_con);
	float y_con = (y / 500.0f) * Math::Pi * 10.0f;
	mOwner->GetComponent<CameraComponent>()->SetPitchSpeed(y_con);

	if (keyState[SDL_SCANCODE_SPACE] && !mSpace && mCurrentState == MoveState::OnGround)
	{
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Jump.wav"), 0);
		AddForce(mJumpForce);
		mCurrentState = MoveState::Jump;
		mSpace = true;
	}
	else if (!keyState[SDL_SCANCODE_SPACE] && mCurrentState == MoveState::OnGround)
	{
		mSpace = false;
	}
}

void PlayerMove::UpdateOnGround(float deltaTime)
{
	PhysicsUpdate(deltaTime);
	bool oneTop = false;
	CollisionComponent* mOwnercc = mOwner->GetComponent<CollisionComponent>();
	for (auto block : mOwner->GetGame()->mBlocks)
	{
		CollSide curr = FixCollision(mOwnercc, block->GetComponent<CollisionComponent>());
		if (curr == CollSide::None) 
			continue;
		if (curr == CollSide::Top && !oneTop) 
			oneTop = true;
		if (curr != CollSide::Top && curr != CollSide::Bottom)
		{
			if (CanWallClimb(curr))
			{
				mCurrentState = MoveState::WallClimb;
				mWallClimbTimer = 0.0f;
				return;
			}
		}
	}
	if (!oneTop) 
		mCurrentState = MoveState::Falling;

	if (mVelocity.Length() > 50.0f) 
		Mix_Resume(mRunningSFX);
	else 
		Mix_Pause(mRunningSFX);
}

void PlayerMove::UpdateJump(float deltaTime)
{
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);
	CollisionComponent* mOwnercc = mOwner->GetComponent<CollisionComponent>();
	for (auto block : mOwner->GetGame()->mBlocks)
	{
		CollSide curr = FixCollision(mOwnercc, block->GetComponent<CollisionComponent>());
		if (curr == CollSide::None) 
			continue;
		if (curr == CollSide::Bottom)
		{
			mCurrentState = MoveState::Falling;
			mVelocity.z = 0.0f;
		}
		if (curr != CollSide::Top && curr != CollSide::Bottom)
		{
			if (CanWallClimb(curr))
			{
				mCurrentState = MoveState::WallClimb;
				mWallClimbTimer = 0.0f;
				return;
			}
			else if (CanWallRun(curr))
			{
				mCurrentState = MoveState::WallRun;
				mWallRunTimer = 0.0f;
				mOwner->GetComponent<CameraComponent>()->SetRotation(curr);
				return;
			}
		}
	}
	if (mVelocity.z <= 0.0f) 
		mCurrentState = MoveState::Falling;
}

void PlayerMove::UpdateFalling(float deltaTime)
{
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);
	CollisionComponent* mOwnercc = mOwner->GetComponent<CollisionComponent>();
	for (auto block : mOwner->GetGame()->mBlocks)
	{
		CollSide curr = FixCollision(mOwnercc, block->GetComponent<CollisionComponent>());
		if (curr == CollSide::Top)
		{
			Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Land.wav"), 0);
			mCurrentState = MoveState::OnGround;
			mVelocity.z = 0.0f;
		}
	}
}

CollSide PlayerMove::FixCollision(CollisionComponent* self, CollisionComponent* block)
{
	Vector3 offset = Vector3::Zero;
	CollSide curr = self->GetMinOverlap(block, offset);
	if (curr == CollSide::None) 
		return curr;

	if (curr == CollSide::SideX1) 
		AddForce(Vector3(-700.0f, 0.0f, 0.0f));
	else if (curr == CollSide::SideX2) 
		AddForce(Vector3(700.0f, 0.0f, 0.0f));
	else if (curr == CollSide::SideY1) 
		AddForce(Vector3(0.0f, -700.0f, 0.0f));
	else if (curr == CollSide::SideY2) 
		AddForce(Vector3(0.0f, 700.0f, 0.0f));

	Vector3 pos = mOwner->GetPosition();
	pos += offset;
	mOwner->SetPosition(pos);
	return curr;
}

void PlayerMove::PhysicsUpdate(float deltaTime)
{
	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += mAcceleration * deltaTime;
	FixXYVelocity();
	Vector3 pos = mOwner->GetPosition();
	pos += mVelocity * deltaTime;
	mOwner->SetPosition(pos);

	float rotation = mOwner->GetRotation() + mAngularSpeed * deltaTime;
	mOwner->SetRotation(rotation);

	mPendingForces = Vector3::Zero;
}

void PlayerMove::AddForce(const Vector3& force)
{
	mPendingForces += force;
}

void PlayerMove::FixXYVelocity()
{
	Vector2 xyVelocity = Vector2(mVelocity.x, mVelocity.y);
	if (xyVelocity.Length() > 400.0f)
	{
		xyVelocity.Normalize();
		xyVelocity *= 400.0f;
	}
	if (mCurrentState == MoveState::OnGround || mCurrentState == MoveState::WallClimb)
	{
		if (Math::NearZero(mAcceleration.x)) 
			xyVelocity.x *= 0.9f;
		if (Math::NearZero(mAcceleration.y)) 
			xyVelocity.y *= 0.9f;
	}
	if ((xyVelocity.x > 0.0f && mAcceleration.x < 0.0f) || (xyVelocity.x < 0.0f && mAcceleration.x > 0.0f)) 
		xyVelocity.x *= 0.9f;
	if ((xyVelocity.y > 0.0f && mAcceleration.y < 0.0f) || (xyVelocity.y < 0.0f && mAcceleration.y > 0.0f)) 
		xyVelocity.y *= 0.9f;;

	mVelocity.x = xyVelocity.x;
	mVelocity.y = xyVelocity.y;
}

void PlayerMove::UpdateWallClimb(float deltaTime)
{
	Mix_Resume(mRunningSFX);

	AddForce(mGravity);
	if(mWallClimbTimer <= 0.4f) 
		AddForce(mClimb);
	PhysicsUpdate(deltaTime);

	bool istouch = false;
	CollisionComponent* mOwnercc = mOwner->GetComponent<CollisionComponent>();
	for (auto block : mOwner->GetGame()->mBlocks)
	{
		CollSide curr = FixCollision(mOwnercc, block->GetComponent<CollisionComponent>());
		if (curr != CollSide::None) 
			istouch = true;
	}
	if (!istouch || mVelocity.z <= 0.0f) {
		mVelocity.z = 0.0f;
		mCurrentState = MoveState::Falling;
	}
	mWallClimbTimer += deltaTime;
}

bool PlayerMove::CanWallClimb(CollSide curr)
{
	// Check facing wall
	Vector3 sidenormal = Vector3::Zero;
	if (curr == CollSide::SideX1) 
		sidenormal.x = 1;
	else if (curr == CollSide::SideX2) 
		sidenormal.x = -1;
	else if (curr == CollSide::SideY1) 
		sidenormal.y = 1;
	else
		sidenormal.y = -1;

	if (1.0f - Vector3::Dot(sidenormal, mOwner->GetForward()) > 0.1f) 
		return false;
	
	// Check speed
	Vector2 xyVelocity = Vector2(mVelocity.x, mVelocity.y);
	if (xyVelocity.Length() <= 350.0f)
		return false;

	// Check xyvelocity facing wall
	xyVelocity.Normalize();
	Vector2 wallnormal = Vector2(sidenormal.x, sidenormal.y);
	if (1.0f - Vector2::Dot(wallnormal, xyVelocity) > 0.1f)
		return false;

	return true;
}

void PlayerMove::UpdateWallRun(float deltaTime)
{
	Mix_Resume(mRunningSFX);

	AddForce(mGravity);
	if (mWallRunTimer <= 0.4f)
		AddForce(mWallRunForce);
	PhysicsUpdate(deltaTime);

	CollisionComponent* mOwnercc = mOwner->GetComponent<CollisionComponent>();
	for (auto block : mOwner->GetGame()->mBlocks)
	{
		FixCollision(mOwnercc, block->GetComponent<CollisionComponent>());
	}
	if (mVelocity.z <= 0.0f) {
		mCurrentState = MoveState::Falling;
		mOwner->GetComponent<CameraComponent>()->SetRotation(CollSide::None);
	}
	mWallRunTimer += deltaTime;
}

bool PlayerMove::CanWallRun(CollSide curr)
{
	// Check facing wall perpedicular
	Vector3 sidenormal = Vector3::Zero;
	if (curr == CollSide::SideX1) 
		sidenormal.x = 1;
	else if (curr == CollSide::SideX2)
		sidenormal.x = -1;
	else if (curr == CollSide::SideY1)
		sidenormal.y = 1;
	else 
		sidenormal.y = -1;

	if (abs(Vector3::Dot(sidenormal, mOwner->GetForward())) > 0.25f) 
		return false;

	// Check speed
	Vector2 xyVelocity = Vector2(mVelocity.x, mVelocity.y);
	if (xyVelocity.Length() <= 350.0f) 
		return false;

	// Check xyvelocity similar with forward
	xyVelocity.Normalize();
	Vector2 speeddir = Vector2(mOwner->GetForward().x, mOwner->GetForward().y);
	if (1.0f - Vector2::Dot(speeddir, xyVelocity) > 0.25f) 
		return false;

	return true;
}