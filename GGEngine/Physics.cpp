#include "stdafx.h"
#include "Physics.h"
#include "Matrix3x3.h"


//================================================================ SYSTEM BUILDING =================================================

#define oneOverSixty 1.0f/60.0f

Physics::Physics()
:
  listOfData{nullptr},
  paused{false}
{
	CoreSystem::SetName("Physics");

	CollisionLayer.emplace("Default", 0);
	CollisionLayer.emplace("Layer1", 1);
	CollisionLayer.emplace("Layer2", 2);
	CollisionLayer.emplace("Layer3", 3);
	CollisionLayer.emplace("Layer4", 4);
	CollisionLayer.emplace("Layer5", 5);
	CollisionLayer.emplace("Layer6", 6);
}


Physics::~Physics()
{
}

void Physics::Initialize(EntityList& data)
{
	listOfData = &data;
}

void Physics::Update()
{
  /* debug group picking
  DebugShape& tmp = DEBUGDRAW_S.DrawQuad(debugTestDraw[0], debugTestDraw[1], debugTestDraw[2], debugTestDraw[3]);
  tmp.color.SetX(0.8f);
  tmp.color.SetY(0.8f);
  tmp.color.SetZ(0.8f);
  */
	dt_holder += DELTATIME;
	while(dt_holder > oneOverSixty)
	{
		dt_holder -= oneOverSixty;
		if (listOfData)
		{
			//! Update all local into global position
			for (auto & transf : listOfData->GetComponents<TransformC>())
			{
				// Settle all the resolve 
				static_cast<TransformC *>(transf)->UpdateTransformationMatrix();
			}

			if (!paused)
			{
				//! Calculate all motion force to predict the end location
				for (auto & rigid : listOfData->GetComponents<RigidBodyC>())
				{

					static_cast<RigidBodyC *>(rigid)->PredictLocation(oneOverSixty);
				}

				/// Set up Collider Box
				SetUpCollisionBox();




				/// Temporary collision Test   - to be updated with spacial partition
				UpdateCollision();



				//! Resolve for collision and physics
				for (auto & rigid : listOfData->GetComponents<RigidBodyC>())
				{
					// Settle all the resolve 
					static_cast<RigidBodyC *>(rigid)->SetLocation();
				}
				for (auto & rigid : listOfData->GetComponents<RigidBodyC>())
				{
					// Settle all the resolve 
					static_cast<RigidBodyC *>(rigid)->SecondLayerResolving();
				}




			}
			else
			{
				for (auto & rigid : listOfData->GetComponents<RigidBodyC>())
				{

					static_cast<RigidBodyC *>(rigid)->PauseUpdate();
				}
			}

		}
	}

  
}

void Physics::SetUpCollisionBox()
{

  auto & collisPlane = listOfData->GetComponents<ColliderPlane>();
  for (auto & elem : collisPlane)
  {
	  static_cast<ColliderPlane *>(elem)->CollisionCall();
	  static_cast<ColliderPlane *>(elem)->UpdateCollider();
  }
  auto & collisAABB = listOfData->GetComponents<ColliderAABB>();
  for (auto & elem : collisAABB)
  {
	  static_cast<ColliderAABB *>(elem)->CollisionCall();
	  static_cast<ColliderAABB *>(elem)->UpdateCollider();
  }
  auto & collisOBB = listOfData->GetComponents<ColliderOBB>();
  for (auto & elem : collisOBB)
  {
	  static_cast<ColliderOBB *>(elem)->CollisionCall();
	  static_cast<ColliderOBB *>(elem)->UpdateCollider();
  }
  auto & collisSphere = listOfData->GetComponents<ColliderSphere>();
  for (auto & elem : collisSphere)
  {
	  static_cast<ColliderSphere *>(elem)->CollisionCall();
	  static_cast<ColliderSphere *>(elem)->UpdateCollider();
  }
  auto & collisCapsule = listOfData->GetComponents<ColliderCapsule>();
  for (auto & elem : collisCapsule)
  {
	  static_cast<ColliderCapsule *>(elem)->CollisionCall();
	  static_cast<ColliderCapsule *>(elem)->UpdateCollider();
  }
  //std::cout << " lifetime" << std::endl;
}

