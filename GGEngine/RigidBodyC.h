#pragma once

struct CollisionData
{
  Vector3 Pos;
  Vector3 Vec;
  Vector3 ModifyForce;
  RigidBodyC * rhs;

  CollisionData(const Vector3 & nPos, const Vector3 & nVec, RigidBodyC * rh = nullptr, const Vector3 & mForce = Vector3{ 0,0,0 })
  : Pos{ nPos }, 
    Vec{ nVec },
    rhs{rh},
    ModifyForce{ mForce }
  {}
};


class RigidBodyC :
  public Component
{
  friend class ColliderPlane;
  friend class ColliderAABB;
  friend class ColliderOBB;
  friend class ColliderSphere;
  friend class ColliderCapsule;

  bool isKinematic;
  bool constraintRotateX;
  bool constraintRotateY;
  bool constraintRotateZ;
  bool hasSpeedLimit;

  float mass;
  //float drag;
  float bounciness;
  float gravityStrength;
  float friction;

  float MaxSpeed{ FLT_MAX };
  float speedLimit;

  Vector3 totalForce;
  Vector3 rotationForce;
  Vector3 velocity;
  Vector3 acceleration;
  Vector3 rotationVec;

  ColliderC* Collider;
  TransformC* tranformObject;
  bool gravity;
  bool constraintPositionX;
  bool constraintPositionY;
  bool constraintPositionZ;


  std::vector<CollisionData> resolveData;
  Vector3 secondLayerResolve;


public:
  Vector3 futurePosition;


  RigidBodyC();
  void AddSerializeData(LuaScript* state = nullptr) override;
  //
  //void addDisplacementForce();

  void Update(float dt);
  void SecondLayerResolving();
  void AddForwardForce(float force);
  void AddSideForce(float force);
  void AddUpForce(float force);
  void AddDirectionForce(Vector3 vec, float force);
  void AddImpulseForwardForce(float force);
  void AddImpulseSideForce(float force);
  void AddImpulseUpForce(float force);
  void AddImpulseDirectionForce(Vector3 vec, float force);

  void SetCollisionData(const Vector3 & nPos, const Vector3 & nVec, RigidBodyC * rhs = nullptr, const Vector3 & nForce = Vector3{ 0,0,0 })
  {
    resolveData.emplace_back(nPos, nVec, rhs, nForce);
  }

  

  void SwitchGravity(bool state)
  {
    gravity = state;
  }

  void SetGravity(float gravityForce)
  {
    gravityStrength = gravityForce;
    gravity = true;
  }

  void SetMass(float objMass)
  {
    mass = objMass;
  }

  void SetFriction(float frictionStrength)
  {
    friction = frictionStrength;
  }

  void SetBounciness(float bouncy)
  {
    bounciness = bouncy;
  }

  void SetAcceleration(const Vector3 acc = Vector3{ 0,0,0 })
  {
    acceleration = acc;
  }

  void SetVelocity(const Vector3 speed)
  {
    velocity = speed;
  }

  void ResetAll()
  {
	  velocity = Vector3{ 0,0,0 };
	  totalForce = Vector3{ 0,0,0 };
	  acceleration = Vector3{ 0,0,0 };
  }


  void SetSpeed(float speed)
  {
    
    Vector3 tmpLocal = velocity;
    //Vector3 tmpUp = tranformObject->GetUpVec();
    //tmpUp.Normalize();
    //tmpUp = (velocity * tmpUp)*tmpUp;
    //tmpLocal -= tmpUp;
    Vector3 tmpUp{ 0,tmpLocal.y,0 };
    tmpLocal.y = 0;
    if(tmpLocal != Vector3{0,0,0})
      tmpLocal.Normalize();

    velocity = (tmpLocal * speed) + tmpUp;
    
  }
  float GetMass() const
  {
    return mass;
  }
  Vector3 GetVelocity()
  {
    return velocity;
  }

  float GetSpeed()
  {
    return velocity.Length();
  }

  float GetSpeed() const
  {

    Vector3 tmpLocal = velocity;
    //Vector3 tmpUp = Vector3{ 0,1.0f,0 };
    //
    //tmpUp = (velocity * tmpUp)*tmpUp;
    tmpLocal.y = 0;

    return tmpLocal.Length();
  }



  void PredictLocation(float dt);

  void SetLocation();

  static constexpr auto GetType() { return COMPONENTTYPE::RIGIDBODY; }

  void Initialize() override;

  void PauseUpdate()
  {
    futurePosition = tranformObject->GetGlobalPosition();
  };

  void SetSpeedLimit(float limit)
  {
	  speedLimit = limit;
	  hasSpeedLimit = true;
  }

};


