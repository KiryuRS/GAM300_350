#include "stdafx.h"
#include "Physics.h"
#include "Matrix3x3.h"

#define lineOffSet 0.1f
#define marginOfError 0.00001f

//*--------------------------------------------------------------------------- COLLISION STUFFS
//static void Momentum(Vector3& v1, float m1, Vector3& v2, float m2)
//{
//  Vector3 M1V1M2V2 = m1 * v1 + m2 * v2;
//  std::cout << M1V1M2V2 << std::endl;
//  Vector3 nV1pV2 = v1 - v2;
//  std::cout << m1 * nV1pV2 << std::endl;
//  Vector3 M2nM1V2 = M1V1M2V2 - m1 * nV1pV2;
//  std::cout << M2nM1V2 << "m2 " << m2 << " m1 " << m1 << std::endl;
//  Vector3 V2;
//  if(m2 - m1)
//    Vector3 V2 = M2nM1V2 / (m2 - m1);
//  else
//    Vector3 V2 = M2nM1V2;
//  std::cout << V2 << std::endl;
//  v1 = V2 - nV1pV2;
//  v2 = V2;
//}


ColliderPlane::ColliderPlane()
	:
	scale{ 1.0f,1.0f,1.0f },
	centre{ 0.0f,0.0f,0.0f },
	extends{ 1.0f,0.0f,1.0f }
{

}

ColliderAABB::ColliderAABB()
	:
	scale{ 1.0f,1.0f,1.0f },
	centre{ 0.0f,0.0f,0.0f },
	extends{ 1.0f,1.0f,1.0f },
	rotationAffect_90{ false }
{

}

ColliderOBB::ColliderOBB()
	:
	scale{ 1.0f,1.0f,1.0f },
	centre{ 0.0f,0.0f,0.0f },
	extends{ 1.0f,1.0f,1.0f }
{

}

ColliderSphere::ColliderSphere()
	:
	radius{ 10.0f },
	centre{ 0.0f,0.0f,0.0f }
{

}

ColliderCapsule::ColliderCapsule()
	:radius{ 5.0f },
	height{ 10.0f },
	centre{ 0.0f,0.0f,0.0f }
{

}



void ColliderPlane::UpdateCollider()
{
	//CollisionCall();
	if (owner->disabled)
		return;

	if (rigidBody != nullptr)
	{
		centre = rigidBody->futurePosition + offset;
	}
	else
	{
		centre = tranformObject->GetGlobalPosition() + offset;
	}

	extends = tranformObject->ModelMax;
	Vector3 localScaling = tranformObject->GetGlobalScale();
	Vector3 tmp{ localScaling.x * scale.x, 1.0f, localScaling.z * scale.z };
	if (tmp.x == 0.0f)
		tmp.x = 1.0f;
	if (tmp.z == 0.0f)
		tmp.z = 1.0f;
	extends.x *= tmp.x;
	extends.z *= tmp.z;



	faceNormals[0] = tranformObject->GetSideVec();
	faceNormals[1] = tranformObject->GetUpVec();
	faceNormals[2] = tranformObject->GetForwardVec();
	//std::cout << centre << std::endl;

}

void ColliderAABB::UpdateCollider()
{
	//CollisionCall();
	if (owner->disabled)
		return;

	if (rigidBody != nullptr)
	{
		centre = rigidBody->futurePosition + offset;
	}
	else
	{
		centre = tranformObject->GetGlobalPosition() + offset;

	}

	extends = tranformObject->ModelMax;
	Vector3 localScaling = tranformObject->GetGlobalScale();
	Vector3 tmp{ localScaling.x * scale.x, localScaling.y * scale.y, localScaling.z * scale.z };
	if (tmp.x == 0.0f)
		tmp.x = 1.0f;
	if (tmp.y == 0.0f)
		tmp.y = 1.0f;
	if (tmp.z == 0.0f)
		tmp.z = 1.0f;
	extends.x *= tmp.x;
	extends.y *= tmp.y;
	extends.z *= tmp.z;

	if (rotationAffect_90)
	{
		Vector3 ref = tranformObject->GetRotation();

		while (ref.x < 0)
			ref.x += 360.0f;

		int tmp2 = ((static_cast<int>(ref.x) + 45) / 90) % 2;

		if (tmp2)
		{
			std::swap(extends.y, extends.z);
		}

		while (ref.y < 0)
			ref.y += 360.0f;

		tmp2 = ((static_cast<int>(ref.y) + 45) / 90) % 2;

		if (tmp2)
		{
			std::swap(extends.x, extends.z);
		}


		while (ref.z < 0)
			ref.z += 360.0f;

		tmp2 = ((static_cast<int>(ref.z) + 45) / 90) % 2;

		if (tmp2)
		{
			std::swap(extends.x, extends.y);
		}
	}


	faceNormals[0] = Vector3{ 1.0f,0.0f,0.0f };
	faceNormals[1] = Vector3{ 0.0f,1.0f,0.0f };
	faceNormals[2] = Vector3{ 0.0f,0.0f,1.0f };

}

void ColliderOBB::UpdateCollider()
{
	//CollisionCall();
	if (owner->disabled)
		return;

	if (rigidBody != nullptr)
	{
		centre = rigidBody->futurePosition + offset;
	}
	else
	{
		centre = tranformObject->GetGlobalPosition() + offset;
	}

	extends = tranformObject->ModelMax;
	Vector3 tmp{ tranformObject->GetScale().x * scale.x, tranformObject->GetScale().y * scale.y, tranformObject->GetScale().z * scale.z };
	if (tmp.x == 0.0f)
		tmp.x = 1.0f;
	if (tmp.y == 0.0f)
		tmp.y = 1.0f;
	if (tmp.z == 0.0f)
		tmp.z = 1.0f;
	extends.x *= tmp.x;
	extends.y *= tmp.y;
	extends.z *= tmp.z;

	faceNormals[0] = tranformObject->GetSideVec();
	faceNormals[1] = tranformObject->GetUpVec();
	faceNormals[2] = tranformObject->GetForwardVec();
}

void ColliderSphere::UpdateCollider()
{
	//CollisionCall();
	if (owner->disabled)
		return;

	if (rigidBody != nullptr)
	{
		centre = rigidBody->futurePosition + offset;
	}
	else
	{
		centre = tranformObject->GetGlobalPosition() + offset;
	}



}

void ColliderCapsule::UpdateCollider()
{
	//CollisionCall();
	if (owner->disabled)
		return;

	if (rigidBody != nullptr)
	{
		centre = rigidBody->futurePosition + offset;
	}
	else
	{
		centre = tranformObject->GetGlobalPosition() + offset;
	}

}
template<typename T1, typename T2>
void AddCollisionData(T1* lhs, T2* rhs)
{
	rhs->newCollisionData.push_back(lhs);
	lhs->newCollisionData.push_back(rhs);
	rhs->proxies[lhs->owner] = lhs->owner->GetProxy();
	lhs->proxies[rhs->owner] = rhs->owner->GetProxy();
}

bool ColliderPlane::CheckCollision(ColliderPlane* rhs)
{

	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	if (rigidBody && rigidBody->velocity * rhs->faceNormals[1] > 0)
		return false;

	if (rhs->rigidBody && rhs->rigidBody->velocity * faceNormals[1] > 0)
		return false;


	{
		Vector3 nearPoint;
		Vector3 farPoint;
		Vector3& yLength = rhs->faceNormals[1];
		Vector3& rhsCen = rhs->centre;
		Vector3& faceX = faceNormals[0];
		Vector3& faceZ = faceNormals[2];
		Vector3 tmp;
		Vector3 tmpDist;
		float farLength = -FLT_MAX;
		float nearLength = FLT_MAX;

		float tmpLength;
		tmp = centre + faceX * extends.x + faceZ * extends.z;
		tmpDist = rhsCen - tmp;
		tmpLength = yLength * tmpDist.Normalize();

		if (tmpLength > farLength)
		{
			farLength = tmpLength;
			farPoint = tmp;
		}

		if (tmpLength < nearLength)
		{
			nearLength = tmpLength;
			nearPoint = tmp;
		}

		tmp = centre - faceX * extends.x + faceZ * extends.z;
		tmpDist = rhsCen - tmp;
		tmpLength = yLength * tmpDist.Normalize();

		if (tmpLength > farLength)
		{
			farLength = tmpLength;
			farPoint = tmp;
		}

		if (tmpLength < nearLength)
		{
			nearLength = tmpLength;
			nearPoint = tmp;
		}

		tmp = centre + faceX * extends.x - faceZ * extends.z;
		tmpDist = rhsCen - tmp;
		tmpLength = yLength * tmpDist.Normalize();

		if (tmpLength > farLength)
		{
			farLength = tmpLength;
			farPoint = tmp;
		}

		if (tmpLength < nearLength)
		{
			nearLength = tmpLength;
			nearPoint = tmp;
		}

		tmp = centre - faceX * extends.x - faceZ * extends.z;
		tmpDist = rhsCen - tmp;
		tmpLength = yLength * tmpDist.Normalize();

		if (tmpLength > farLength)
		{
			farLength = tmpLength;
			farPoint = tmp;
		}

		if (tmpLength < nearLength)
		{
			nearLength = tmpLength;
			nearPoint = tmp;
		}

		if (nearLength > 0 || farLength < 0)
			return false;
	}

	{
		Vector3 nearPoint;
		Vector3 farPoint;
		Vector3& yLength = faceNormals[1];
		Vector3& rhsCen = centre;
		Vector3& faceX = rhs->faceNormals[0];
		Vector3& faceZ = rhs->faceNormals[2];
		Vector3 tmp;
		Vector3 tmpDist;
		float farLength = -FLT_MAX;
		float nearLength = FLT_MAX;

		float tmpLength;
		tmp = rhs->centre + faceX * extends.x + faceZ * extends.z;
		tmpDist = rhsCen - tmp;
		tmpLength = yLength * tmpDist.Normalize();

		if (tmpLength > farLength)
		{
			farLength = tmpLength;
			farPoint = tmp;
		}

		if (tmpLength < nearLength)
		{
			nearLength = tmpLength;
			nearPoint = tmp;
		}

		tmp = rhs->centre - faceX * extends.x + faceZ * extends.z;
		tmpDist = rhsCen - tmp;
		tmpLength = yLength * tmpDist.Normalize();

		if (tmpLength > farLength)
		{
			farLength = tmpLength;
			farPoint = tmp;
		}

		if (tmpLength < nearLength)
		{
			nearLength = tmpLength;
			nearPoint = tmp;
		}

		tmp = rhs->centre + faceX * extends.x - faceZ * extends.z;
		tmpDist = rhsCen - tmp;
		tmpLength = yLength * tmpDist.Normalize();

		if (tmpLength > farLength)
		{
			farLength = tmpLength;
			farPoint = tmp;
		}

		if (tmpLength < nearLength)
		{
			nearLength = tmpLength;
			nearPoint = tmp;
		}

		tmp = rhs->centre - faceX * extends.x - faceZ * extends.z;
		tmpDist = rhsCen - tmp;
		tmpLength = yLength * tmpDist.Normalize();

		if (tmpLength > farLength)
		{
			farLength = tmpLength;
			farPoint = tmp;
		}

		if (tmpLength < nearLength)
		{
			nearLength = tmpLength;
			nearPoint = tmp;
		}

		if (nearLength > 0 || farLength < 0)
			return false;
	}

	AddCollisionData(this, rhs);
	return true;
}