void Physics::UpdateCollision()
{

  //std::map<ColliderC*, collisionData> colliderMap;
    auto & collisPlane = listOfData->GetComponents<ColliderPlane>();
    auto & collisAABB = listOfData->GetComponents<ColliderAABB>();
    auto & collisOBB = listOfData->GetComponents<ColliderOBB>();
    auto & collisSphere = listOfData->GetComponents<ColliderSphere>();
    auto & collisCapsule = listOfData->GetComponents<ColliderCapsule>();


    //! do for all plane
    {
      auto itrStart = collisPlane.begin();
      auto itrEnd = collisPlane.end();
      auto itr = itrStart;

      while (itrStart != itrEnd)
      {
        ColliderPlane * currItr = static_cast<ColliderPlane *>(*itrStart);
        itr = itrStart;
        itr++;
		//std::cout << "start " << std::endl;
		//std::cout << currItr->centre << std::endl;

        while (itr != itrEnd)
        {

          currItr->CheckCollision(static_cast<ColliderPlane *>(*itr));
          itr++;
        }

		//std::cout << currItr->centre << std::endl;

        for (auto & col : collisAABB)
        {
          currItr->CheckCollision(static_cast<ColliderAABB *>(col));
        }
		
		//std::cout << currItr->centre << std::endl;

        for (auto & col : collisOBB)
        {
          currItr->CheckCollision(static_cast<ColliderOBB *>(col));
        }

		//std::cout << currItr->centre << std::endl;

        for (auto & col : collisSphere)
        {
          currItr->CheckCollision(static_cast<ColliderSphere *>(col));
        }

		//std::cout << currItr->centre << std::endl;

        for (auto & col : collisCapsule)
        {
          currItr->CheckCollision(static_cast<ColliderCapsule *>(col));
        }

		//std::cout << currItr->centre << std::endl;

        itrStart++;
      }
    }



    //! do for all aabb
    {
      auto itrStart = collisAABB.begin();
      auto itrEnd = collisAABB.end();
      auto itr = itrStart;

      while (itrStart != itrEnd)
      {
        ColliderAABB * currItr = static_cast<ColliderAABB *>(*itrStart);
        itr = itrStart;
        itr++;

        while (itr != itrEnd)
        {

          currItr->CheckCollision(static_cast<ColliderAABB *>(*itr));
          itr++;
        }

        for (auto & col : collisOBB)
        {
          currItr->CheckCollision(static_cast<ColliderOBB *>(col));
        }

        for (auto & col : collisSphere)
        {
          currItr->CheckCollision(static_cast<ColliderSphere *>(col));
        }

        for (auto & col : collisCapsule)
        {
          currItr->CheckCollision(static_cast<ColliderCapsule *>(col));
        }

        itrStart++;
      }
    }





    //! do for all OBB
    {
      auto itrStart = collisOBB.begin();
      auto itrEnd = collisOBB.end();
      auto itr = itrStart;

      while (itrStart != itrEnd)
      {
        ColliderOBB * currItr = static_cast<ColliderOBB *>(*itrStart);
        itr = itrStart;
        itr++;

        while (itr != itrEnd)
        {

          currItr->CheckCollision(static_cast<ColliderOBB *>(*itr));
          itr++;
        }

        for (auto & col : collisSphere)
        {
          currItr->CheckCollision(static_cast<ColliderSphere *>(col));
        }

        for (auto & col : collisCapsule)
        {
          currItr->CheckCollision(static_cast<ColliderCapsule *>(col));
        }

        itrStart++;
      }
    }





    //! do for all Sphere
    {
      auto itrStart = collisSphere.begin();
      auto itrEnd = collisSphere.end();
      auto itr = itrStart;

      while (itrStart != itrEnd)
      {
        ColliderSphere * currItr = static_cast<ColliderSphere *>(*itrStart);
        itr = itrStart;
        itr++;

        while (itr != itrEnd)
        {

          currItr->CheckCollision(static_cast<ColliderSphere *>(*itr));
          itr++;
        }

        for (auto & col : collisCapsule)
        {
          currItr->CheckCollision(static_cast<ColliderCapsule *>(col));
        }

        itrStart++;
      }
    }


    {
      auto itrStart = collisCapsule.begin();
      auto itrEnd = collisCapsule.end();
      auto itr = itrStart;

      while (itrStart != itrEnd)
      {
        ColliderCapsule * currItr = static_cast<ColliderCapsule *>(*itrStart);
        itr = itrStart;
        itr++;

        while (itr != itrEnd)
        {

          currItr->CheckCollision(static_cast<ColliderCapsule *>(*itr));
          itr++;
        }

        itrStart++;
      }
    }

}

void Physics::PhysicsPause(bool pause)
{
  paused = pause;
}

