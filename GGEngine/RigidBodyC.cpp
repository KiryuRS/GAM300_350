#include "stdafx.h"
#include "Physics.h"

#pragma warning (disable : 4244)

//*--------------------------------------------------------------------------- PHYSICS STUFFS
RigidBodyC::RigidBodyC()
  : tranformObject{ nullptr },
  isKinematic{ false },
  constraintRotateX{ false },
  constraintRotateY{ false },
  constraintRotateZ{ false },
  mass{ 1.0f },
  //drag{ 0.0f },
  bounciness{ 1.0f },
  gravityStrength{ 1.0f },
  friction{ 0.f },
  totalForce{ 0,0,0 },
  rotationForce{ 0,0,0 },
  velocity{ 0,0,0 },
  acceleration{ 0,0,0 },
  rotationVec{ 0,0,0 },
  futurePosition{ 0,0,0 },
  Collider{ nullptr },
  gravity{ true },
  constraintPositionX{ false },
  constraintPositionY{ false },
  constraintPositionZ{ false },
  hasSpeedLimit{ false },
  speedLimit{ 0.f },
  secondLayerResolve{0,0,0}
{
  RequiredComponents(tranformObject);

}

void RigidBodyC::AddSerializeData(LuaScript* state)
{

  AddSerializable("isGravity", gravity, "", state);
  AddSerializable("GravityStrength", gravityStrength, "", state);

  AddSerializable("Mass", mass, "", state);
  AddSerializable("Bounciness", bounciness, "", state);
  AddSerializable("Friction", friction, "", state);

  AddSerializable("isKinematic", isKinematic, "", state);

  AddSerializable("Force", totalForce, "", state);
  AddSerializable("Velocity", velocity, "", state); /// basically try not to use this unless its going be static motion

  AddSerializable("constraintPositionX", constraintPositionX, "", state);
  AddSerializable("constraintPositionY", constraintPositionY, "", state);
  AddSerializable("constraintPositionZ", constraintPositionZ, "", state);

  AddSerializable("constraintRotateX", constraintRotateX, "", state);
  AddSerializable("constraintRotateY", constraintRotateY, "", state);
  AddSerializable("constraintRotateZ", constraintRotateZ, "", state);

  AddCallableFunction("AddForwardForce", &RigidBodyC::AddForwardForce, {}, state);
  AddCallableFunction("AddSideForce", &RigidBodyC::AddSideForce, {}, state);
  AddCallableFunction("AddUpForce", &RigidBodyC::AddUpForce, {}, state);
  AddCallableFunction("AddDirectionForce", &RigidBodyC::AddDirectionForce, {}, state);
  AddCallableFunction("AddImpulseForwardForce", &RigidBodyC::AddImpulseForwardForce, {}, state);
  AddCallableFunction("AddImpulseSideForce", &RigidBodyC::AddImpulseSideForce, {}, state);
  AddCallableFunction("AddImpulseUpForce", &RigidBodyC::AddImpulseUpForce, {}, state);
  AddCallableFunction("AddImpulseDirectionForce", &RigidBodyC::AddImpulseDirectionForce, {}, state);

  AddCallableFunction("SwitchGravity", &RigidBodyC::SwitchGravity, {}, state);
  AddCallableFunction("SetGravity", &RigidBodyC::SetGravity, {}, state);
  AddCallableFunction("SetMass", &RigidBodyC::SetMass, {}, state);
  AddCallableFunction("SetFriction", &RigidBodyC::SetFriction, {}, state);
  AddCallableFunction("SetBounciness", &RigidBodyC::SetBounciness, {}, state);
  AddCallableFunction("SetSpeedLimit", &RigidBodyC::SetSpeedLimit, {}, state);
  AddCallableFunction("SetVelocity", &RigidBodyC::SetVelocity, {}, state);
  AddCallableFunction("ResetAll", &RigidBodyC::ResetAll, {}, state);
  //AddCallableFunction("GetVelocity", &RigidBodyC::GetVelocity, {}, state);
  //SetVelocity
}



void RigidBodyC::Update(float dt)
{
  UNREFERENCED_PARAMETER(dt);
	if (hasSpeedLimit && GetSpeed() > speedLimit)
		SetSpeed(speedLimit);
}