bool ColliderPlane::CheckCollision(ColliderAABB* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	if (rhs->rigidBody && rhs->rigidBody->velocity * faceNormals[1] > 0)
		return false;

	/*
	Vector3 MinPlane{ FLT_MAX,FLT_MAX,FLT_MAX };
	Vector3 MaxPlane{ -MinPlane };

	{
	Vector3 tmp = centre + faceNormals[0] * extends.x + faceNormals[2] * extends.z;
	if (MinPlane.x > tmp.x)
	MinPlane.x = tmp.x;
	if (MinPlane.y > tmp.y)
	MinPlane.y = tmp.y;
	if (MinPlane.z > tmp.z)
	MinPlane.z = tmp.z;

	if (MaxPlane.x < tmp.x)
	MaxPlane.x = tmp.x;
	if (MaxPlane.y < tmp.y)
	MaxPlane.y = tmp.y;
	if (MaxPlane.z < tmp.z)
	MaxPlane.z = tmp.z;
	}

	{
	Vector3 tmp = centre + faceNormals[0] * extends.x - faceNormals[2] * extends.z;
	if (MinPlane.x > tmp.x)
	MinPlane.x = tmp.x;
	if (MinPlane.y > tmp.y)
	MinPlane.y = tmp.y;
	if (MinPlane.z > tmp.z)
	MinPlane.z = tmp.z;

	if (MaxPlane.x < tmp.x)
	MaxPlane.x = tmp.x;
	if (MaxPlane.y < tmp.y)
	MaxPlane.y = tmp.y;
	if (MaxPlane.z < tmp.z)
	MaxPlane.z = tmp.z;
	}

	{
	Vector3 tmp = centre - faceNormals[0] * extends.x + faceNormals[2] * extends.z;
	if (MinPlane.x > tmp.x)
	MinPlane.x = tmp.x;
	if (MinPlane.y > tmp.y)
	MinPlane.y = tmp.y;
	if (MinPlane.z > tmp.z)
	MinPlane.z = tmp.z;

	if (MaxPlane.x < tmp.x)
	MaxPlane.x = tmp.x;
	if (MaxPlane.y < tmp.y)
	MaxPlane.y = tmp.y;
	if (MaxPlane.z < tmp.z)
	MaxPlane.z = tmp.z;
	}

	{
	Vector3 tmp = centre - faceNormals[0] * extends.x - faceNormals[2] * extends.z;
	if (MinPlane.x > tmp.x)
	MinPlane.x = tmp.x;
	if (MinPlane.y > tmp.y)
	MinPlane.y = tmp.y;
	if (MinPlane.z > tmp.z)
	MinPlane.z = tmp.z;

	if (MaxPlane.x < tmp.x)
	MaxPlane.x = tmp.x;
	if (MaxPlane.y < tmp.y)
	MaxPlane.y = tmp.y;
	if (MaxPlane.z < tmp.z)
	MaxPlane.z = tmp.z;
	}



	Vector3 MinPlaneAABB{ rhs->centre.x - rhs->extends.x,rhs->centre.y - rhs->extends.y,rhs->centre.z - rhs->extends.z };
	Vector3 MaxPlaneAABB{ rhs->centre.x + rhs->extends.x,rhs->centre.y + rhs->extends.y,rhs->centre.z + rhs->extends.z };

	if (MinPlaneAABB.x > MaxPlane.x)
	return false;

	if (MinPlaneAABB.y > MaxPlane.y)
	return false;

	if (MinPlaneAABB.z > MaxPlane.z)
	return false;

	if (MaxPlaneAABB.x < MinPlane.x)
	return false;

	if (MaxPlaneAABB.y < MinPlane.y)
	return false;

	if (MaxPlaneAABB.z < MinPlane.z)
	return false;


	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{
	// get the furthest behind aabb point
	Vector3 furthest;
	Vector3 closest;

	if (faceNormals[1].y < 0)
	{
	furthest.y = MinPlaneAABB.y;
	closest.y = MaxPlaneAABB.y;
	}
	else
	{
	furthest.y = MaxPlaneAABB.y;
	closest.y = MinPlaneAABB.y;
	}

	if (faceNormals[1].x < 0)
	{
	furthest.x = MinPlaneAABB.x;
	closest.x = MaxPlaneAABB.x;
	}
	else
	{
	furthest.x = MaxPlaneAABB.x;
	closest.x = MinPlaneAABB.x;
	}

	if (faceNormals[1].z < 0)
	{
	furthest.z = MinPlaneAABB.z;
	closest.z = MaxPlaneAABB.z;
	}
	else
	{
	furthest.z = MaxPlaneAABB.z;
	closest.y = MinPlaneAABB.y;
	}

	if ((closest-centre) * faceNormals[1] > 0 || (furthest - centre) * faceNormals[1] < 0)
	return false;


	Vector3 cenToFurthest = closest - centre;

	Vector3 closestPoint = centre;


	if (faceNormals[0] * cenToFurthest < extends.x)
	{
	closestPoint += (faceNormals[0] * cenToFurthest) * faceNormals[0];
	}
	else
	{
	closestPoint += faceNormals[0] * extends.x;
	}

	if (faceNormals[2] * cenToFurthest < extends.z)
	{
	closestPoint += (faceNormals[2] * cenToFurthest) * faceNormals[2];
	}
	else
	{
	closestPoint += faceNormals[2] * extends.z;
	}

	float distCheck = FLT_MAX;
	Vector3 displacementVec{0,0,0};
	//Vector3 extendVec{ 0,0,0 };
	if (closestPoint.x != closest.x)
	{
	float test = closestPoint.x - closest.x;
	if (std::fabsf(test) < distCheck)
	{
	distCheck = test;
	displacementVec = Vector3{ test, 0 , 0 };
	//Vector3 tmp = displacementVec;

	//extendVec = Vector3{ tmp.Normalize()*Vector3{1.0f,0,0}*Vector3{1.0f,0,0} *extends.x};
	}
	}


	if (closestPoint.y != closest.y)
	{
	float test = closestPoint.y - closest.y;
	if (std::fabsf(test) < distCheck)
	{
	distCheck = test;
	displacementVec = Vector3{ 0, test , 0 };
	//Vector3 tmp = displacementVec;

	//extendVec = Vector3{ tmp.Normalize()*Vector3{ 0,1.0f,0 }*Vector3{ 0,1.0f,0 } *extends.y};

	}
	}

	if (closestPoint.z != closest.z)
	{
	float test = closestPoint.z - closest.z;
	if (std::fabsf(test) < distCheck)
	{
	distCheck = test;
	displacementVec = Vector3{ 0, 0 , test };
	//Vector3 tmp = displacementVec;

	//extendVec = Vector3{ tmp.Normalize()*Vector3{ 0,0,1.0f }*Vector3{ 0,0,1.0f } *extends.z };
	}
	}

	if (rigidBody == nullptr)
	{
	rhs->rigidBody->SetCollisionData(displacementVec + rhs->centre - rhs->offset, rhs->faceNormals[1]);
	}
	else if (rhs->rigidBody == nullptr)
	{
	rigidBody->SetCollisionData(centre -displacementVec - offset, displacementVec);
	}
	else
	{
	rhs->rigidBody->SetCollisionData(displacementVec/2.0f + rhs->centre + rhs->offset, rhs->faceNormals[1]);
	rigidBody->SetCollisionData(centre -displacementVec / 2.0f - offset, displacementVec);
	}
	}
	*/

	ColliderAABB& box = *rhs;
	Vector3 boxCen = box.centre;
	char normalBitCheck = 0;
	float minDist = 0;
	Vector3 minIntersectEdge;
	float dirDist = FLT_MAX;

	//! - - - - - - - - - - - - - - - - - - - - - - - - - - - - 1
	Vector3 curInterSectEdge{ boxCen.x + box.extends.x,boxCen.y + box.extends.y,boxCen.z + box.extends.z };
	Vector3 tmpVec = curInterSectEdge - centre;
	float   tmpDist = tmpVec * faceNormals[1];
	float   curDist = tmpVec.Length();

	if (tmpDist <= 0)
	{
		normalBitCheck |= 0x02;
		if (tmpDist < minDist)
		{
			tmpDist = minDist;
			minIntersectEdge = curInterSectEdge;
			dirDist = curDist;
		}
		else if (tmpDist == minDist && curDist < dirDist)
			minIntersectEdge = curInterSectEdge;
	}
	if (tmpDist >= 0)
		normalBitCheck |= 0x01;


	//! - - - - - - - - - - - - - - - - - - - - - - - - - - - - 2
	curInterSectEdge = Vector3{ boxCen.x - box.extends.x,boxCen.y + box.extends.y,boxCen.z + box.extends.z };
	tmpVec = curInterSectEdge - centre;
	tmpDist = tmpVec * faceNormals[1];
	curDist = tmpVec.Length();

	if (tmpDist <= 0)
	{
		normalBitCheck |= 0x02;
		if (tmpDist < minDist)
		{
			tmpDist = minDist;
			minIntersectEdge = curInterSectEdge;
			dirDist = curDist;
		}
		else if (tmpDist == minDist && curDist < dirDist)
			minIntersectEdge = curInterSectEdge;
	}
	if (tmpDist >= 0)
		normalBitCheck |= 0x01;

	//! - - - - - - - - - - - - - - - - - - - - - - - - - - - - 3
	curInterSectEdge = Vector3{ boxCen.x + box.extends.x,boxCen.y - box.extends.y,boxCen.z + box.extends.z };
	tmpVec = curInterSectEdge - centre;
	tmpDist = tmpVec * faceNormals[1];
	curDist = tmpVec.Length();

	if (tmpDist <= 0)
	{
		normalBitCheck |= 0x02;
		if (tmpDist < minDist)
		{
			tmpDist = minDist;
			minIntersectEdge = curInterSectEdge;
			dirDist = curDist;
		}
		else if (tmpDist == minDist && curDist < dirDist)
			minIntersectEdge = curInterSectEdge;
	}
	if (tmpDist >= 0)
		normalBitCheck |= 0x01;


	//! - - - - - - - - - - - - - - - - - - - - - - - - - - - - 4
	curInterSectEdge = Vector3{ boxCen.x - box.extends.x,boxCen.y - box.extends.y,boxCen.z + box.extends.z };
	tmpVec = curInterSectEdge - centre;
	tmpDist = tmpVec * faceNormals[1];
	curDist = tmpVec.Length();

	if (tmpDist <= 0)
	{
		normalBitCheck |= 0x02;
		if (tmpDist < minDist)
		{
			tmpDist = minDist;
			minIntersectEdge = curInterSectEdge;
			dirDist = curDist;
		}
		else if (tmpDist == minDist && curDist < dirDist)
			minIntersectEdge = curInterSectEdge;
	}
	if (tmpDist >= 0)
		normalBitCheck |= 0x01;

	//! - - - - - - - - - - - - - - - - - - - - - - - - - - - - 5
	curInterSectEdge = Vector3{ boxCen.x + box.extends.x,boxCen.y + box.extends.y,boxCen.z - box.extends.z };
	tmpVec = curInterSectEdge - centre;
	tmpDist = tmpVec * faceNormals[1];
	curDist = tmpVec.Length();

	if (tmpDist <= 0)
	{
		normalBitCheck |= 0x02;
		if (tmpDist < minDist)
		{
			tmpDist = minDist;
			minIntersectEdge = curInterSectEdge;
			dirDist = curDist;
		}
		else if (tmpDist == minDist && curDist < dirDist)
			minIntersectEdge = curInterSectEdge;
	}
	if (tmpDist >= 0)
		normalBitCheck |= 0x01;

	//! - - - - - - - - - - - - - - - - - - - - - - - - - - - - 6
	curInterSectEdge = Vector3{ boxCen.x - box.extends.x,boxCen.y + box.extends.y,boxCen.z - box.extends.z };
	tmpVec = curInterSectEdge - centre;
	tmpDist = tmpVec * faceNormals[1];
	curDist = tmpVec.Length();

	if (tmpDist <= 0)
	{
		normalBitCheck |= 0x02;
		if (tmpDist < minDist)
		{
			tmpDist = minDist;
			minIntersectEdge = curInterSectEdge;
			dirDist = curDist;
		}
		else if (tmpDist == minDist && curDist < dirDist)
			minIntersectEdge = curInterSectEdge;
	}
	if (tmpDist >= 0)
		normalBitCheck |= 0x01;

	//! - - - - - - - - - - - - - - - - - - - - - - - - - - - - 7
	curInterSectEdge = Vector3{ boxCen.x + box.extends.x,boxCen.y - box.extends.y,boxCen.z - box.extends.z };
	tmpVec = curInterSectEdge - centre;
	tmpDist = tmpVec * faceNormals[1];
	curDist = tmpVec.Length();

	if (tmpDist <= 0)
	{
		normalBitCheck |= 0x02;
		if (tmpDist < minDist)
		{
			tmpDist = minDist;
			minIntersectEdge = curInterSectEdge;
			dirDist = curDist;
		}
		else if (tmpDist == minDist && curDist < dirDist)
			minIntersectEdge = curInterSectEdge;
	}
	if (tmpDist >= 0)
		normalBitCheck |= 0x01;

	//! - - - - - - - - - - - - - - - - - - - - - - - - - - - - 8
	curInterSectEdge = Vector3{ boxCen.x - box.extends.x,boxCen.y - box.extends.y,boxCen.z - box.extends.z };
	tmpVec = curInterSectEdge - centre;
	tmpDist = tmpVec * faceNormals[1];
	curDist = tmpVec.Length();

	if (tmpDist <= 0)
	{
		normalBitCheck |= 0x02;
		if (tmpDist < minDist)
		{
			tmpDist = minDist;
			minIntersectEdge = curInterSectEdge;
			dirDist = curDist;
		}
		else if (tmpDist == minDist && curDist < dirDist)
			minIntersectEdge = curInterSectEdge;
	}
	if (tmpDist >= 0)
		normalBitCheck |= 0x01;

	if (!(normalBitCheck & 0x01 && normalBitCheck & 0x02))
		return false;

	/// get the closest plane point
	tmpVec = minIntersectEdge - centre;
	bool edge = false;
	float shift = tmpVec * faceNormals[0];

	if (shift > extends.x)
	{
		shift = extends.x;
		edge = true;
	}
	else if (shift < -extends.x)
	{
		shift = -extends.x;
		edge = true;
	}

	Vector3 closestPoint = centre + shift * faceNormals[0];

	shift = tmpVec * faceNormals[2];

	if (shift > extends.z)
	{
		shift = extends.z;
		edge = true;
	}
	else if (shift < -extends.z)
	{
		shift = -extends.z;
		edge = true;
	}

	closestPoint += shift * faceNormals[2];

	Vector3 maxLoc{ boxCen.x + box.extends.x + marginOfError,boxCen.y + box.extends.y + marginOfError,boxCen.z + box.extends.z + marginOfError };

	if (closestPoint.x > maxLoc.x || closestPoint.y > maxLoc.y || closestPoint.z > maxLoc.z)
		return false;
	maxLoc = Vector3{ boxCen.x - box.extends.x - marginOfError,boxCen.y - box.extends.y - marginOfError,boxCen.z - box.extends.z - marginOfError };

	if (closestPoint.x < maxLoc.x || closestPoint.y < maxLoc.y || closestPoint.z < maxLoc.z)
		return false;

	Vector3 tmpCen = closestPoint - boxCen;

	Vector3 edgeDist{ box.extends.x - std::fabsf(tmpCen.x),box.extends.y - std::fabsf(tmpCen.y) ,box.extends.z - std::fabsf(tmpCen.z) };

	//std::cout << " ? " << std::endl;
	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

		if (rigidBody == nullptr || (rigidBody->isKinematic && rhs->rigidBody))
		{
			if (!rhs->rigidBody->isKinematic)
			{
				//std::cout << "box" << std::endl;
				if (edgeDist.y >= edgeDist.x && edgeDist.y >= edgeDist.z)
				{
					if (tmpCen.y < 0)
					{
						//std::cout << "y" << faceNormals[1] << std::endl;
						Vector3 resolve{ 0, edgeDist.y, 0 };

						if (edge)
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, rhs->faceNormals[1]);
						else
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					}
					else
					{
						//std::cout << "here" << std::endl;
						Vector3 resolve{ 0, -edgeDist.y, 0 };
						if (edge)
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, -rhs->faceNormals[1]);
						else
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					}
				}
				else if (edgeDist.z >= edgeDist.y && edgeDist.z >= edgeDist.x)
				{
					//std::cout << "no Z" << std::endl;
					if (tmpCen.z < 0)
					{
						Vector3 resolve{ 0, 0, edgeDist.z };

						if (edge)
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, rhs->faceNormals[2]);
						else
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					}
					else
					{
						Vector3 resolve{ 0, 0, -edgeDist.z };
						if (edge)
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, -rhs->faceNormals[2]);
						else
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					}
				}
				else
				{
					//std::cout << "no X" << std::endl;
					if (tmpCen.x < 0)
					{
						Vector3 resolve{ edgeDist.x,0, 0 };

						if (edge)
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, rhs->faceNormals[0]);
						else
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					}
					else
					{
						Vector3 resolve{ -edgeDist.x,0, 0 };
						if (edge)
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, -rhs->faceNormals[0]);
						else
							rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					}
				}
			}
		}
		else if (rhs->rigidBody == nullptr || (rhs->rigidBody->isKinematic && rigidBody))
		{
			if (!rigidBody->isKinematic)
			{
				if (edgeDist.y >= edgeDist.x && edgeDist.y >= edgeDist.z)
				{
					if (tmpCen.y < 0)
					{
						Vector3 resolve{ 0, edgeDist.y, 0 };

						rigidBody->SetCollisionData(centre - offset - resolve, -rhs->faceNormals[1]);
					}
					else
					{
						Vector3 resolve{ 0, -edgeDist.y, 0 };
						rigidBody->SetCollisionData(centre - offset - resolve, rhs->faceNormals[1]);
					}
				}
				else if (edgeDist.z >= edgeDist.y && edgeDist.z >= edgeDist.x)
				{
					if (tmpCen.z < 0)
					{
						Vector3 resolve{ 0, 0, edgeDist.z };

						rigidBody->SetCollisionData(centre - offset - resolve, -rhs->faceNormals[2]);
					}
					else
					{
						Vector3 resolve{ 0, 0, -edgeDist.z };
						rigidBody->SetCollisionData(centre - offset - resolve, rhs->faceNormals[2]);
					}
				}
				else
				{
					if (tmpCen.x < 0)
					{
						Vector3 resolve{ edgeDist.x,0, 0 };

						rigidBody->SetCollisionData(centre - offset - resolve, -rhs->faceNormals[0]);
					}
					else
					{
						Vector3 resolve{ -edgeDist.x,0, 0 };
						rigidBody->SetCollisionData(centre - offset - resolve, rhs->faceNormals[0]);
					}
				}
			}
		}
		else
		{
			if (edgeDist.y >= edgeDist.x && edgeDist.y >= edgeDist.z)
			{
				if (tmpCen.y < 0)
				{
					Vector3 resolve{ 0, edgeDist.y / 2.0f, 0 };

					if (edge)
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, rhs->faceNormals[1]);
					else
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					rigidBody->SetCollisionData(centre - offset - resolve, -rhs->faceNormals[1]);
				}
				else
				{
					Vector3 resolve{ 0, -edgeDist.y / 2.0f, 0 };
					if (edge)
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, -rhs->faceNormals[1]);
					else
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					rigidBody->SetCollisionData(centre - offset - resolve, rhs->faceNormals[1]);
				}
			}
			else if (edgeDist.z >= edgeDist.y && edgeDist.z >= edgeDist.x)
			{
				if (tmpCen.z < 0)
				{
					Vector3 resolve{ 0, 0, edgeDist.z / 2.0f };

					if (edge)
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, rhs->faceNormals[2]);
					else
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					rigidBody->SetCollisionData(centre - offset - resolve, -rhs->faceNormals[2]);
				}
				else
				{
					Vector3 resolve{ 0, 0, -edgeDist.z / 2.0f };
					if (edge)
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, -rhs->faceNormals[2]);
					else
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					rigidBody->SetCollisionData(centre - offset - resolve, rhs->faceNormals[2]);
				}
			}
			else
			{
				if (tmpCen.x < 0)
				{
					Vector3 resolve{ edgeDist.x / 2.0f,0, 0 };

					if (edge)
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, rhs->faceNormals[0]);
					else
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					rigidBody->SetCollisionData(centre - offset - resolve, -rhs->faceNormals[0]);
				}
				else
				{
					Vector3 resolve{ -edgeDist.x / 2.0f,0, 0 };
					if (edge)
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, -rhs->faceNormals[0]);
					else
						rhs->rigidBody->SetCollisionData(boxCen - rhs->offset + resolve, faceNormals[1]);
					rigidBody->SetCollisionData(centre - offset - resolve, rhs->faceNormals[0]);
				}
			}
			//rhs->rigidBody->SetCollisionData(newPos - rhs->offset, tmpVecY);
		}
	}

	AddCollisionData(this, rhs);
	return true;

}