std::vector<Entity *> Physics::PhysicsGroupModel(std::vector<Vector4> & planes)
{
  std::vector<Entity *> group;

  auto & trans = listOfData->GetComponents<TransformC>();

  for (auto & elem : trans)
  {
    TransformC * model = static_cast<TransformC *>(elem);

    auto modelScale = [&](Vector3 & model, Vector3 scale)
    {
      model.x *= scale.x;
      model.y *= scale.y;
      model.z *= scale.z;
    };

    Vector3 minAABB = model->ModelMin;
    Vector3 maxAABB = model->ModelMax;

    modelScale(minAABB, model->GetGlobalScale());
    modelScale(maxAABB, model->GetGlobalScale());

    minAABB += model->GetGlobalPosition();
    maxAABB += model->GetGlobalPosition();
    

    auto centre = (minAABB + maxAABB) / 2.0f;
    auto extend = maxAABB - centre;
    


    int i = 0;
    for (const auto & n_planes : planes)
    {
      auto & SinglePlane = n_planes;
      Vector3 u;
      u.x = SinglePlane.x >= 0.f ? extend.x : -extend.x;
      u.y = SinglePlane.y >= 0.f ? extend.y : -extend.y;
      u.z = SinglePlane.z >= 0.f ? extend.z : -extend.z;

      auto planePos = Vector3{ SinglePlane.x, SinglePlane.y, SinglePlane.z };
      auto radius = planePos * u;


      float dis = (centre*planePos) - n_planes.w;
      auto projection = centre - (planePos * dis);

      Vector3 dir_rad = centre - projection;

      if (dir_rad.Length() * dir_rad.Length() <= radius * radius)
      {
        break;
      }
      else if (dir_rad*planePos > 0)
      {
        if (++i == planes.size())
          group.emplace_back(model->owner);
      }
      else
        break;
      
    }


  }




  return group;
}

Entity * Physics::PhysicsRayCastModel(const Vector3 & pos, const Vector3 & dir, float maxDist)
{
  Entity * found = nullptr;

  auto & trans = listOfData->GetComponents<TransformC>();
  
  Vector3 dirN = dir;
  dirN.Normalize();

  
  for (auto & elem : trans)
  {
    TransformC * model = static_cast<TransformC *>(elem);



    auto modelScale = [&](Vector3 & model, Vector3 scale)
    {
      model.x *= scale.x;
      model.y *= scale.y;
      model.z *= scale.z;
    };

    Vector3 minAABB = model->ModelMin;
    Vector3 maxAABB = model->ModelMax;

    modelScale(minAABB, model->GetGlobalScale());
    modelScale(maxAABB, model->GetGlobalScale());



    Vector3 ref = model->GetRotation();

    while (ref.x < 0)
      ref.x += 360.0f;

    int tmp = ((static_cast<int>(ref.x) + 45) / 90) % 2;

    if (tmp)
    {
      std::swap(minAABB.y, minAABB.z);
      std::swap(maxAABB.y, maxAABB.z);
    }

    while (ref.y < 0)
      ref.y += 360.0f;

    tmp = ((static_cast<int>(ref.y) + 45) / 90) % 2;

    if (tmp)
    {
      std::swap(minAABB.x, minAABB.z);
      std::swap(maxAABB.x, maxAABB.z);
    }


    while (ref.z < 0)
      ref.z += 360.0f;

    tmp = ((static_cast<int>(ref.z) + 45) / 90) % 2;

    if (tmp)
    {
      std::swap(minAABB.x, minAABB.y);
      std::swap(maxAABB.x, maxAABB.y);
    }

    minAABB += model->GetGlobalPosition();
    maxAABB += model->GetGlobalPosition();
    std::vector<float> minValues, maxValues;


    auto checkAndEmplace = [&](float aMin, float aMax, float b, float c)
    {
      if (c > 0.f)
      {
        minValues.emplace_back((aMin - b) / c);
        maxValues.emplace_back((aMax - b) / c);
      }
      else if (c < 0.f)
      {
        maxValues.emplace_back((aMin - b) / c);
        minValues.emplace_back((aMax - b) / c);
      }
      else if (b < aMin || b > aMax)
        return false;
      return true;
    };

    if (!checkAndEmplace(minAABB.x, maxAABB.x, pos.x, dir.x)) continue;
    if (!checkAndEmplace(minAABB.y, maxAABB.y, pos.y, dir.y)) continue;
    if (!checkAndEmplace(minAABB.z, maxAABB.z, pos.z, dir.z)) continue;

    auto maxFunc = [](const std::vector<float>& vec)
    {
      float max = std::numeric_limits<float>::lowest();
      for (auto item : vec)
      {
        if (max < item) max = item;
      }
      return max;
    };
    auto minFunc = [](const std::vector<float>& vec)
    {
      float min = std::numeric_limits<float>::max();
      for (auto item : vec)
      {
        if (min > item) min = item;
      }
      return min;
    };

    float tMin = maxFunc(minValues);
    float tMax = minFunc(maxValues);
    
    if (tMin > tMax) continue;
    if (tMax < 0.f || tMin < 0.f) continue;

    if (tMin < maxDist)
    {
      found = model->owner;
      maxDist = tMin;
    }


  }

  //if (found != nullptr)
  //  std::cout << " found yo " << std::endl;
  return found;
}

