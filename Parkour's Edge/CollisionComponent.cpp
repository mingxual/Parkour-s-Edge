#include "CollisionComponent.h"
#include "Actor.h"

CollisionComponent::CollisionComponent(class Actor* owner)
:Component(owner)
,mWidth(0.0f)
,mHeight(0.0f)
,mDepth(0.0f)
{
	
}

CollisionComponent::~CollisionComponent()
{
	
}

bool CollisionComponent::Intersect(const CollisionComponent* other)
{
	Vector3 aMin = GetMin();
	Vector3 aMax = GetMax();
	Vector3 bMin = other->GetMin();
	Vector3 bMax = other->GetMax();

	bool no = aMax.x < bMin.x ||
		aMax.y < bMin.y ||
		aMax.z < bMin.z ||
		bMax.x < aMin.x ||
		bMax.y < aMin.y ||
		bMax.z < aMin.z;

	return !no;
}

Vector3 CollisionComponent::GetMin() const
{
	Vector3 v = mOwner->GetPosition();
	v.x -= mDepth * mOwner->GetScale() / 2.0f;
	v.y -= mWidth * mOwner->GetScale() / 2.0f;
	v.z -= mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetMax() const
{
	Vector3 v = mOwner->GetPosition();
	v.x += mDepth * mOwner->GetScale() / 2.0f;
	v.y += mWidth * mOwner->GetScale() / 2.0f;
	v.z += mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

const Vector3& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(
	const CollisionComponent* other, Vector3& offset)
{
	offset = Vector3::Zero;
	if (!Intersect(other)) return CollSide::None;

	float dx1 = abs(other->GetMin().x - this->GetMax().x);
	float minimum = dx1;
	float dx2 = abs(this->GetMin().x - other->GetMax().x);
	minimum = fmin(minimum, dx2);
	float dy1 = abs(other->GetMin().y - this->GetMax().y);
	minimum = fmin(minimum, dy1);
	float dy2 = abs(this->GetMin().y - other->GetMax().y);
	minimum = fmin(minimum, dy2);
	float dz1 = abs(other->GetMin().z - this->GetMax().z);
	minimum = fmin(minimum, dz1);
	float dz2 = abs(this->GetMin().z - other->GetMax().z);
	minimum = fmin(minimum, dz2);
	if (minimum == dz1)
	{
		offset.z = -minimum;
		return CollSide::Bottom;
	}
	else if (minimum == dz2)
	{
		offset.z = minimum;
		return CollSide::Top;
	}
	else if (minimum == dx1)
	{
		offset.x = -minimum;
		return CollSide::SideX1;
	}
	else if (minimum == dx2)
	{
		offset.x = minimum;
		return CollSide::SideX2;
	}
	else if (minimum == dy1)
	{
		offset.y = -minimum;
		return CollSide::SideY1;
	}
	else
	{
		offset.y = minimum;
		return CollSide::SideY2;
	}
}
