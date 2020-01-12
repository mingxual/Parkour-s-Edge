#include "LaserComponent.h"
#include "Actor.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "Game.h"
#include "Renderer.h"
#include "Block.h"
#include "Player.h"

LaserComponent::LaserComponent(Actor* mowner)
	:MeshComponent(mowner, true)
{
	SetMesh(mOwner->GetGame()->GetRenderer()->GetMesh("Assets/Laser.gpmesh"));
}

void LaserComponent::Update(float deltaTime)
{
	// Clear the vector
	ls.clear();
	// Calculate the first linesegment
	LineSegment currls;
	currls.mStart = mOwner->GetWorldPosition();
	currls.mEnd = mOwner->GetWorldPosition() + 500.0f * mOwner->GetWorldForward();
	ls.push_back(currls);

	CastInfo temp;
	if (SegmentCast((Actor*)mOwner->GetGame()->mPlayer, currls, temp))
	{
		Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Laser.wav"), 0);
		ls[ls.size() - 1].mEnd = temp.mPoint;
		mOwner->GetGame()->mPlayer->SetPosition(mOwner->GetGame()->mPlayer->GetRespawnPos());
	}
	else if (SegmentCast(mOwner->GetGame()->mBlocks, currls, temp, mOwner->mParent))
	{
		ls[ls.size() - 1].mEnd = temp.mPoint;
		Block* currBlock = (Block*)temp.mActor;
		Vector3 lastDir = mOwner->GetWorldForward();
		
		if (currBlock->GetReflect())
		{
			while (true)
			{
				LineSegment newls;
				newls.mStart = ls[ls.size() - 1].mEnd;
				Vector3 currDir = Vector3::Reflect(lastDir, temp.mNormal);
				newls.mEnd = newls.mStart + 500.0f * currDir;
				lastDir = currDir;
				ls.push_back(newls);
				if (SegmentCast((Actor*)mOwner->GetGame()->mPlayer, newls, temp))
				{
					Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Laser.wav"), 0);
					ls[ls.size() - 1].mEnd = temp.mPoint;
					mOwner->GetGame()->mPlayer->SetPosition(mOwner->GetGame()->mPlayer->GetRespawnPos());
				}
				else if (SegmentCast(mOwner->GetGame()->mBlocks, newls, temp, currBlock))
				{
					ls[ls.size() - 1].mEnd = temp.mPoint;
					currBlock = (Block*)temp.mActor;
					if (currBlock->GetReflect()) continue;
				}
				break;
			}
		}	
	}
}

void LaserComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		for (unsigned int i = 0; i < ls.size(); i++)
		{
			// Set the world transform
			shader->SetMatrixUniform("uWorldTransform",
				WorldTransform(ls[i]));
			// Set the active texture
			Texture* t = mMesh->GetTexture(mTextureIndex);
			if (t)
			{
				t->SetActive();
			}
			// Set the mesh's vertex array as active
			VertexArray* va = mMesh->GetVertexArray();
			va->SetActive();
			// Draw
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

Matrix4 LaserComponent::WorldTransform(LineSegment ls)
{
	Vector3 dist = ls.mEnd - ls.mStart;
	Matrix4 scale = Matrix4::CreateScale(Vector3(dist.Length(), 1.0f, 1.0f));
	
	dist.Normalize();
	float dot = Vector3::Dot(Vector3::UnitX, dist);
	Matrix4 rotation;
	if (dot == 1.0f) rotation = Matrix4::CreateFromQuaternion(Quaternion::Identity);
	else if (dot == -1.0f) rotation = Matrix4::CreateRotationZ(Math::Pi);
	else {
		float angle = Math::Acos(dot);
		Vector3 axis = Vector3::Cross(Vector3::UnitX, dist);
		axis.Normalize();
		Quaternion temp(axis, angle);
		rotation = Matrix4::CreateFromQuaternion(temp);
	}
	Vector3 centerpos = ls.PointOnSegment(0.5f);
	Matrix4 translation = Matrix4::CreateTranslation(centerpos);
	return (scale * rotation * translation);
}