Entity * Physics::PhysicsRayCast(const Vector3 & pos, const Vector3 & dir, float maxDist)
{
  Entity * found = nullptr;
  Entity * tmp = nullptr;
  found = PhysicsRayCastPlane(pos, dir, maxDist);
  tmp = PhysicsRayCastAABB(pos, dir, maxDist);
  if (tmp != nullptr)
    found = tmp;
  tmp = PhysicsRayCastOBB(pos, dir, maxDist);
  if (tmp != nullptr)
    found = tmp;
  tmp = PhysicsRayCastCapsule(pos, dir, maxDist);
  if (tmp != nullptr)
    found = tmp;
  tmp = PhysicsRayCastSphere(pos, dir, maxDist);
  if (tmp != nullptr)
    found = tmp;

  return found;
}

Entity * Physics::PhysicsRayCastPlane(const Vector3 & pos, const Vector3 & dir, float & maxDist)
{
  Entity * found = nullptr;

  auto & trans = listOfData->GetComponents<ColliderPlane>();

  Vector3 dirN = dir;
  dirN.Normalize();

  for (auto & elem : trans)
  {
    ColliderPlane * model = static_cast<ColliderPlane *>(elem);
    Vector3 centre = model->centre;

    if (dir * (centre - pos) < 0)
      continue;

    if (dir * model->faceNormals[1] > 0)
      continue;


    float d = -(centre * model->faceNormals[1]);
    float t = -(pos*model->faceNormals[1] + d) / (dir * model->faceNormals[1]);

    if (t >= maxDist || t < 0)
      continue;

    Vector3 intersect = pos + t * dir;

    Vector3 onQuad = intersect - centre;

    if (std::fabsf(onQuad * model->faceNormals[0]) > model->extends.x)
      continue;

    if (std::fabsf(onQuad * model->faceNormals[2]) > model->extends.z)
      continue;

    found = model->owner;
    maxDist = t;

  }



  return found;
}

Entity * Physics::PhysicsRayCastAABB(const Vector3 & pos, const Vector3 & dir, float & maxDist)
{
  Entity * found = nullptr;

  auto & trans = listOfData->GetComponents<ColliderAABB>();

  //auto & trans = listOfData->GetComponents<TransformC>();

  Vector3 dirN = dir;
  dirN.Normalize();


  for (auto & elem : trans)
  {
    ColliderAABB * model = static_cast<ColliderAABB *>(elem);



    auto modelScale = [&](Vector3 & model, Vector3 scale)
    {
      model.x *= scale.x;
      model.y *= scale.y;
      model.z *= scale.z;
    };

    Vector3 minAABB = model->centre - model->extends;
    Vector3 maxAABB = model->centre + model->extends;

    //modelScale(minAABB, model->GetGlobalScale());
    //modelScale(maxAABB, model->GetGlobalScale());
    //
    //minAABB += model->GetGlobalPosition();
    //maxAABB += model->GetGlobalPosition();

    std::vector<float> minValues, maxValues;


    auto checkAndEmplace = [&](float aMin, float aMax, float b, float c)
    {
      if (c > 0.f)
      {
        minValues.emplace_back((aMin - b) / c);
        maxValues.emplace_back((aMax - b) / c);
      }
      else if (c < 0.f)
      {
        maxValues.emplace_back((aMin - b) / c);
        minValues.emplace_back((aMax - b) / c);
      }
      else if (b < aMin || b > aMax)
        return false;
      return true;
    };

    if (!checkAndEmplace(minAABB.x, maxAABB.x, pos.x, dir.x)) continue;
    if (!checkAndEmplace(minAABB.y, maxAABB.y, pos.y, dir.y)) continue;
    if (!checkAndEmplace(minAABB.z, maxAABB.z, pos.z, dir.z)) continue;

    auto maxFunc = [](const std::vector<float>& vec)
    {
      float max = std::numeric_limits<float>::lowest();
      for (auto item : vec)
      {
        if (max < item) max = item;
      }
      return max;
    };
    auto minFunc = [](const std::vector<float>& vec)
    {
      float min = std::numeric_limits<float>::max();
      for (auto item : vec)
      {
        if (min > item) min = item;
      }
      return min;
    };

    float tMin = maxFunc(minValues);
    float tMax = minFunc(maxValues);

    if (tMin > tMax) continue;
    if (tMax < 0.f || tMin < 0.f) continue;

    if (tMin < maxDist)
    {
      found = model->owner;
      maxDist = tMin;
    }


  }

  //if (found != nullptr)
  //  std::cout << " found yo " << std::endl;
  return found;
}

