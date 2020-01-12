#pragma once
#include "MeshComponent.h"
#include "SegmentCast.h"

class LaserComponent : public MeshComponent
{
public:
	LaserComponent(class Actor* owner);
	void Update(float deltaTime);
	void Draw(class Shader* shader);
	Matrix4 WorldTransform(LineSegment ls);

private:
	std::vector<LineSegment> ls;
};