void RigidBodyC::AddForwardForce(float force)
{


  Vector3 forwardVec = tranformObject->GetForwardVec();
  if (constraintPositionY)
	  forwardVec.y = 0;
  totalForce += forwardVec.Normalize() * force;
  //std::cout << std::endl;
  //for (auto elem : owner->prefabName)
	//  std::cout << elem;
  //std::cout << std::endl;
  //std::cout << totalForce << " : "  << forwardVec * force << " force " << force << std::endl;


}





void RigidBodyC::AddSideForce(float force)
{

  Vector3 sideVec = tranformObject->GetSideVec().Normalize();
  totalForce += sideVec * force;

}

void RigidBodyC::AddUpForce(float force)
{

  Vector3 upVec = tranformObject->GetUpVec().Normalize();
  totalForce += upVec * force;

}



void RigidBodyC::AddDirectionForce(Vector3 vec, float force)
{

  vec.Normalize();
  totalForce += vec * force;


}

void RigidBodyC::AddImpulseForwardForce(float force)
{


  Vector3 forwardVec = tranformObject->GetForwardVec().Normalize();


  Vector3 tmp = forwardVec * force / mass;
  acceleration += tmp;
  velocity += tmp;

}





void RigidBodyC::AddImpulseSideForce(float force)
{

  Vector3 sideVec = tranformObject->GetSideVec().Normalize();
  //velocity += sideVec * force / mass;
 

  Vector3 tmp = sideVec * force / mass;
  acceleration += tmp;
  velocity += tmp;
}

void RigidBodyC::AddImpulseUpForce(float force)
{
  Vector3 upVec = tranformObject->GetUpVec().Normalize();
  //velocity += upVec * force / mass;

  Vector3 tmp = upVec * force / mass;
  acceleration += tmp;
  velocity += tmp;

}

void RigidBodyC::AddImpulseDirectionForce(Vector3 vec, float force)
{

  vec.Normalize();
  Vector3 tmp = vec * force / mass;
  acceleration += tmp;
  velocity += tmp;
}



void RigidBodyC::PredictLocation(float dt)
{


  //float newDt = mass;

  Vector3 Normal{ 0,0,0 };
  Vector3 upVec{ 0,1.0f,0 };
  Vector3 cross2;
  Vector3 offSetVelo{ 0,0,0 };
  bool plane = false;
  bool frictionCheck = false;
  float tmpMass{ mass == 0 ? 0.01f : mass };
  if (gravity)
  {

    

    if(resolveData.empty())
      totalForce += Vector3{ 0,-gravityStrength * tmpMass,0 };
    else
    {
      frictionCheck = true;

      for (auto & elem : resolveData)
      {
        if (elem.Vec.y >= 0)
        {
          Vector3 tmp = elem.Vec;
          if (elem.Vec == Vector3{ 0,0,0 })
            continue;

          tmp.Normalize();

          if (tmp.y == 1.0f)
          {
            plane = true;
            break;
          }

          Normal += tmp;

        }
      }
      
      if (!plane && Normal != Vector3{ 0,0,0 })
      {
        Normal.Normalize();
        //std::cout << "N :" << Normal << std::endl;

        Vector3 cross1{ upVec.Cross(Normal) };
        cross2 = cross1.Cross(Normal);



        cross2 = (cross2 * (upVec * -gravityStrength))* cross2;
        totalForce += cross2;

        //Vector3 offVel = cross2.Normalize();
        //offVel = offVel * velocity*offVel;
        //offSetVelo.y += offVel * Vector3{ 0,1.0f,0 };
        //std::cout << offSetVelo.y << " yyyyy" << std::endl;
        //cross2.

      }
      

    }
    
  }

  acceleration = totalForce / tmpMass;
  velocity += acceleration * dt *0.99f;
  if (constraintPositionX)
	  velocity.x = 0;
  if (constraintPositionY)
	  velocity.y = 0;
  if (constraintPositionZ)
	  velocity.z = 0;
  futurePosition = tranformObject->GetGlobalPosition() + (velocity*dt);

  LineSegment line{ tranformObject->GetGlobalPosition(), futurePosition+ velocity };
  DebugShape& tmp = DEBUGDRAW_S.DrawLine(line);
  tmp.color.SetX(0.2f);
  tmp.color.SetY(0.8f);

  if (frictionCheck)
  {
    if (plane)
    {
      if (velocity != Vector3{ 0,0,0 })
      {
        Vector3 frictionVec{ velocity };
        frictionVec.Normalize();
        frictionVec = frictionVec * (-gravityStrength / tmpMass * friction * dt  * dt * 0.99f);
        if (frictionVec.Length() > velocity.Length())
          velocity = Vector3{ 0,0,0 };
        else
          velocity += frictionVec;
      }
    }
    else if (Normal != Vector3{ 0,0,0 })
    { 
      
      Normal.Normalize();
      Normal = (Normal * (upVec * -gravityStrength))* Normal;
    
      Vector3 velo = velocity;
      //if(velocity * cross2 > 0)
      //  cross2 = -cross2.Normalize();
      velo = -velo.Normalize();
      velo = velo *  (friction  * dt * dt * 0.99f *Normal.Length() / tmpMass);
      if (velo.Length() <= velocity.Length())
        velocity += velo;
      else
        velocity = Vector3{ 0,0,0 };
        
    }
    
  }

  resolveData.clear();
}



