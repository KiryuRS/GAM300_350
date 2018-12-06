#pragma once


/*
enum ColliderType : char
{
AABB = 0,
OBB = 1,
Sphere = 2,
Plane = 3
};
*/


enum CollisionFace : char
{
  NoneCol = 0,
  forwardCol,
  upCol,
  sideCol,
  negForwardCol,
  negUpCol,
  negSideCol
};


class RigidBodyC;
class ColliderC;
class ColliderPlane;
class ColliderAABB;
class ColliderOBB;
class ColliderSphere;
class ColliderCapsule;

class ColliderC :
  public Component
{



public:
	std::unordered_map<Entity*, std::weak_ptr<char>> proxies;
	bool debugDraw;
  std::vector<ColliderC *> preCollisionData;

  Vector3 offset;
  TransformC* tranformObject;
  RigidBodyC* rigidBody;
  float collisionTime;
  std::vector<ColliderC *> newCollisionData;
  bool isTrigger; // mainly to inform not to resolve


  // colliderTag
  int ColliderTag = 1;

  ColliderC();

  void CollisionCall();

  virtual void AddSerializeData(LuaScript* state = nullptr);

  void ResetCollisionTime();

  bool EditorGUI(Editor& edit) override;
};

class ColliderPlane :
  public ColliderC
{




public:
  Vector3 scale;
  Vector3 centre;
  Vector3 faceNormals[3];
  Vector3 extends;

  ColliderPlane();
  void UpdateCollider();
  bool CheckCollision(ColliderPlane* rhs);
  bool CheckCollision(ColliderAABB* rhs);
  bool CheckCollision(ColliderOBB* rhs);
  bool CheckCollision(ColliderSphere* rhs);
  bool CheckCollision(ColliderCapsule* rhs);
  void AddSerializeData(LuaScript* state = nullptr) override;

  static constexpr auto GetType() { return COMPONENTTYPE::PLANECOLLIDER; }

  void Initialize() override;
  void EditorUpdate(float);
  void Update(float dt)
  {
    EditorUpdate(dt);
  }
};

class ColliderAABB :
  public ColliderC
{


public:
  Vector3 scale;
  Vector3 centre;
  Vector3 faceNormals[3];
  Vector3 extends;
  bool rotationAffect_90;

  ColliderAABB();
  void UpdateCollider();
  bool CheckCollision(ColliderPlane* rhs);
  bool CheckCollision(ColliderAABB* rhs);
  bool CheckCollision(ColliderOBB* rhs);
  bool CheckCollision(ColliderSphere* rhs);
  bool CheckCollision(ColliderCapsule* rhs);
  void AddSerializeData(LuaScript* state = nullptr) override;

  static constexpr auto GetType() { return COMPONENTTYPE::AABBCOLLIDER; }

  void Initialize() override;
  void EditorUpdate(float);
  void Update(float dt)
  {
    EditorUpdate(dt);
  }
  void SetTrigger(bool b) { isTrigger = b; }
};

class ColliderOBB :
  public ColliderC
{

public:
  Vector3 scale;
  Vector3 centre;
  Vector3 faceNormals[3];
  Vector3 extends;



  ColliderOBB();
  void UpdateCollider();
  bool CheckCollision(ColliderPlane* rhs);
  bool CheckCollision(ColliderAABB* rhs);
  bool CheckCollision(ColliderOBB* rhs);
  bool CheckCollision(ColliderSphere* rhs);
  bool CheckCollision(ColliderCapsule* rhs);
  void AddSerializeData(LuaScript* state = nullptr) override;

  static constexpr auto GetType() { return COMPONENTTYPE::OBBCOLLIDER; }

  void Initialize() override;
};

class ColliderSphere :
  public ColliderC
{



public:
  float radius;
  Vector3 centre;

  ColliderSphere();
  void UpdateCollider();
  bool CheckCollision(ColliderPlane* rhs);
  bool CheckCollision(ColliderAABB* rhs);
  bool CheckCollision(ColliderOBB* rhs);
  bool CheckCollision(ColliderSphere* rhs);
  bool CheckCollision(ColliderCapsule* rhs);
  void AddSerializeData(LuaScript* state = nullptr) override;
  void EditorUpdate(float);
  void Update(float dt)
  {
    EditorUpdate(dt);
  }

  static constexpr auto GetType() { return COMPONENTTYPE::SPHERECOLLIDER; }

  void Initialize() override;

};

class ColliderCapsule :
  public ColliderC
{




public:
  float height;
  float radius;
  Vector3 centre;

  ColliderCapsule();
  void UpdateCollider();
  bool CheckCollision(ColliderPlane* rhs);
  bool CheckCollision(ColliderAABB* rhs);
  bool CheckCollision(ColliderOBB* rhs);
  bool CheckCollision(ColliderSphere* rhs);
  bool CheckCollision(ColliderCapsule* rhs);
  void AddSerializeData(LuaScript* state = nullptr) override;

  static constexpr auto GetType() { return COMPONENTTYPE::CAPSULECOLLIDER; }

  void Initialize() override;
  void EditorUpdate(float);
  void Update(float dt)
  {
    EditorUpdate(dt);
  }
};