bool ColliderPlane::CheckCollision(ColliderOBB* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

	}
	return false;

}

bool ColliderPlane::CheckCollision(ColliderSphere* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;

	Vector3 vec3 = rhs->centre - centre;
	if (rhs->rigidBody && rhs->rigidBody->velocity * faceNormals[1] > 0)
		return false;

	Vector3 tmpVecY = faceNormals[1];
	float dist = vec3 * tmpVecY.Normalize();


	if (std::fabsf(dist) > rhs->radius)
		return false;

	//Get closest point in x and z
	Vector3 tmpVecX = faceNormals[0];
	tmpVecX.Normalize();

	bool edge = false;

	float xDist = tmpVecX * vec3;
	if (xDist > extends.x)
	{
		edge = true;
		xDist = extends.x;
	}
	else if (xDist < -extends.x)
	{
		edge = true;
		xDist = -extends.x;
	}



	Vector3 tmpVecZ = faceNormals[2];
	tmpVecZ.Normalize();

	float zDist = tmpVecZ * vec3;
	if (zDist > extends.z)
	{
		edge = true;
		zDist = extends.z;
	}
	else if (zDist < -extends.z)
	{
		edge = true;
		zDist = -extends.z;
	}
	Vector3 closestPoint = centre + tmpVecX * xDist + tmpVecZ * zDist;
	vec3 = rhs->centre - closestPoint;

	if (vec3.Length() > rhs->radius)
		return false;

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

		if (rigidBody == nullptr || (rigidBody->isKinematic && rhs->rigidBody))
		{
			if (!rhs->rigidBody->isKinematic)
			{
				if (edge)
				{
					Vector3 newPos{ closestPoint + vec3.Normalize() * rhs->radius };
					rhs->rigidBody->SetCollisionData(newPos - rhs->offset, vec3);

				}
				else
				{
					Vector3 newPos{ closestPoint + tmpVecY * rhs->radius };
					rhs->rigidBody->SetCollisionData(newPos - rhs->offset, tmpVecY);
				}
			}
		}
		else if (rhs->rigidBody == nullptr || (rhs->rigidBody->isKinematic && rigidBody))
		{
			if (!rigidBody->isKinematic)
			{
				if (edge)
				{
					Vector3 newPos{ rhs->centre - vec3.Normalize() * rhs->radius };
					rigidBody->SetCollisionData(newPos - offset, -vec3);
				}
				else
				{
					Vector3 newPos{ rhs->centre - tmpVecY * rhs->radius };
					rigidBody->SetCollisionData(newPos - offset, -tmpVecY);
				}
			}
		}
		else
		{

			if (edge)
			{

				Vector3 newPos{ rhs->centre - vec3.Normalize() * (rhs->radius / 2.0f) };
				rigidBody->SetCollisionData(newPos - offset, -vec3);

				newPos = closestPoint + vec3.Normalize() * (rhs->radius / 2.0f);
				rhs->rigidBody->SetCollisionData(newPos - rhs->offset, vec3);
			}
			else
			{
				Vector3 newPos{ rhs->centre - tmpVecY * (rhs->radius / 2.0f) };
				rigidBody->SetCollisionData(newPos - offset, -tmpVecY);

				newPos = closestPoint + tmpVecY * (rhs->radius / 2.0f);
				rhs->rigidBody->SetCollisionData(newPos - rhs->offset, tmpVecY);
			}
		}
	}
	AddCollisionData(this, rhs);
	return true;

}


