#pragma once
#include "MoveComponent.h"
#include "SDL/SDL.h"
#include "Math.h"
#include "CollisionComponent.h"

enum class MoveState
{
	OnGround,
	Jump,
	Falling,
	WallClimb,
	WallRun
};

class PlayerMove : public MoveComponent
{
public:
	PlayerMove(class Actor* owner);
	~PlayerMove();
	void Update(float deltaTime);
	void ProcessInput(const Uint8* keyState);
	void ChangeState(MoveState newState) { mCurrentState = newState; }
	void UpdateOnGround(float deltaTime);
	void UpdateJump(float deltaTime);
	void UpdateFalling(float deltaTime);
	CollSide FixCollision(class CollisionComponent* self, class CollisionComponent* block);
	void PhysicsUpdate(float deltaTime);
	void AddForce(const Vector3& force);
	void FixXYVelocity();

	void UpdateWallClimb(float deltaTime);
	bool CanWallClimb(CollSide curr);
	void UpdateWallRun(float deltaTime);
	bool CanWallRun(CollSide curr);

private:
	MoveState mCurrentState;
	Vector3 mVelocity;
	Vector3 mAcceleration;
	Vector3 mPendingForces;
	float mMass = 1.0f;
	Vector3 mGravity = Vector3(0.0f, 0.0f, -980.0f);
	Vector3 mJumpForce = Vector3(0.0f, 0.0f, 35000.0f);
	bool mSpace = false;

	Vector3 mClimb = Vector3(0.0f, 0.0f, 1800.0f);
	float mWallClimbTimer;
	Vector3 mWallRunForce = Vector3(0.0f, 0.0f, 1200.0f);
	float mWallRunTimer;
	int mRunningSFX;
};

