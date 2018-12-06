#pragma once
#include "TransformC.h"
#include "CollisionC.h"
#include "RigidBodyC.h"
#include "ParticleEmitter.h"
#include "common.h"
#include <unordered_set>

#define floatMax FLT_MAX
#define PhysicsEpsilon 0.00001f
#define ReflectEpsilon 2.5f


class EntityList;
struct LineSegment;




class GridNode
{
  std::vector<ColliderC *> roomObject;

  std::vector<ColliderC *> dynamicObject;

};

class GridTree
{
  int partionX;
  int partionY;
  int partionZ;


  Vector3 position;

  float perGridHeight;
  float perGridWidth;
  float perGridLength;

  /// Y X Z
  std::vector<std::vector<std::vector<GridNode>>> tree;

  /// store all dynamic Object into the grid per frame base on the 8 points
  std::vector<ColliderC *> dynamicObject;

  /// to not duplicate test grids
  std::unordered_set<GridNode*> dynamicGrid;
  

};


class Physics : public CoreSystem
{
	EntityList* listOfData;
	bool paused;
	float dt_holder = 0;
  //bool lineDraw = false;
  //Vector3 startLine;
  //Vector3 endLine;

public:
  //Vector3 debugTestDraw[4] = { Vector3{}, Vector3{}, Vector3{},Vector3{} };

	std::map<std::string, unsigned int> CollisionLayer;
	  
	Physics();
	~Physics();
	void Initialize(EntityList&);
	void Update();
	void SetUpCollisionBox();
	void UpdateCollision();
	void Clear(){}

  std::vector<Entity *> PhysicsGroupModel(std::vector<Vector4> & planes);

	Entity * PhysicsRayCastModel(const Vector3 & pos, const Vector3 & dir, float maxDist = FLT_MAX);
	Entity * PhysicsRayCast(const Vector3 & pos, const Vector3 & dir, float maxDist = FLT_MAX);
	Entity * PhysicsRayCastPlane(const Vector3 & pos, const Vector3 & dir, float & maxDist);
	Entity * PhysicsRayCastAABB(const Vector3 & pos, const Vector3 & dir, float & maxDist);
	Entity * PhysicsRayCastOBB(const Vector3 & pos, const Vector3 & dir, float & maxDist);
	Entity * PhysicsRayCastCapsule(const Vector3 & pos, const Vector3 & dir, float & maxDist);
	Entity * PhysicsRayCastSphere(const Vector3 & pos, const Vector3 & dir, float & maxDist);



  Entity * PhysicsRayCastContactData(const Vector3 & pos, const Vector3 & dir, float maxDist, Vector3 & pointOfContact);
  Entity * PhysicsRayCastPlaneContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact);
  Entity * PhysicsRayCastAABBContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact);
  Entity * PhysicsRayCastOBBContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact);
  Entity * PhysicsRayCastCapsuleContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact);
  Entity * PhysicsRayCastSphereContactData(const Vector3 & pos, const Vector3 & dir, float & maxDist, Vector3 & pointOfContact);

  //void SetLine(Vector3 start , Vector3 end)
  //{
  //  startLine = start;
  //  endLine = end;
  //  lineDraw = true;
  //}

  
  void PhysicsPause(bool pause);

};