bool ColliderPlane::CheckCollision(ColliderCapsule* rhs)
{

	return rhs->CheckCollision(this);
}




bool ColliderAABB::CheckCollision(ColliderPlane* rhs)
{

	return rhs->CheckCollision(this);
}

/*

switch (data) {
case negForwardCol:
break;
case forwardCol:
break;
case negSideCol:
break;
case sideCol:
break;
case upCol:
break;
case negUpCol:
break;
}

*/



bool ColliderAABB::CheckCollision(ColliderAABB* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;

	Vector3 dist = rhs->centre - centre;
	Vector3 distExtend = extends + rhs->extends;
	Vector3 absDist{ distExtend.x - std::fabsf(dist.x), distExtend.y - std::fabsf(dist.y) ,distExtend.z - std::fabsf(dist.z) };

	if (absDist.x <= 0 || absDist.y <= 0 || absDist.z <= 0)
		return false;



	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{
		if (rigidBody == nullptr || (rigidBody->isKinematic && rhs->rigidBody))
		{
			if (!rhs->rigidBody->isKinematic)
			{

				if (absDist.y <= absDist.x && absDist.y <= absDist.z)
				{
					Vector3 off;
					float value{ absDist.y };
					if (dist.y > 0)
						off = Vector3{ 0, value  ,0 };
					else
						off = Vector3{ 0, -value ,0 };
					Vector3 tmp = off;
					rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + off, tmp.Normalize());
				}
				else if (absDist.z <= absDist.x && absDist.z <= absDist.y)
				{
					Vector3 off;
					float value{ absDist.z };
					if (dist.z > 0)
						off = Vector3{ 0, 0  ,value };
					else
						off = Vector3{ 0, 0 , -value };
					Vector3 tmp = off;

					rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + off, tmp.Normalize());
				}
				else
				{
					Vector3 off;
					float value{ absDist.x };
					if (dist.x > 0)
						off = Vector3{ value, 0  ,0 };
					else
						off = Vector3{ -value,0 ,0 };
					Vector3 tmp = off;

					rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + off, tmp.Normalize());
				}
			}

			//rhs->rigidBody->SetCollisionData(newPosition - rhs->offset, Vector3{ 0, 1.0f ,0 });

		}
		else if (rhs->rigidBody == nullptr || (rhs->rigidBody->isKinematic && rigidBody))
		{
			if (!rigidBody->isKinematic)
			{
				if (absDist.y <= absDist.x && absDist.y <= absDist.z)
				{
					Vector3 off;
					float value{ absDist.y };
					if (dist.y > 0)
						off = Vector3{ 0, -value  ,0 };
					else
						off = Vector3{ 0, value ,0 };
					Vector3 tmp = off;

					rigidBody->SetCollisionData(centre - offset + off, tmp.Normalize());
				}
				else if (absDist.z <= absDist.x && absDist.z <= absDist.y)
				{
					Vector3 off;
					float value{ absDist.z };
					if (dist.z > 0)
						off = Vector3{ 0, 0 ,-value };
					else
						off = Vector3{ 0, 0 ,value };
					Vector3 tmp = off;

					rigidBody->SetCollisionData(centre - offset + off, tmp.Normalize());
				}
				else
				{
					Vector3 off;
					float value{ absDist.x };
					if (dist.x > 0)
						off = Vector3{ -value, 0  ,0 };
					else
						off = Vector3{ value,0 ,0 };
					Vector3 tmp = off;

					rigidBody->SetCollisionData(centre - offset + off, tmp.Normalize());
				}
				//rigidBody->SetCollisionData(newPosition - offset, Vector3{ 0, -1.0f ,0 });
			}
		}
		else
		{
			if (absDist.y <= absDist.x && absDist.y <= absDist.z)
			{
				Vector3 off;
				float value{ absDist.y };
				if (dist.y > 0)
					off = Vector3{ 0, value / 2.0f  ,0 };
				else
					off = Vector3{ 0, -value / 2.0f ,0 };
				Vector3 tmp = off;

				rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + off, tmp.Normalize());
				rigidBody->SetCollisionData(centre - offset - off, -tmp.Normalize());
			}
			else if (absDist.z <= absDist.x && absDist.z <= absDist.y)
			{
				Vector3 off;
				float value{ absDist.z };
				if (dist.z > 0)
					off = Vector3{ 0, 0  ,value / 2.0f };
				else
					off = Vector3{ 0, 0,-value / 2.0f };
				Vector3 tmp = off;

				rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + off, tmp.Normalize());
				rigidBody->SetCollisionData(centre - offset - off, -tmp.Normalize());
			}
			else
			{
				Vector3 off;
				float value{ absDist.x };
				if (dist.x > 0)
					off = Vector3{ value / 2.0f, 0  ,0 };
				else
					off = Vector3{ -value / 2.0f,0 ,0 };
				Vector3 tmp = off;

				rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + off, tmp.Normalize());
				rigidBody->SetCollisionData(centre - offset - off, -tmp.Normalize());
			}
			//rhs->rigidBody->SetCollisionData(newPosition - rhs->offset, Vector3{ 0,1.0f,0 });
		}

	}

	AddCollisionData(this, rhs);
	return true;
}

bool ColliderAABB::CheckCollision(ColliderOBB* rhs)
{
	// it's gonna be the same for everything else for all OBB and OBB+AABB

	/*
	use SAT for default axis if AABB is included



	if not use the rotation matrix to create a new world then get all min and max x y z and perform SAT


	then proceed to use the rotation matrix again to create another new world to perform SAT again




	*/

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

	}
	return false;
}


bool ColliderAABB::CheckCollision(ColliderSphere* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	// get the closest point
	ColliderSphere& sphere = *rhs;

	Vector3 vecDir = sphere.centre - centre;
	
	Vector3 extendCube{ extends.x + sphere.radius,extends.y + sphere.radius ,extends.z + sphere.radius };

	Vector3 shift{ extendCube.x - std::fabsf(vecDir.x), extendCube.y - std::fabsf(vecDir.y) ,extendCube.z - std::fabsf(vecDir.z) };

	if (shift.x <= 0 || shift.y <= 0 || shift.z <= 0)
		return false;

	//std::cout << shift << std::endl;

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{
		if (rigidBody == nullptr || (rigidBody->isKinematic && rhs->rigidBody))
		{
			if (!rhs->rigidBody->isKinematic)
			{

				if (shift.y <= shift.x && shift.y <= shift.z)
				{
					Vector3 cenShift{ 0,-shift.y,0 };
					if (vecDir.y <= 0)
						rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + cenShift, Vector3{ 0,-1,0 });
					else
						rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset - cenShift, Vector3{ 0,1,0 });
				}
				else if (shift.z <= shift.x && shift.z <= shift.y)
				{
					Vector3 cenShift{ 0,0 ,-shift.z };
					if (vecDir.z <= 0)
						rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + cenShift, Vector3{ 0,0,-1 });
					else
						rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset - cenShift, Vector3{ 0,0,1 });
				}
				else
				{
					Vector3 cenShift{ -shift.x, 0,0 };
					if (vecDir.x <= 0)
						rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + cenShift, Vector3{ -1,0,0 });
					else
						rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset - cenShift, Vector3{ 1,0,0 });
				}
			}
			//rhs->rigidBody->SetCollisionData(newPos - rhs->offset, newNorm);
		}
		else if (rhs->rigidBody == nullptr || (rhs->rigidBody->isKinematic && rigidBody))
		{
			if (!rigidBody->isKinematic)
			{
				vecDir.Normalize();
				if (shift.y <= shift.x && shift.y <= shift.z)
				{
					Vector3 cenShift{ 0,shift.y,0 };
					if (vecDir.y <= 0)
						rigidBody->SetCollisionData(centre - offset + cenShift, -vecDir);
					else
						rigidBody->SetCollisionData(centre - offset - cenShift, -vecDir);
				}
				else if (shift.z <= shift.x && shift.z <= shift.y)
				{
					Vector3 cenShift{ 0,0 ,shift.z };
					if (vecDir.z <= 0)
						rigidBody->SetCollisionData(centre - offset + cenShift, -vecDir);
					else
						rigidBody->SetCollisionData(centre - offset - cenShift, -vecDir);
				}
				else
				{
					Vector3 cenShift{ shift.x, 0,0 };
					if (vecDir.x <= 0)
						rigidBody->SetCollisionData(centre - offset + cenShift, -vecDir);
					else
						rigidBody->SetCollisionData(centre - offset - cenShift, -vecDir);
				}
			}
			//rigidBody->SetCollisionData(newPos - offset, newNorm);
		}
		else
		{
			if (shift.y <= shift.x && shift.y <= shift.z)
			{
				Vector3 cenShift{ 0,-shift.y/2.0f,0 };
				if (vecDir.y <= 0)
				{
					rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + cenShift, Vector3{ 0,-1,0 });
					rigidBody->SetCollisionData(centre - offset - cenShift, -vecDir);
				}
				else
				{
					rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset - cenShift, Vector3{ 0,1,0 });
					rigidBody->SetCollisionData(centre - offset + cenShift, -vecDir);
				}
			}
			else if (shift.z <= shift.x && shift.z <= shift.y)
			{
				Vector3 cenShift{ 0,0 ,-shift.z / 2.0f };
				if (vecDir.z <= 0)
				{
					rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + cenShift, Vector3{ 0,0,-1 });
					rigidBody->SetCollisionData(centre - offset - cenShift, -vecDir);
				}
				else
				{
					rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset - cenShift, Vector3{ 0,0,1 });
					rigidBody->SetCollisionData(centre - offset + cenShift, -vecDir);
				}
			}
			else
			{
				Vector3 cenShift{ -shift.x / 2.0f, 0,0 };
				if (vecDir.x <= 0)
				{
					rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset + cenShift, Vector3{ -1,0,0 });
					rigidBody->SetCollisionData(centre - offset - cenShift, -vecDir);
				}
				else
				{
					rhs->rigidBody->SetCollisionData(rhs->centre - rhs->offset - cenShift, Vector3{ 1,0,0 });
					rigidBody->SetCollisionData(centre - offset + cenShift, -vecDir);
				}
			}
		}
	}

	/*
	float dist;

	CollisionFace data = NoneCol;
	float lessDist = FLT_MIN;
	bool edgeTest = false;

	if (closestPoint.x > sphere.centre.x)
	{
		dist = closestPoint.x - sphere.centre.x;
		if (dist > extends.x + sphere.radius)
			return false;
		if (dist > extends.x)
		{
			closestPoint.x = centre.x - extends.x;
			edgeTest = true;
		}
		else
			closestPoint.x -= dist;

		if (dist - extends.x > lessDist)
		{
			lessDist = dist - extends.x;
			data = negSideCol;
		}
	}
	else if (closestPoint.x < sphere.centre.x)
	{
		dist = sphere.centre.x - closestPoint.x;
		if (dist > extends.x + sphere.radius)
			return false;
		if (dist > extends.x)
		{
			closestPoint.x = centre.x + extends.x;
			edgeTest = true;
		}
		else
			closestPoint.x += dist;

		if (dist - extends.x > lessDist)
		{
			lessDist = dist - extends.x;
			data = sideCol;
		}
	}

	if (closestPoint.z > sphere.centre.z)
	{
		dist = closestPoint.z - sphere.centre.z;
		if (dist > extends.z + sphere.radius)
			return false;
		if (dist > extends.z)
		{
			closestPoint.z = centre.z - extends.z;
			edgeTest = true;
		}
		else
			closestPoint.z -= dist;

		if (dist - extends.z > lessDist)
		{
			lessDist = dist - extends.z;
			data = negForwardCol;
		}
	}
	else if (closestPoint.z < sphere.centre.z)
	{
		dist = sphere.centre.z - closestPoint.z;
		if (dist > extends.z + sphere.radius)
			return false;
		if (dist > extends.z)
		{
			closestPoint.z = centre.z + extends.z;
			edgeTest = true;
		}
		else
			closestPoint.z += dist;

		if (dist - extends.z > lessDist)
		{
			lessDist = dist - extends.z;
			data = forwardCol;
		}
	}

	if (closestPoint.y > sphere.centre.y)
	{
		dist = closestPoint.y - sphere.centre.y;
		if (dist > extends.y + sphere.radius)
			return false;
		if (dist > extends.y)
		{
			closestPoint.y = centre.y - extends.y;
			edgeTest = true;
		}
		else
			closestPoint.y -= dist;

		if (dist - extends.y > lessDist)
		{
			lessDist = dist - extends.y;
			data = negUpCol;
		}
	}
	else if (closestPoint.y < sphere.centre.y)
	{
		dist = sphere.centre.y - closestPoint.y;
		if (dist > extends.y + sphere.radius)
			return false;
		if (dist > extends.y)
		{
			closestPoint.y = centre.y + extends.y;
			edgeTest = true;
		}
		else
			closestPoint.y += dist;

		if (dist - extends.y > lessDist)
		{
			lessDist = dist - extends.y;
			data = upCol;
		}
	}

	Vector3 distObjects{ sphere.centre - closestPoint };
	float distLength = distObjects.Length();

	if (distLength > sphere.radius)
		return false;


	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{
		if (rigidBody == nullptr)
		{
			if (edgeTest == false)
			{
				switch (data) {
				case negForwardCol:
					closestPoint.z = centre.z - extends.z;
					break;
				case forwardCol:
					closestPoint.z = centre.z + extends.z;
					break;
				case negSideCol:
					closestPoint.x = centre.x - extends.x;
					break;
				case sideCol:
					closestPoint.x = centre.x + extends.x;
					break;
				case upCol:
					closestPoint.y = centre.y + extends.y;
					break;
				case negUpCol:
					closestPoint.y = centre.y - extends.y;
					break;
				}

			}


			Vector3 newNorm = sphere.centre - closestPoint;
			if (newNorm != Vector3{ 0,0,0 })
				newNorm.Normalize();
			Vector3 newPos = closestPoint + newNorm * sphere.radius;
			rhs->rigidBody->SetCollisionData(newPos - rhs->offset, newNorm);

		}
		else if (rhs->rigidBody == nullptr)
		{
			if (edgeTest == false)
			{
				switch (data) {
				case negForwardCol:
					closestPoint.z = centre.z - extends.z;
					break;
				case forwardCol:
					closestPoint.z = centre.z + extends.z;
					break;
				case negSideCol:
					closestPoint.x = centre.x - extends.x;
					break;
				case sideCol:
					closestPoint.x = centre.x + extends.x;
					break;
				case upCol:
					closestPoint.y = centre.y + extends.y;
					break;
				case negUpCol:
					closestPoint.y = centre.y - extends.y;
					break;
				}
			}
			//Vector3 closeToCen = centre - closestPoint;
			Vector3 newNorm = closestPoint - sphere.centre;
			Vector3 newOffset = -newNorm;
			newNorm.Normalize();
			Vector3 newPos = centre + newOffset + newNorm * sphere.radius;

			rigidBody->SetCollisionData(newPos - offset, newNorm);
		}
		else
		{
			if (edgeTest == false)
			{
				switch (data) {
				case negForwardCol:
					closestPoint.z = centre.z - extends.z;
					break;
				case forwardCol:
					closestPoint.z = centre.z + extends.z;
					break;
				case negSideCol:
					closestPoint.x = centre.x - extends.x;
					break;
				case sideCol:
					closestPoint.x = centre.x + extends.x;
					break;
				case upCol:
					closestPoint.y = centre.y + extends.y;
					break;
				case negUpCol:
					closestPoint.y = centre.y - extends.y;
					break;
				}

			}


			Vector3 newNorm = sphere.centre - closestPoint;
			newNorm.Normalize();
			Vector3 newPos1 = closestPoint + newNorm * sphere.radius;

			rhs->rigidBody->SetCollisionData(newPos1 - rhs->offset, newNorm);
			newNorm = closestPoint - sphere.centre;
			Vector3 newOffset = -newNorm;
			newNorm.Normalize();
			Vector3 newPos2 = centre + newOffset + newNorm * sphere.radius;

			rigidBody->SetCollisionData(newPos2 - offset, newNorm);


		}
	}


	*/
	AddCollisionData(this, rhs);
	return true;
}