Entity * Physics::PhysicsRayCastOBB(const Vector3 & pos, const Vector3 & dir, float & maxDist)
{
  Entity * found = nullptr;

  auto & trans = listOfData->GetComponents<ColliderOBB>();

  Vector3 dirN = dir;
  dirN.Normalize();

  for (auto & elem : trans)
  {
    ColliderOBB * model = static_cast<ColliderOBB *>(elem);

    Vector3 centre = model->centre;
    Vector3 tgt;
    Vector3 max{ centre + model->extends };
    Vector3 min{ centre - model->extends };


    if (dir * (centre - pos) < 0)
      continue;




    if (dir.x > 0)
    {
      tgt = centre;
      tgt.x += min.x;

      //Vector3 Vec{ tgt - pos };
      Vector3 Vec{ tgt.x - pos.x, pos.y, pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.y - centre.y) < max.y && std::fabsf(face.z - centre.z) < max.z)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            found = model->owner;
          }
        }

      }
    }
    else if (dir.x < 0)
    {
      tgt = centre;
      tgt.x += max.x;

      Vector3 Vec{ tgt.x - pos.x, pos.y, pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.y - centre.y) < max.y && std::fabsf(face.z - centre.z) < max.z)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            found = model->owner;
          }
        }

      }
    }


    if (dir.y > 0)
    {
      tgt = centre;
      tgt.y += min.y;

      Vector3 Vec{ pos.x, tgt.y - pos.y, pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.x - centre.x) < max.x && std::fabsf(face.z - centre.z) < max.z)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            found = model->owner;
          }
        }

      }
    }
    else if (dir.y < 0)
    {
      tgt = centre;
      tgt.y += max.y;

      Vector3 Vec{ pos.x, tgt.y - pos.y, pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.x - centre.x) < max.x && std::fabsf(face.z - centre.z) < max.z)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            found = model->owner;
          }
        }

      }
    }

    if (dir.z > 0)
    {
      tgt = centre;
      tgt.z += min.z;

      Vector3 Vec{ pos.x, pos.y, tgt.z - pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.x - centre.x) < max.x && std::fabsf(face.y - centre.y) < max.y)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            found = model->owner;
          }
        }

      }
    }
    else if (dir.z < 0)
    {
      tgt = centre;
      tgt.z += max.z;

      Vector3 Vec{ pos.x, pos.y, tgt.z - pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.x - centre.x) < max.x && std::fabsf(face.y - centre.y) < max.y)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            found = model->owner;
          }
        }

      }
    }
  }


  return found;
}