void RigidBodyC::SetLocation()
{

  if (!resolveData.empty())
  {

    Vector3 resPos = futurePosition;
    Vector3 resNormal;

    Vector3 oldPos = tranformObject->GetPosition();
    Vector3 newVec{ 0,0,0 };
    Vector3 push{ 0,0,0 };
    float tmpMass = mass == 0 ? 1 : mass;


    for (auto & elem : resolveData)
    {

      // set bounciness
      resPos += elem.Pos - futurePosition;
      //if (velocity * elem.Vec == 0)
      //  continue;
      Vector3 resolveVec{ 0,0,0 };
	  elem.Vec = elem.Vec.Normalize();
      if (velocity * elem.Vec <= 0)
      {
		  
        Vector3 tmpVec{ velocity - 2.0f *(elem.Vec * velocity)* elem.Vec };
        Vector3 downVec = elem.Vec;
        if (downVec == Vector3{ 0,0,0 })
          continue;
        downVec.Normalize();

        Vector3 reflectVec{ (tmpVec*elem.Vec)*bounciness*elem.Vec };

        if (reflectVec.Length() < gravityStrength / ReflectEpsilon)
          reflectVec.y = 0;


        if (tmpVec* elem.Vec >= 0)
          resolveVec = tmpVec - (tmpVec*downVec)*downVec + reflectVec;
        else
          resolveVec = tmpVec + reflectVec * 0.01f;
      }

      if (elem.rhs)
      {
        push += elem.ModifyForce;
        elem.Vec = resolveVec / 2.0f;
        resolveVec += elem.ModifyForce/ tmpMass;
        resolveVec /= 2;
      }

      newVec += resolveVec;
    }

    newVec /= static_cast<float>(resolveData.size());

    if (push == Vector3{ 0,0,0 })
    {
      for (auto & elem : resolveData)
      {
        if(elem.rhs)
          elem.rhs->secondLayerResolve -= (elem.ModifyForce / (2.0f* tmpMass) + elem.Vec) * 2.0f;
      }
    }

    // convert force into velocity in the frame of the impact
    if (mass != 0 && push != Vector3{ 0,0,0 })
      push /= mass;
    velocity = newVec;


    futurePosition = resPos;

  }

  // to deal with impulse
  if (totalForce == Vector3{ 0,0,0 } && velocity != Vector3{ 0,0,0 })
  {
    acceleration = totalForce;
  }

  totalForce = Vector3{ 0,0,0 };

  tranformObject->SetGlobalPosition(futurePosition);


}

void RigidBodyC::SecondLayerResolving()
{
  //if (!resolveData.empty())
  //{

    velocity += secondLayerResolve;
    secondLayerResolve = Vector3{ 0,0,0 };
  //}

}

void RigidBodyC::Initialize()
{
  //AddRequiredComponents(tranformObject);

  //if (tranformObject != nullptr)
  float tmpMass{ mass == 0 ? 0.01f : mass };
  velocity += totalForce / tmpMass;
  totalForce = Vector3{ 0,0,0 };

  resolveData.clear();
}