bool ColliderAABB::CheckCollision(ColliderCapsule* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	// get the closest point
	ColliderCapsule& sphere = *rhs;


	Vector3 centreOfCap = sphere.centre;
	Vector3 nearestPlanePt = centre;
	Vector3 x{ faceNormals[0] };
	Vector3 z{ faceNormals[2] };

	/// check for negative 0 (plane uprightTest)
	//char edgeCase{ 0 };
	float y_shift = nearestPlanePt.y - centreOfCap.y;

	if (y_shift > sphere.height)
	{
		y_shift = sphere.height;
	}
	if (y_shift < -sphere.height)
	{
		y_shift = -sphere.height;
	}

	centreOfCap.y += y_shift;

	y_shift = centreOfCap.y - nearestPlanePt.y;

	if (y_shift > extends.y)
	{
		y_shift = extends.y;
		//edgeCase &= 0x01;
	}
	if (y_shift < -extends.y)
	{
		y_shift = -extends.y;
		//edgeCase &= 0x02;
	}

	nearestPlanePt.y += y_shift;

	float shift = centreOfCap.x - nearestPlanePt.x;

	if (shift > extends.x)
	{
		shift = extends.x;
		//edgeCase &= 0x04;
	}
	if (shift < -extends.x)
	{
		shift = -extends.x;
		//edgeCase &= 0x08;
	}

	nearestPlanePt.x += shift;

	shift = centreOfCap.z - nearestPlanePt.z;

	if (shift > extends.z)
	{
		shift = extends.z;
		//edgeCase &= 0x10;
	}
	if (shift < -extends.z)
	{
		shift = -extends.z;
		//edgeCase &= 0x20;
	}

	nearestPlanePt.z += shift;


	Vector3 toCap = centreOfCap - nearestPlanePt;

	if (toCap.Length() > sphere.radius)
		return false;




	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{
		if (rigidBody == nullptr || (rigidBody->isKinematic && rhs->rigidBody))
		{
			if (!rhs->rigidBody->isKinematic)
			{

				//rhs->rigidBody->SetCollisionData(newPos2 - offset, newNorm);

				if (toCap != Vector3{ 0,0,0 })
				{
					Vector3 absVec{ std::fabsf(toCap.x),std::fabsf(toCap.y) ,std::fabsf(toCap.z) };
					if (absVec.y >= absVec.z && absVec.y >= absVec.x)
					{
						Vector3 off;
						float value{ sphere.radius - absVec.y };
						if (toCap.y > 0)
							off = Vector3{ 0, value  ,0 };
						else
							off = Vector3{ 0, -value ,0 };
						rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());

					}
					else if (absVec.z >= absVec.y && absVec.z >= absVec.x)
					{
						Vector3 off;
						float value{ sphere.radius - absVec.z };
						if (toCap.z > 0)
							off = Vector3{ 0,0, value };
						else
							off = Vector3{ 0,0, -value };
						rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());

					}
					else
					{
						Vector3 off;
						float value{ sphere.radius - absVec.x };
						if (toCap.x > 0)
							off = Vector3{ value ,0,0 };
						else
							off = Vector3{ -value,0,0 };
						rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());

					}

				}
				else
				{
					toCap = nearestPlanePt - centre;

					Vector3 absVec{ extends.x - std::fabsf(toCap.x), extends.y - std::fabsf(toCap.y) , extends.z - std::fabsf(toCap.z) };
					if (absVec.y <= absVec.z && absVec.y <= absVec.x)
					{
						Vector3 off;
						float value{ absVec.y };
						if (toCap.y > 0)
							off = Vector3{ 0, value + sphere.radius  ,0 };
						else
							off = Vector3{ 0, -value - sphere.radius ,0 };
						rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());

					}
					else if (absVec.z <= absVec.y && absVec.z <= absVec.x)
					{
						Vector3 off;
						float value{ absVec.z };
						if (toCap.z > 0)
							off = Vector3{ 0,0, value + sphere.radius };
						else
							off = Vector3{ 0,0, -value - sphere.radius };
						rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());

					}
					else
					{
						Vector3 off;
						float value{ absVec.x };
						if (toCap.x > 0)
							off = Vector3{ value + sphere.radius ,0,0 };
						else
							off = Vector3{ -value - sphere.radius,0,0 };
						rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());

					}


				}
			}
		}
		else if (rhs->rigidBody == nullptr || (rhs->rigidBody->isKinematic && rigidBody))
		{
			if (!rigidBody->isKinematic)
			{
				if (toCap != Vector3{ 0,0,0 })
				{
					Vector3 absVec{ std::fabsf(toCap.x),std::fabsf(toCap.y) ,std::fabsf(toCap.z) };
					if (absVec.y >= absVec.z && absVec.y >= absVec.x)
					{
						Vector3 off;
						float value{ sphere.radius - absVec.y };
						if (toCap.y > 0)
							off = Vector3{ 0, -value  ,0 };
						else
							off = Vector3{ 0, value ,0 };
						rigidBody->SetCollisionData(centre - offset + off, -toCap.Normalize());

					}
					else if (absVec.z >= absVec.y && absVec.z >= absVec.x)
					{
						Vector3 off;
						float value{ sphere.radius - absVec.z };
						if (toCap.z > 0)
							off = Vector3{ 0,0, -value };
						else
							off = Vector3{ 0,0, value };
						rigidBody->SetCollisionData(centre - offset + off, -toCap.Normalize());

					}
					else
					{
						Vector3 off;
						float value{ sphere.radius - absVec.x };
						if (toCap.x > 0)
							off = Vector3{ -value ,0,0 };
						else
							off = Vector3{ value,0,0 };
						rigidBody->SetCollisionData(centre - offset + off, -toCap.Normalize());

					}

				}
				else
				{
					toCap = nearestPlanePt - centre;

					Vector3 absVec{ extends.x - std::fabsf(toCap.x), extends.y - std::fabsf(toCap.y) , extends.z - std::fabsf(toCap.z) };
					if (absVec.y <= absVec.z && absVec.y <= absVec.x)
					{
						Vector3 off;
						float value{ absVec.y };
						if (toCap.y > 0)
							off = Vector3{ 0, -value - sphere.radius  ,0 };
						else
							off = Vector3{ 0, value + sphere.radius ,0 };
						rigidBody->SetCollisionData(centre - offset + off, -toCap.Normalize());

					}
					else if (absVec.z <= absVec.y && absVec.z <= absVec.x)
					{
						Vector3 off;
						float value{ absVec.z };
						if (toCap.z > 0)
							off = Vector3{ 0,0, -value - sphere.radius };
						else
							off = Vector3{ 0,0, value + sphere.radius };
						rigidBody->SetCollisionData(centre - offset + off, -toCap.Normalize());

					}
					else
					{
						Vector3 off;
						float value{ absVec.x };
						if (toCap.x > 0)
							off = Vector3{ -value - sphere.radius ,0,0 };
						else
							off = Vector3{ value + sphere.radius,0,0 };
						rigidBody->SetCollisionData(centre - offset + off, -toCap.Normalize());

					}


				}

			}
			//rigidBody->SetCollisionData(newPos2 - offset, newNorm);


		}
		else
		{
			if (toCap != Vector3{ 0,0,0 })
			{
				Vector3 absVec{ std::fabsf(toCap.x),std::fabsf(toCap.y) ,std::fabsf(toCap.z) };
				if (absVec.y >= absVec.z && absVec.y >= absVec.x)
				{
					Vector3 off;
					float value{ sphere.radius - absVec.y };
					if (toCap.y > 0)
						off = Vector3{ 0, value / 2.0f  ,0 };
					else
						off = Vector3{ 0, -value / 2.0f ,0 };
					rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());
					rigidBody->SetCollisionData(centre - offset - off, -toCap.Normalize());
				}
				else if (absVec.z >= absVec.y && absVec.z >= absVec.x)
				{
					Vector3 off;
					float value{ sphere.radius - absVec.z };
					if (toCap.z > 0)
						off = Vector3{ 0,0, value / 2.0f };
					else
						off = Vector3{ 0,0, -value / 2.0f };
					rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());
					rigidBody->SetCollisionData(centre - offset - off, -toCap.Normalize());
				}
				else
				{
					Vector3 off;
					float value{ sphere.radius - absVec.x };
					if (toCap.x > 0)
						off = Vector3{ value / 2.0f ,0,0 };
					else
						off = Vector3{ -value / 2.0f,0,0 };
					rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());
					rigidBody->SetCollisionData(centre - offset - off, -toCap.Normalize());
				}

			}
			else
			{
				toCap = nearestPlanePt - centre;

				Vector3 absVec{ extends.x - std::fabsf(toCap.x), extends.y - std::fabsf(toCap.y) , extends.z - std::fabsf(toCap.z) };
				if (absVec.y <= absVec.z && absVec.y <= absVec.x)
				{
					Vector3 off;
					float value{ absVec.y };
					if (toCap.y > 0)
						off = Vector3{ 0, (value + sphere.radius) / 2.0f  ,0 };
					else
						off = Vector3{ 0, (-value - sphere.radius) / 2.0f ,0 };
					rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());
					rigidBody->SetCollisionData(centre - offset - off, -toCap.Normalize());
				}
				else if (absVec.z <= absVec.y && absVec.z <= absVec.x)
				{
					Vector3 off;
					float value{ absVec.z };
					if (toCap.z > 0)
						off = Vector3{ 0,0, (value + sphere.radius) / 2.0f };
					else
						off = Vector3{ 0,0, (-value - sphere.radius) / 2.0f };
					rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());
					rigidBody->SetCollisionData(centre - offset - off, -toCap.Normalize());
				}
				else
				{
					Vector3 off;
					float value{ absVec.x };
					if (toCap.x > 0)
						off = Vector3{ (value + sphere.radius) / 2.0f ,0,0 };
					else
						off = Vector3{ (-value - sphere.radius) / 2.0f,0,0 };
					rhs->rigidBody->SetCollisionData(sphere.centre - sphere.offset + off, toCap.Normalize());
					rigidBody->SetCollisionData(centre - offset - off, -toCap.Normalize());
				}


			}
			//rigidBody->SetCollisionData(newPos2 - offset, newNorm);
			//rhs->rigidBody->SetCollisionData(newPos2 - offset, newNorm);
		}
	}


	AddCollisionData(this, rhs);
	return true;



}
bool ColliderOBB::CheckCollision(ColliderPlane* rhs)
{
	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

	}
	return false;
}