Entity * Physics::PhysicsRayCastCapsule(const Vector3 & pos, const Vector3 & dir, float & maxDist)
{
  Entity * found = nullptr;


  auto & trans = listOfData->GetComponents<ColliderCapsule>();

  Vector3 dirN = dir;
  dirN.Normalize();

  for (auto & elem : trans)
  {
    ColliderCapsule * model = static_cast<ColliderCapsule *>(elem);

    Vector3 centre = model->centre;

    float radius = model->radius;

    Vector3 ref = centre - pos;
    ref.y = 0;
    float shiftDist = ref.Length();

    if (shiftDist == 0)
      continue;

    Vector3 ref_2 = dirN;
    ref_2.y = 0;
    float shiftDist_2 = ref_2.Length();
    ref = ref_2;

    if (shiftDist_2 != 0)
    {
      ref_2 = dirN * shiftDist / shiftDist_2;
      ref = -dirN * radius / shiftDist_2;
      float offSet_y = (pos.y - centre.y) + ref.y + ref_2.y;

      if (offSet_y > model->height)
        offSet_y = model->height;
      else if (offSet_y < -model->height)
        offSet_y = -model->height;

      centre.y += offSet_y;

    }

    if (dir * (centre - pos) < 0)
      continue;

    float tmp = (dirN * (centre - pos));

    Vector3 cut = tmp * dirN + pos;

    Vector3 a{ centre - cut };
    if (a == Vector3{ 0,0,0 })
    {
      tmp -= radius;
      if (tmp < maxDist)
      {
        maxDist = tmp;
        found = model->owner;
      }
    }
    else
    {
      if (a.Length() > radius)
        continue;
      float aL = a.Length();
      aL = aL * aL;

      float hL = radius * radius;
      float rad = std::sqrtf(hL - aL);

      tmp -= rad;
      if (tmp < maxDist)
      {
        maxDist = tmp;
        found = model->owner;
      }
    }

  }

  return found;
}

Entity * Physics::PhysicsRayCastSphere(const Vector3 & pos, const Vector3 & dir, float & maxDist)
{
  Entity * found = nullptr;


  auto & trans = listOfData->GetComponents<ColliderSphere>();

  Vector3 dirN = dir;
  dirN.Normalize();

  for (auto & elem : trans)
  {
    ColliderSphere * model = static_cast<ColliderSphere *>(elem);

    Vector3 centre = model->centre;
    
    float radius = model->radius;

    if (dir * (centre - pos) < 0)
      continue;

    float tmp = (dirN * (centre - pos));

    Vector3 cut = tmp * dirN + pos;

    Vector3 a{ centre - cut };
    if (a == Vector3{ 0,0,0 })
    {
      tmp -= radius;
      if (tmp < maxDist)
      {
        maxDist = tmp;
        found = model->owner;
      }
    }
    else
    {
      if (a.Length() > radius)
        continue;
      float aL = a.Length();
      aL = aL * aL;

      float hL = radius * radius;
      float rad = std::sqrtf(hL - aL);

      tmp -= rad;
      if (tmp < maxDist)
      {
        maxDist = tmp;
        found = model->owner;
      }
    }

  }

  return found;
}

Entity * Physics::PhysicsRayCastContactData(const Vector3 & pos, const Vector3 & dir, float maxDist, Vector3 & pointOfContact)
{
  if (maxDist == 0)
    maxDist = FLT_MAX;
  Entity * found = nullptr;
  Entity * tmp = nullptr;
  found = PhysicsRayCastPlaneContactData(pos, dir, maxDist, pointOfContact);
  tmp = PhysicsRayCastAABBContactData(pos, dir, maxDist, pointOfContact);
  if (tmp != nullptr)
    found = tmp;
  tmp = PhysicsRayCastOBBContactData(pos, dir, maxDist, pointOfContact);
  if (tmp != nullptr)
    found = tmp;
  tmp = PhysicsRayCastCapsuleContactData(pos, dir, maxDist, pointOfContact);
  if (tmp != nullptr)
    found = tmp;
  tmp = PhysicsRayCastSphereContactData(pos, dir, maxDist, pointOfContact);
  if (tmp != nullptr)
    found = tmp;

  return found;
}

Entity * Physics::PhysicsRayCastPlaneContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact)
{
  Entity * found = nullptr;

  auto & trans = listOfData->GetComponents<ColliderPlane>();

  Vector3 dirN = dir;
  dirN.Normalize();

  for (auto & elem : trans)
  {
    ColliderPlane * model = static_cast<ColliderPlane *>(elem);
    Vector3 centre = model->centre;

    if (dir * (centre - pos) < 0)
      continue;

    if (dir * model->faceNormals[1] > 0)
      continue;


    float d = -(centre * model->faceNormals[1]);
    float t = -(pos*model->faceNormals[1] + d) / (dir * model->faceNormals[1]);

    if (t >= maxDist || t < 0)
      continue;

    Vector3 intersect = pos + t * dir;

    Vector3 onQuad = intersect - centre;

    if (std::fabsf(onQuad * model->faceNormals[0]) > model->extends.x)
      continue;

    if (std::fabsf(onQuad * model->faceNormals[2]) > model->extends.z)
      continue;

    found = model->owner;
    maxDist = t;

    pointOfContact = pos + (dirN * t);
  }



  return found;
}

Entity * Physics::PhysicsRayCastAABBContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact)
{
  Entity * found = nullptr;

  auto & trans = listOfData->GetComponents<ColliderAABB>();

  //auto & trans = listOfData->GetComponents<TransformC>();

  Vector3 dirN = dir;
  dirN.Normalize();


  for (auto & elem : trans)
  {
    ColliderAABB * model = static_cast<ColliderAABB *>(elem);



    auto modelScale = [&](Vector3 & model, Vector3 scale)
    {
      model.x *= scale.x;
      model.y *= scale.y;
      model.z *= scale.z;
    };

    Vector3 minAABB = model->centre - model->extends;
    Vector3 maxAABB = model->centre + model->extends;

    //modelScale(minAABB, model->GetGlobalScale());
    //modelScale(maxAABB, model->GetGlobalScale());
    //
    //minAABB += model->GetGlobalPosition();
    //maxAABB += model->GetGlobalPosition();

    std::vector<float> minValues, maxValues;


    auto checkAndEmplace = [&](float aMin, float aMax, float b, float c)
    {
      if (c > 0.f)
      {
        minValues.emplace_back((aMin - b) / c);
        maxValues.emplace_back((aMax - b) / c);
      }
      else if (c < 0.f)
      {
        maxValues.emplace_back((aMin - b) / c);
        minValues.emplace_back((aMax - b) / c);
      }
      else if (b < aMin || b > aMax)
        return false;
      return true;
    };

    if (!checkAndEmplace(minAABB.x, maxAABB.x, pos.x, dir.x)) continue;
    if (!checkAndEmplace(minAABB.y, maxAABB.y, pos.y, dir.y)) continue;
    if (!checkAndEmplace(minAABB.z, maxAABB.z, pos.z, dir.z)) continue;

    auto maxFunc = [](const std::vector<float>& vec)
    {
      float max = std::numeric_limits<float>::lowest();
      for (auto item : vec)
      {
        if (max < item) max = item;
      }
      return max;
    };
    auto minFunc = [](const std::vector<float>& vec)
    {
      float min = std::numeric_limits<float>::max();
      for (auto item : vec)
      {
        if (min > item) min = item;
      }
      return min;
    };

    float tMin = maxFunc(minValues);
    float tMax = minFunc(maxValues);

    if (tMin > tMax) continue;
    if (tMax < 0.f || tMin < 0.f) continue;

    if (tMin < maxDist)
    {
      found = model->owner;
      maxDist = tMin;
      pointOfContact = pos + (dirN * tMin);
    }


  }

  //if (found != nullptr)
  //  std::cout << " found yo " << std::endl;
  return found;
}

Entity * Physics::PhysicsRayCastOBBContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact)
{
  Entity * found = nullptr;

  auto & trans = listOfData->GetComponents<ColliderOBB>();

  Vector3 dirN = dir;
  dirN.Normalize();

  for (auto & elem : trans)
  {
    ColliderOBB * model = static_cast<ColliderOBB *>(elem);

    Vector3 centre = model->centre;
    Vector3 tgt;
    Vector3 max{ centre + model->extends };
    Vector3 min{ centre - model->extends };


    if (dir * (centre - pos) < 0)
      continue;




    if (dir.x > 0)
    {
      tgt = centre;
      tgt.x += min.x;

      //Vector3 Vec{ tgt - pos };
      Vector3 Vec{ tgt.x - pos.x, pos.y, pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.y - centre.y) < max.y && std::fabsf(face.z - centre.z) < max.z)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            pointOfContact = pos + (dirN * tmp);
            found = model->owner;
          }
        }

      }
    }
    else if (dir.x < 0)
    {
      tgt = centre;
      tgt.x += max.x;

      Vector3 Vec{ tgt.x - pos.x, pos.y, pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.y - centre.y) < max.y && std::fabsf(face.z - centre.z) < max.z)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            pointOfContact = pos + (dirN * tmp);
            found = model->owner;
          }
        }

      }
    }


    if (dir.y > 0)
    {
      tgt = centre;
      tgt.y += min.y;

      Vector3 Vec{ pos.x, tgt.y - pos.y, pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.x - centre.x) < max.x && std::fabsf(face.z - centre.z) < max.z)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            pointOfContact = pos + (dirN * tmp);
            found = model->owner;
          }
        }

      }
    }
    else if (dir.y < 0)
    {
      tgt = centre;
      tgt.y += max.y;

      Vector3 Vec{ pos.x, tgt.y - pos.y, pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.x - centre.x) < max.x && std::fabsf(face.z - centre.z) < max.z)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            pointOfContact = pos + (dirN * tmp);
            found = model->owner;
          }
        }

      }
    }

    if (dir.z > 0)
    {
      tgt = centre;
      tgt.z += min.z;

      Vector3 Vec{ pos.x, pos.y, tgt.z - pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.x - centre.x) < max.x && std::fabsf(face.y - centre.y) < max.y)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            pointOfContact = pos + (dirN * tmp);
            found = model->owner;
          }
        }

      }
    }
    else if (dir.z < 0)
    {
      tgt = centre;
      tgt.z += max.z;

      Vector3 Vec{ pos.x, pos.y, tgt.z - pos.z };

      if ((Vec * dir)> 0)
      {
        float tmp;

        tmp = dirN * Vec;

        Vector3 face = tmp * dirN + pos;

        Vector3 checkFace{ face - centre };
        if (std::fabsf(face.x - centre.x) < max.x && std::fabsf(face.y - centre.y) < max.y)
        {
          if (tmp < maxDist)
          {
            maxDist = tmp;
            pointOfContact = pos + (dirN * tmp);
            found = model->owner;
          }
        }

      }
    }
  }


  return found;
}

Entity * Physics::PhysicsRayCastCapsuleContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact)
{
  Entity * found = nullptr;


  auto & trans = listOfData->GetComponents<ColliderCapsule>();

  Vector3 dirN = dir;
  dirN.Normalize();

  for (auto & elem : trans)
  {
    ColliderCapsule * model = static_cast<ColliderCapsule *>(elem);

    Vector3 centre = model->centre;

    float radius = model->radius;

    Vector3 ref = centre - pos;
    ref.y = 0;
    float shiftDist = ref.Length();

    if (shiftDist == 0)
      continue;

    Vector3 ref_2 = dirN;
    ref_2.y = 0;
    float shiftDist_2 = ref_2.Length();
    ref = ref_2;

    if (shiftDist_2 != 0)
    {
      ref_2 = dirN * shiftDist / shiftDist_2;
      ref = -dirN * radius / shiftDist_2;
      float offSet_y = (pos.y - centre.y) + ref.y + ref_2.y;

      if (offSet_y > model->height)
        offSet_y = model->height;
      else if (offSet_y < -model->height)
        offSet_y = -model->height;

      centre.y += offSet_y;

    }

    if (dir * (centre - pos) < 0)
      continue;

    float tmp = (dirN * (centre - pos));

    Vector3 cut = tmp * dirN + pos;

    Vector3 a{ centre - cut };
    if (a == Vector3{ 0,0,0 })
    {
      tmp -= radius;
      if (tmp < maxDist)
      {
        maxDist = tmp;
        pointOfContact = pos + (dirN * tmp);
        found = model->owner;
      }
    }
    else
    {
      if (a.Length() > radius)
        continue;
      float aL = a.Length();
      aL = aL * aL;

      float hL = radius * radius;
      float rad = std::sqrtf(hL - aL);

      tmp -= rad;
      if (tmp < maxDist)
      {
        maxDist = tmp;
        pointOfContact = pos + (dirN * tmp);
        found = model->owner;
      }
    }

  }

  return found;
}

Entity * Physics::PhysicsRayCastSphereContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact)
{
  Entity * found = nullptr;


  auto & trans = listOfData->GetComponents<ColliderSphere>();

  Vector3 dirN = dir;
  dirN.Normalize();

  for (auto & elem : trans)
  {
    ColliderSphere * model = static_cast<ColliderSphere *>(elem);

    Vector3 centre = model->centre;

    float radius = model->radius;

    if (dir * (centre - pos) < 0)
      continue;

    float tmp = (dirN * (centre - pos));

    Vector3 cut = tmp * dirN + pos;

    Vector3 a{ centre - cut };
    if (a == Vector3{ 0,0,0 })
    {
      tmp -= radius;
      if (tmp < maxDist)
      {
        maxDist = tmp;
        found = model->owner;
        pointOfContact = pos + (dirN * tmp);
      }
    }
    else
    {
      if (a.Length() > radius)
        continue;
      float aL = a.Length();
      aL = aL * aL;

      float hL = radius * radius;
      float rad = std::sqrtf(hL - aL);

      tmp -= rad;
      if (tmp < maxDist)
      {
        maxDist = tmp;
        found = model->owner;
        pointOfContact = pos + (dirN * tmp);
      }
    }

  }

  return found;
}