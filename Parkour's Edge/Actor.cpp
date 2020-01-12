#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game, Actor* parent)
	:mGame(game), mParent(parent)
	,mState(ActorState::Active)
	,mPosition(Vector3::Zero)
	,mScale(1.0f)
	,mRotation(0.0f)
{
	if (mParent == NULL) mGame->AddActor(this);
	else mParent->AddChild(this);
}

Actor::~Actor()
{
	// TODO
	while (!mChildren.empty()) delete mChildren.back();

	if (mParent == NULL) mGame->RemoveActor(this);
	else mParent->RemoveChild(this);

	for (auto component : mComponents)
	{
		delete component;
	}
	mComponents.clear();
}

void Actor::Update(float deltaTime)
{
	// TODO
	CalcWorldTransform();

	if (mState == ActorState::Active) 
	{
		for (auto component : mComponents) 
		{
			component->Update(deltaTime);
		}
		OnUpdate(deltaTime);
	}

	CalcWorldTransform();
	for (unsigned int i = 0; i < mChildren.size(); i++)
		mChildren[i]->Update(deltaTime);
}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState)
{
	// TODO
	if (mState == ActorState::Active)
	{
		for (auto component : mComponents)
		{
			if (component != NULL)
			{
				component->ProcessInput(keyState);
			}
		}
		OnProcessInput(keyState);
	}
}

void Actor::OnProcessInput(const Uint8* keyState)
{
}

void Actor::AddComponent(Component* c)
{
	mComponents.emplace_back(c);
	std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
		return a->GetUpdateOrder() < b->GetUpdateOrder();
	});
}

Vector3 Actor::GetForward()
{
	float angle = GetRotation();
	return Vector3(cos(angle), sin(angle), 0.0f);
}

Vector3 Actor::GetRight()
{
	float angle = GetRotation() + Math::PiOver2;
	return Vector3(cos(angle), sin(angle), 0.0f);
}

Vector3 Actor::GetQuatForward()
{
	Vector3 forward = Vector3::Transform(Vector3::UnitX, q);
	forward.Normalize();
	return forward;
}

void Actor::CalcWorldTransform()
{
	Matrix4 scale = Matrix4::CreateScale(mScale);
	Matrix4 rotation = Matrix4::CreateRotationZ(mRotation);
	Matrix4 rotat = Matrix4::CreateFromQuaternion(q);
	Matrix4 translation = Matrix4::CreateTranslation(mPosition);
	mWorldTransform = scale * rotation * rotat * translation;

	if (mParent != NULL)
	{
		if (mInheritScale)
		{
			mWorldTransform *= mParent->mWorldTransform;
		}
		else
		{
			mWorldTransform *= mParent->CalcWorldRotTrans();
		}
	}
}

Matrix4 Actor::CalcWorldRotTrans()
{
	Matrix4 rotation = Matrix4::CreateRotationZ(mRotation);
	Matrix4 rotat = Matrix4::CreateFromQuaternion(q);
	Matrix4 translation = Matrix4::CreateTranslation(mPosition);
	Matrix4 temp = rotation * rotat * translation;

	if (mParent != NULL) temp *= mParent->CalcWorldRotTrans();

	return temp;
}

Vector3 Actor::GetWorldPosition()
{
	return mWorldTransform.GetTranslation();
}

Vector3 Actor::GetWorldForward()
{
	return mWorldTransform.GetXAxis();
}

void Actor::AddChild(Actor* child)
{
	mChildren.push_back(child);
}

void Actor::RemoveChild(Actor* child)
{
	for (unsigned int i = 0; i < mChildren.size(); i++)
	{
		if (mChildren[i] == child)
		{
			mChildren.erase(mChildren.begin() + i);
			return;
		}
	}
}