bool ColliderOBB::CheckCollision(ColliderAABB* rhs)
{

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

	}

	return false;
}

bool ColliderOBB::CheckCollision(ColliderOBB* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	float rayD;
	Matrix3x3 absRayM, rayM;

	Vector3 tmpD = centre - rhs->centre;

	Vector3 trans{ tmpD*faceNormals[0],tmpD*faceNormals[1] ,tmpD*faceNormals[2] };

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			rayM(i, j) = faceNormals[i] * rhs->faceNormals[j];
			absRayM(i, j) = std::fabsf(rayM(i, j)) + PhysicsEpsilon;
		}
	}



	/// Test axes lhs side normal
	rayD = extends[0] + rhs->extends[0] * absRayM(0, 0) + rhs->extends[1] * absRayM(0, 1) + rhs->extends[2] * absRayM(0, 2);
	if (std::fabsf(trans.x) > rayD) return false;


	/// Test axes lhs up normal
	rayD = extends[1] + rhs->extends[0] * absRayM(1, 0) + rhs->extends[1] * absRayM(1, 1) + rhs->extends[2] * absRayM(1, 2);
	if (std::fabsf(trans.y) > rayD) return false;

	/// Test axes lhs forward normal
	rayD = extends[2] + rhs->extends[0] * absRayM(2, 0) + rhs->extends[1] * absRayM(2, 1) + rhs->extends[2] * absRayM(2, 2);
	if (std::fabsf(trans.z) > rayD) return false;


	/// Test axes rhs side normal
	rayD = rhs->extends[0] + extends[0] * absRayM(0, 0) + extends[1] * absRayM(1, 0) + extends[2] * absRayM(2, 0);
	if (std::fabsf(trans.x * rayM(0, 0) + trans.y * rayM(1, 0) + trans.z * rayM(2, 0)) > rayD) return false;

	/// Test axes rhs up normal
	rayD = rhs->extends[1] + extends[0] * absRayM(0, 1) + extends[1] * absRayM(1, 1) + extends[2] * absRayM(2, 1);
	if (std::fabsf(trans.x * rayM(0, 1) + trans.y * rayM(1, 1) + trans.z * rayM(2, 1)) > rayD) return false;


	/// Test axes rhs forward normal
	rayD = rhs->extends[2] + extends[0] * absRayM(0, 2) + extends[1] * absRayM(1, 2) + extends[2] * absRayM(2, 2);
	if (std::fabsf(trans.x * rayM(0, 2) + trans.y * rayM(1, 2) + trans.z * rayM(2, 2)) > rayD) return false;



	/// Test axes lhs side normal cross rhs side normal
	rayD = extends[1] * absRayM(2, 0) + extends[2] * absRayM(1, 0);
	rayD += rhs->extends[1] * absRayM(0, 2) + rhs->extends[2] * absRayM(0, 1);
	if (std::fabsf(trans[2] * rayM(1, 0) - trans[1] * rayM(2, 0)) > rayD) return false;


	/// Test axes lhs side normal cross rhs up normal
	rayD = extends[1] * absRayM(2, 1) + extends[2] * absRayM(1, 1);
	rayD += rhs->extends[0] * absRayM(0, 2) + rhs->extends[2] * absRayM(0, 0);
	if (std::fabsf(trans[2] * rayM(1, 1) - trans[1] * rayM(2, 1)) > rayD) return false;

	/// Test axes lhs side normal cross rhs forward normal
	rayD = extends[1] * absRayM(2, 2) + extends[2] * absRayM(1, 2);
	rayD += rhs->extends[0] * absRayM(0, 1) + rhs->extends[1] * absRayM(0, 0);
	if (std::fabsf(trans[2] * rayM(1, 2) - trans[1] * rayM(2, 2)) > rayD) return false;





	/// Test axes lhs up normal cross rhs side normal
	rayD = extends[0] * absRayM(2, 0) + extends[2] * absRayM(0, 0);
	rayD += rhs->extends[1] * absRayM(1, 2) + rhs->extends[2] * absRayM(1, 1);
	if (std::fabsf(trans[2] * rayM(2, 1) - trans[2] * rayM(0, 1)) > rayD) return false;

	/// Test axes lhs up normal cross rhs up normal
	rayD = extends[0] * absRayM(2, 1) + extends[2] * absRayM(0, 1);
	rayD += rhs->extends[0] * absRayM(1, 2) + rhs->extends[2] * absRayM(1, 0);
	if (std::fabsf(trans[0] * rayM(2, 1) - trans[2] * rayM(0, 1)) > rayD) return false;

	/// Test axes lhs up normal cross rhs forward normal
	rayD = extends[0] * absRayM(2, 2) + extends[2] * absRayM(0, 2);
	rayD += rhs->extends[0] * absRayM(1, 1) + rhs->extends[1] * absRayM(1, 0);
	if (std::fabsf(trans[0] * rayM(2, 2) - trans[2] * rayM(0, 2)) > rayD) return false;



	/// Test axes lhs forward normal cross rhs side normal
	rayD = extends[0] * absRayM(1, 0) + extends[1] * absRayM(0, 0);
	rayD += rhs->extends[1] * absRayM(2, 2) + rhs->extends[2] * absRayM(2, 1);
	if (std::fabsf(trans[1] * rayM(0, 0) - trans[0] * rayM(1, 0)) > rayD) return false;

	/// Test axes lhs forward normal cross rhs up normal
	rayD = extends[0] * absRayM(1, 1) + extends[1] * absRayM(0, 1);
	rayD += rhs->extends[0] * absRayM(2, 2) + rhs->extends[2] * absRayM(2, 0);
	if (std::fabsf(trans[1] * rayM(0, 1) - trans[0] * rayM(1, 1)) > rayD) return false;

	/// Test axes lhs forward normal cross rhs forward normal

	rayD = extends[0] * absRayM(1, 2) + extends[1] * absRayM(0, 2);
	rayD += rhs->extends[1] * absRayM(2, 1) + rhs->extends[1] * absRayM(2, 0);
	if (std::fabsf(trans[1] * rayM(0, 2) - trans[0] * rayM(1, 2)) > rayD) return false;

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

	}

	AddCollisionData(this, rhs);

	return true;
}

bool ColliderOBB::CheckCollision(ColliderSphere* rhs)
{

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

	}
	return false;
}

bool ColliderOBB::CheckCollision(ColliderCapsule* rhs)
{
	return rhs->CheckCollision(this);
}

bool ColliderSphere::CheckCollision(ColliderPlane* rhs)
{
	return rhs->CheckCollision(this);
}

bool ColliderSphere::CheckCollision(ColliderAABB* rhs)
{
	return rhs->CheckCollision(this);
}

bool ColliderSphere::CheckCollision(ColliderOBB* rhs)
{

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

	}



	return false;
}

bool ColliderSphere::CheckCollision(ColliderSphere* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	Vector3 cenVec = rhs->centre - centre;
	Vector3 newPos;
	Vector3 newNorm;
	float rad = rhs->radius + radius;
	float dist = cenVec.Length();
	if (dist > rad)
		return false;

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{
		if (rigidBody == nullptr || (rigidBody->isKinematic && rhs->rigidBody))
		{
			if (!rhs->rigidBody->isKinematic)
			{

				cenVec.Normalize();

				newPos = centre + cenVec * (rad);
				newNorm = cenVec;


				rhs->rigidBody->SetCollisionData(newPos - rhs->offset, newNorm);
			}
		}
		else if (rhs->rigidBody == nullptr || (rhs->rigidBody->isKinematic && rigidBody))
		{
			if (!rigidBody->isKinematic)
			{
				//float percentage = radius / rad;
				cenVec.Normalize();

				newPos = centre - cenVec * (rad);
				newNorm = -cenVec;


				rigidBody->SetCollisionData(newPos - offset, newNorm);
			}
		}
		else
		{

			float percentage = radius / rad;

			newPos = centre + cenVec * percentage;
			cenVec.Normalize();

			if (rhs->rigidBody->velocity == Vector3{ 0,0,0 })
			{
				if (rigidBody->velocity == Vector3{ 0,0,0 })
				{
					rigidBody->SetCollisionData(newPos - cenVec * radius - offset, -cenVec);

					rhs->rigidBody->SetCollisionData(newPos + cenVec * rhs->radius - rhs->offset, cenVec);
				}
				else
				{

					float friction{ 0 };
					if (rhs->rigidBody->mass != 0 && rhs->rigidBody->friction != 0)
						friction = rhs->rigidBody->mass * rhs->rigidBody->friction;

					Vector3 impactForce{ rigidBody->velocity * rigidBody->mass };
					Vector3 dotForce = impactForce;
					dotForce = dotForce.Normalize();

					if (impactForce*dotForce > friction)
					{
						impactForce -= friction * dotForce;

						rhs->rigidBody->SetCollisionData(newPos + cenVec * rhs->radius - rhs->offset, cenVec, rigidBody, impactForce.Length() * cenVec);
						rigidBody->SetCollisionData(newPos - cenVec * radius - offset, -cenVec, rhs->rigidBody, impactForce);
					}
					else
					{

						rhs->rigidBody->SetCollisionData(newPos + cenVec * rhs->radius - rhs->offset, cenVec);
						rigidBody->SetCollisionData(newPos - cenVec * radius - offset, -cenVec);

					}
				}
			}
			else if (rigidBody->velocity == Vector3{ 0,0,0 })
			{
				float friction{ 0 };
				if (rigidBody->mass != 0 && rigidBody->friction != 0)
					friction = rigidBody->mass * rigidBody->friction;

				Vector3 impactForce{ rhs->rigidBody->velocity * rhs->rigidBody->mass };
				Vector3 dotForce = impactForce;
				dotForce = dotForce.Normalize();

				if (impactForce*dotForce > friction)
				{
					impactForce -= friction * dotForce;

					rhs->rigidBody->SetCollisionData(newPos + cenVec * rhs->radius - rhs->offset, cenVec, rigidBody, impactForce);
					rigidBody->SetCollisionData(newPos - cenVec * radius - offset, -cenVec, rhs->rigidBody, impactForce.Length() * -cenVec);
				}
				else
				{
					rhs->rigidBody->SetCollisionData(newPos + cenVec * rhs->radius - rhs->offset, cenVec);
					rigidBody->SetCollisionData(newPos - cenVec * radius - offset, -cenVec);
				}
			}
			else
			{

				Vector3 impactForce_lhs{ rigidBody->velocity * rigidBody->mass };
				Vector3 impactForce_rhs{ rhs->rigidBody->velocity * rhs->rigidBody->mass };

				Vector3 tmp{ impactForce_lhs + impactForce_rhs };

				rigidBody->SetCollisionData(newPos - cenVec * radius - offset, -cenVec, rhs->rigidBody, impactForce_rhs);

				rhs->rigidBody->SetCollisionData(newPos + cenVec * rhs->radius - rhs->offset, cenVec, rigidBody, impactForce_lhs);
			}



			//Vector3 selfVec = rigidBody->velocity;
			//Vector3 rhsVec = rhs->rigidBody->velocity;
			//Momentum(selfVec, rigidBody->mass, rhsVec, rhs->rigidBody->mass);
			//
			//std::cout << "self " << rigidBody->velocity << " " << selfVec << std::endl;
			//std::cout << "rhs " << rhs->rigidBody->velocity << " " << rhsVec << std::endl;

		}
	}

	AddCollisionData(this, rhs);
	return true;
}

bool ColliderSphere::CheckCollision(ColliderCapsule* rhs)
{
	return rhs->CheckCollision(this);
}


bool ColliderCapsule::CheckCollision(ColliderPlane* rhs)
{
	//for (auto & elem : rhs->owner->prefabName)
	//	std::cout << elem;
	//std::cout << rhs->centre << " the ? " << centre << std::endl;
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	if (rigidBody && rigidBody->velocity * rhs->faceNormals[1] > 0)
		return false;

	bool edgeCase{ false };

	float upRightTest = rhs->faceNormals[1] * Vector3{ 0,1,0 };
	//std::cout << rhs->centre << "start point " << std::endl;
	Vector3 centreOfCap = centre;
	Vector3 nearestPlanePt = rhs->centre;
	Vector3 x{ rhs->faceNormals[0] };
	Vector3 z{ rhs->faceNormals[2] };

	/// check for negative 0 (plane uprightTest)
	if (upRightTest == 0 || upRightTest == -0)
	{
		float y_shift = nearestPlanePt.y - centreOfCap.y;

		if (y_shift > height)
			y_shift = height;
		if (y_shift < -height)
			y_shift = -height;

		centreOfCap.y += y_shift;

		Vector3 toCap = centreOfCap - nearestPlanePt;

		float dist = x.Normalize() * toCap;
		if (dist > rhs->extends.x)
		{
			dist = rhs->extends.x;
			edgeCase = true;
		}
		if (dist < -rhs->extends.x)
		{
			dist = -rhs->extends.x;
			edgeCase = true;
		}

		x = x * dist;

		dist = z.Normalize() * toCap;
		if (dist > rhs->extends.z)
		{
			dist = rhs->extends.z;
			edgeCase = true;
		}
		if (dist < -rhs->extends.z)
		{
			dist = -rhs->extends.z;
			edgeCase = true;
		}

		z = z * dist;

		nearestPlanePt += x + z;

		//std::cout << nearestPlanePt << " he " << std::endl;
	}
	else
	{
		/// get the nearest capsule y
		Vector3 shift = centreOfCap - nearestPlanePt;

		x *= rhs->extends.x;
		Vector3 tmpx{ x.x, 0, x.z };
		float tmpDist = tmpx.Length();
		float distX = (tmpx.Normalize() * shift) / tmpDist;

		if (distX > 1.0f)
		{
			distX = 1.0f;
			//edgeCase = true;
		}
		else if (distX < -1.0f)
		{
			distX = -1.0f;
			//edgeCase = true;
		}

		x *= distX;


		z *= rhs->extends.z;
		Vector3 tmpz{ z.x, 0, z.z };
		tmpDist = tmpz.Length();
		float distZ = (tmpz.Normalize() *shift) / tmpDist;

		if (distZ > 1.0f)
		{
			distZ = 1.0f;
		}
		else if (distZ < -1.0f)
		{
			distZ = -1.0f;
		}

		z *= distZ;

		nearestPlanePt += x + z;
		//std::cout << nearestPlanePt << " ho " << std::endl;
		float y_shift = nearestPlanePt.y - centreOfCap.y;

		if (y_shift > height)
			y_shift = height;
		if (y_shift < -height)
			y_shift = -height;

		centreOfCap.y += y_shift;

		/// get the nearest plane point to the nearest capsule y
		nearestPlanePt = rhs->centre;
		//std::cout << nearestPlanePt << " h??? " << std::endl;
		x = rhs->faceNormals[0];
		z = rhs->faceNormals[2];

		Vector3 toCap = centreOfCap - nearestPlanePt;

		float dist = x.Normalize() * toCap;
		if (dist > rhs->extends.x)
		{
			dist = rhs->extends.x;
			//std::cout << "x g" << std::endl;
			edgeCase = true;
		}
		if (dist < -rhs->extends.x)
		{
			dist = -rhs->extends.x;
			//std::cout << "x l" << std::endl;
			edgeCase = true;
		}

		x = x * dist;

		dist = z.Normalize() * toCap;
		if (dist > rhs->extends.z)
		{
			dist = rhs->extends.z;
			//std::cout << "z g" << std::endl;
			edgeCase = true;
		}
		if (dist < -rhs->extends.z)
		{
			dist = -rhs->extends.z;
			//std::cout << "z l" << std::endl;
			edgeCase = true;
		}

		z = z * dist;

		nearestPlanePt += x + z;

		//if (edgeCase)
			//std::cout << "THE HECK " << std::endl;

		//std::cout << nearestPlanePt << " ho " << std::endl;
	}

	Vector3 resolvePoint = nearestPlanePt - centreOfCap;
	//std::cout << nearestPlanePt << " " << centreOfCap << std::endl;
	/// if the points are not the same, do sphere and plane test
	if (centreOfCap != nearestPlanePt)
	{
		// no collision
		if (resolvePoint.Length() > radius)
		{

			//std::cout << "--------------------------------------------------------------------------------------------- " << std::endl;
			return false;
		}
	}

	float y_resolve{ 0 };

	//std::cout << resolvePoint.y << std::endl;

	if (resolvePoint.y > 0)
	{
		y_resolve = resolvePoint.y - radius;
		y_resolve = (centre.y + height - centreOfCap.y) + y_resolve;
	}
	else
	{
		y_resolve = radius + resolvePoint.y;
		y_resolve = (centre.y - height - centreOfCap.y) + y_resolve;
	}

	resolvePoint.y = 0;

	float xz_resolve{ resolvePoint.Length() - radius };
	resolvePoint.Normalize();

	Vector3 newPos = centre - offset;

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{
		if (rigidBody == nullptr || (rigidBody->isKinematic && rhs->rigidBody))
		{
			if (!rhs->rigidBody->isKinematic)
			{

				//rhs->rigidBody->SetCollisionData(newPos - rhs->offset, -offSetVar);
				Vector3 newPos2 = rhs->centre - rhs->offset;
				if (std::fabsf(y_resolve) < std::fabsf(xz_resolve))
				{
					newPos2.y -= y_resolve;

				}
				else
				{
					newPos2 -= resolvePoint * xz_resolve;
				}


				rhs->rigidBody->SetCollisionData(newPos2, (nearestPlanePt - centreOfCap).Normalize());
			}

		}
		else if (rhs->rigidBody == nullptr || (rhs->rigidBody->isKinematic && rigidBody))
		{
			if (!rigidBody->isKinematic)
			{

				//rigidBody->SetCollisionData(newPos - offset, offSetVar);


				if (std::fabsf(y_resolve) < std::fabsf(xz_resolve))
				{
					newPos.y += y_resolve;
					//newPos.y += y_resolve - 2.1f;
				}
				else
				{
					newPos += resolvePoint * xz_resolve;
				}

				if (edgeCase)
				{
					rigidBody->SetCollisionData(newPos, (centreOfCap - nearestPlanePt).Normalize());
				}
				else
				{
					rigidBody->SetCollisionData(newPos, rhs->faceNormals[1]);

				}
			}

		}
		else
		{


			Vector3 newPos2 = rhs->centre - rhs->offset;
			if (std::fabsf(y_resolve) < std::fabsf(xz_resolve))
			{
				newPos.y += y_resolve / 2.0f;
				newPos2.y -= y_resolve / 2.0f;
			}
			else
			{
				newPos += resolvePoint * xz_resolve / 2.0f;
				newPos2 -= resolvePoint * xz_resolve / 2.0f;
			}


			if (edgeCase)
			{
				rigidBody->SetCollisionData(newPos, (centreOfCap - nearestPlanePt).Normalize());
			}
			else
			{
				rigidBody->SetCollisionData(newPos, rhs->faceNormals[1]);

			}
			rhs->rigidBody->SetCollisionData(newPos, (nearestPlanePt - centreOfCap).Normalize());
		}
	}
	//std::cout << "collider " << newPos << std::endl;
	AddCollisionData(this, rhs);
	return true;
}

bool ColliderCapsule::CheckCollision(ColliderAABB* rhs)
{
	return rhs->CheckCollision(this);
}

bool ColliderCapsule::CheckCollision(ColliderOBB* rhs)
{
	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{

	}



	return false;
}

bool ColliderCapsule::CheckCollision(ColliderSphere* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	Vector3 colliderPoint = centre;
	Vector3 cenVec = rhs->centre - centre;
	if (cenVec.y > height)
		colliderPoint.y += height;
	else if (cenVec.y < -height)
		colliderPoint.y -= height;
	else
		colliderPoint.y += cenVec.y;

	Vector3 offSetCen = centre - colliderPoint;

	Vector3 colVec = rhs->centre - colliderPoint;
	Vector3 newPos;
	Vector3 newNorm;
	float rad = rhs->radius + radius;
	float dist = colVec.Length();
	if (dist > rad)
		return false;

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{
		if (rigidBody == nullptr || (rigidBody->isKinematic && rhs->rigidBody))
		{
			if (!rhs->rigidBody->isKinematic)
			{

				colVec.Normalize();

				newPos = colliderPoint + (colVec * rad);
				newNorm = colVec;


				rhs->rigidBody->SetCollisionData(newPos - rhs->offset, newNorm);
			}
		}
		else if (rhs->rigidBody == nullptr || (rhs->rigidBody->isKinematic && rigidBody))
		{
			if (!rigidBody->isKinematic)
			{
				//float percentage = radius / rad;
				colVec.Normalize();

				newPos = offSetCen + colliderPoint - (colVec * rad);

				newNorm = -colVec;


				rigidBody->SetCollisionData(newPos - offset, newNorm);
			}
		}
		else
		{

			float percentage = radius / rad;

			newPos = colliderPoint + colVec * percentage;
			colVec.Normalize();

			rigidBody->SetCollisionData(offSetCen + newPos - colVec * radius - offset, -colVec);

			rhs->rigidBody->SetCollisionData(newPos + colVec * rhs->radius - rhs->offset, colVec);

		}
	}
	AddCollisionData(this, rhs);
	return true;
}

bool ColliderCapsule::CheckCollision(ColliderCapsule* rhs)
{
	if (owner->disabled || rhs->owner->disabled)
		return false;
	if (!(ColliderTag & rhs->ColliderTag))
		return false;
	Vector3 colliderPoint = centre;
	Vector3 rhsColliderPoint = rhs->centre;
	Vector3 cenVec = rhs->centre - centre;
	if (cenVec.y > height + rhs->height + radius + rhs->radius)
		return false;
	if (cenVec.y < -(height + rhs->height + radius + rhs->radius))
		return false;

	if (cenVec.y > height + rhs->height)
	{
		colliderPoint.y += height;
		rhsColliderPoint.y -= rhs->height;
	}
	else if (cenVec.y < -(height + rhs->height))
	{
		colliderPoint.y -= height;
		rhsColliderPoint.y += rhs->height;
	}
	else
	{
		colliderPoint.y = 0;
		rhsColliderPoint.y = 0;
	}

	Vector3 offSetCen = centre - colliderPoint;
	Vector3 offSetCenRhs = rhs->centre - rhsColliderPoint;

	Vector3 colVec = rhsColliderPoint - colliderPoint;
	Vector3 newPos;
	Vector3 newNorm;
	float rad = rhs->radius + radius;
	float dist = colVec.Length();
	if (dist > rad)
		return false;

	if (!(isTrigger || rhs->isTrigger) && (rigidBody || rhs->rigidBody))
	{
		if (rigidBody == nullptr || (rigidBody->isKinematic && rhs->rigidBody))
		{
			if (!rhs->rigidBody->isKinematic)
			{
				if (colVec != Vector3{ 0,0,0 })
					colVec.Normalize();

				newPos = offSetCenRhs + rhsColliderPoint + (colVec * rad);
				newNorm = colVec;


				rhs->rigidBody->SetCollisionData(newPos - rhs->offset, newNorm);
			}
		}
		else if (rhs->rigidBody == nullptr || (rhs->rigidBody->isKinematic && rigidBody))
		{
			if (!rigidBody->isKinematic)
			{
				//float percentage = radius / rad;
				if (colVec != Vector3{ 0,0,0 })
					colVec.Normalize();

				newPos = offSetCen + colliderPoint - (colVec * rad);

				newNorm = -colVec;


				rigidBody->SetCollisionData(newPos - offset, newNorm);
			}
		}
		else
		{

			float percentage = radius / rad;

			newPos = colliderPoint + colVec * percentage;
			if (colVec != Vector3{ 0,0,0 })
				colVec.Normalize();

			rigidBody->SetCollisionData(offSetCen + newPos - colVec * radius - offset, -colVec);

			rhs->rigidBody->SetCollisionData(offSetCenRhs + newPos + colVec * rhs->radius - rhs->offset, colVec);

		}
	}
	AddCollisionData(this, rhs);
	return true;
}


ColliderC::ColliderC()
	: rigidBody{ nullptr },
	collisionTime{ floatMax },
	isTrigger{ false }, debugDraw{ true }
{
	RequiredComponents(tranformObject);
	OptionalComponents(rigidBody);
}

bool ColliderC::EditorGUI(Editor&)
{
	bool returnBool = false;
	if (ImGui::TreeNode("CollisionLayer"))
	{
		for (auto &[layer_name, layer_num] : PHYSICS_S.CollisionLayer)
		{
			bool initial = (1 << layer_num) & ColliderTag;
			bool change = initial;
			ImGui::Checkbox(layer_name.c_str(), &change);
			ImGui::SameLine();
			ImGui::Text(layer_name.c_str());
			if (change != initial)
			{
				int bitShift = 1 << layer_num;
				ColliderTag ^= bitShift;
				UpdateSerializables(ColliderTag);
				returnBool = true;
			}
		}
		ImGui::TreePop();
	}
	return returnBool;
}

void ColliderC::CollisionCall()
{
	if (owner->disabled)
		return;


	std::vector<bool> exit;
	exit.reserve(preCollisionData.size());

	for (size_t i = 0; i < preCollisionData.size(); i++)
		exit.emplace_back(true);

	int counter2{ 0 };
	int deleteNum = 0;
	auto iter = newCollisionData.begin();
	auto iterEnd = newCollisionData.end();

	while (iter < iterEnd)
	{
		auto& newEnt = *iter;
		bool found{ false };
		counter2 = 0;
		if (newEnt->owner->disabled)
		{
			++iter;
			continue;
		}

		if (proxies[newEnt->owner].lock())
		{
			for (auto & preEnt : preCollisionData)
			{


				if (newEnt == preEnt)
				{
					owner->CollisionStay(*newEnt->owner);
					exit[counter2] = false;
					found = true;
					break;
				}
				counter2++;
			}
			if (!found)
			{
				owner->CollisionEnter(*newEnt->owner);
			}
		}
		else
		{
			proxies.erase(newEnt->owner);
			std::swap(*iter, *(--iterEnd));
			++deleteNum;
		}
		++iter;
	}
	while (deleteNum > 0)
	{
		newCollisionData.pop_back();
		--deleteNum;
	}


	counter2 = 0;
	for (auto elem : exit)
	{
		if (elem)
		{
			auto prox = proxies.find(preCollisionData[counter2]->owner);
			if (prox != proxies.end())
			{
				auto curr = prox->second.lock();
				if (curr && !preCollisionData[counter2]->owner->disabled)
					owner->CollisionExit(*preCollisionData[counter2]->owner);
				proxies.erase(prox);
			}
		}
		counter2++;
	}


	//  if (newCollisionData.size() || preCollisionData.size())
	//	  std::cout << "s " << newCollisionData.size() << " " << preCollisionData.size() << std::endl;
	std::swap(newCollisionData, preCollisionData);
	newCollisionData.clear();
	//  if (newCollisionData.size() || preCollisionData.size())
	//	  std::cout << "e " << newCollisionData.size() << " " << preCollisionData.size() << std::endl;
}

void ColliderC::AddSerializeData(LuaScript * state)
{
	AddSerializable("Debug Draw", debugDraw, "", state);
	AddSerializable("isTrigger", isTrigger, "", state);
	AddSerializable("offset", offset, "", state);
}



void ColliderC::ResetCollisionTime()
{
	collisionTime = floatMax;
}

void ColliderPlane::Initialize()
{
}

void ColliderAABB::Initialize()
{
}




void ColliderOBB::Initialize()
{
}


void ColliderSphere::Initialize()
{
}


void ColliderCapsule::Initialize()
{

}


void ColliderPlane::AddSerializeData(LuaScript* state)
{
	ColliderC::AddSerializeData(state);
	AddSerializable("Scale", scale, "", state);
	AddSerializable("ColliderLayerCode", ColliderTag, "", state);
	//AddCallableFunction("AddForwardForce", &RigidBodyC::AddForwardForce, {}, state);


}

void ColliderAABB::AddSerializeData(LuaScript* state)
{

	ColliderC::AddSerializeData(state);
	AddSerializable("Scale", scale, "", state);
	AddSerializable("ColliderLayerCode", ColliderTag, "", state);
	AddSerializable("rotationAffect_90", rotationAffect_90, "", state);

	//AddCallableFunction("AddForwardForce", &RigidBodyC::AddForwardForce, {}, state);
	AddCallableFunction("SetTrigger", &ColliderAABB::SetTrigger, {}, state);

}

void ColliderOBB::AddSerializeData(LuaScript* state)
{

	ColliderC::AddSerializeData(state);
	AddSerializable("Scale", scale, "", state);
	AddSerializable("ColliderLayerCode", ColliderTag, "", state);
	//AddCallableFunction("AddForwardForce", &RigidBodyC::AddForwardForce, {}, state);


}

void ColliderSphere::AddSerializeData(LuaScript* state)
{

	ColliderC::AddSerializeData(state);
	AddSerializable("Radius", radius, "", state);
	AddSerializable("ColliderLayerCode", ColliderTag, "", state);
	//AddCallableFunction("AddForwardForce", &RigidBodyC::AddForwardForce, {}, state);


}

void ColliderCapsule::AddSerializeData(LuaScript* state)
{
	ColliderC::AddSerializeData(state);
	AddSerializable("Radius", radius, "", state);
	AddSerializable("Height", height, "", state);
	AddSerializable("ColliderLayerCode", ColliderTag, "", state);
}






void ColliderSphere::EditorUpdate(float)
{
	UpdateCollider();
	if (debugDraw)
		DEBUGDRAW_S.DrawSphere(Sphere{ centre, radius + lineOffSet });
}

void ColliderAABB::EditorUpdate(float)
{
	UpdateCollider();
	if (debugDraw)
	{
		Vector3 extendline{ extends.x + lineOffSet,extends.y + lineOffSet,extends.z + lineOffSet };
		DEBUGDRAW_S.DrawAabb({ centre - extendline, centre + extendline });
	}
}

void ColliderPlane::EditorUpdate(float)
{

	UpdateCollider();
	if (debugDraw)
	{
		Vector3 rightFront{ centre + faceNormals[0] * (extends.x + lineOffSet) + faceNormals[2] * (extends.z + lineOffSet) };
		Vector3 leftFront{ centre - faceNormals[0] * (extends.x + lineOffSet) + faceNormals[2] * (extends.z + lineOffSet) };
		Vector3 rightBack{ centre + faceNormals[0] * (extends.x + lineOffSet) - faceNormals[2] * (extends.z + lineOffSet) };
		Vector3 leftBack{ centre - faceNormals[0] * (extends.x + lineOffSet) - faceNormals[2] * (extends.z + lineOffSet) };
		DEBUGDRAW_S.DrawQuad(rightFront, leftFront, leftBack, rightBack);
	}
}

void ColliderCapsule::EditorUpdate(float)
{
	UpdateCollider();

	if (debugDraw)
	{
		float extendRadius = radius + lineOffSet;
		LineSegment line{ centre - Vector3{ extendRadius,height,0 }, centre - Vector3{ extendRadius,-height ,0 } };
		DEBUGDRAW_S.DrawLine(line);
		line = LineSegment{ centre - Vector3{ 0,height,extendRadius }, centre - Vector3{ 0,-height,extendRadius } };
		DEBUGDRAW_S.DrawLine(line);
		line = LineSegment{ centre - Vector3{ -extendRadius,height,0 }, centre - Vector3{ -extendRadius,-height,0 } };
		DEBUGDRAW_S.DrawLine(line);
		line = LineSegment{ centre - Vector3{ 0,height,-extendRadius }, centre - Vector3{ 0,-height,-extendRadius } };
		DEBUGDRAW_S.DrawLine(line);

		DEBUGDRAW_S.DrawSphere(Sphere{ centre - Vector3{ 0,height,0 }, extendRadius });
		DEBUGDRAW_S.DrawSphere(Sphere{ centre - Vector3{ 0,-height,0 }, extendRadius });
	}
}
