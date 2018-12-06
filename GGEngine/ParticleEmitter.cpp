#include "stdafx.h"
#include "ParticleEmitter.h"

#pragma warning (disable : 4244)
#pragma warning (disable : 4100)

#define TWO_PI 6.28318f
#define MaxParticle 1024

static float RandomRangeFloat()
{
  return static_cast <float> (std::rand()) / (static_cast <float> (RAND_MAX) / 2.0f) - 1.0f;
}

static float RandomFloat()
{
  return static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX);
}

ParticleEmitterC::ParticleEmitterC()
  :
  /*tranformObject{ nullptr },*/
  base{*this},
  offSetPosition{0,0,0},
  counter{ 0 },
  warmthUpTime{ 0 },
  lifeTime{ 3.0f },
  totalParticles{16},
  emitRate{ 0.5f },
  particlesPerEmit{ 1 },
  gravityStrength{ 0 },
  emiting{ true },
  rangeVelocity{ false },
  emitStyle{ defaultParticle },
  velocityStrength{ 5.0f },
  velocityStrength_2{ 0 },
  emitterTimer{0},
  initialOpacity{1.0f},
  rangeInitialDist{false},
  initialDistance{0},
  initialDistance_2{0},
  rangeSize{ false },
  size{ 0.1f},
  size_2{ 1.0f },
  growth{0},
  directVector{0,0,1.0f},
  degree{0},
  particlesPerCyclic{60},
  lookAtCam{false},
  curCyclic{0},
	directionRotation{false}
{
  //RequiredComponents(tranformObject);
	base.updateModelMinMax = false;
	glGenBuffers(1, &particles_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, particles_VBO);
	glBufferData(GL_ARRAY_BUFFER, MaxParticle * sizeof(ParticleGL), NULL, GL_STREAM_DRAW);
}

/// do warm up if needed
void ParticleEmitterC::Initialize()
{
  particlesVel.reserve(static_cast<unsigned>(ceilf(totalParticles)));
  particlesVel.assign(static_cast<unsigned>(ceilf(totalParticles)),Vector3{});
  GL_particles.reserve(static_cast<unsigned>(ceilf(totalParticles)));
  GL_particles.assign(static_cast<unsigned>(ceilf(totalParticles)), ParticleGL{});


  if (emitOnceStart)
  {
	  EmitOnce();
  }


  objectPos = base.transform->GetGlobalPosition();
}

void ParticleEmitterC::DestroyUpdate()
{
  glDeleteBuffers(1, &particles_VBO);
}

void ParticleEmitterC::AddSerializeData(LuaScript* state)
{
  base.AddSerializeData(state);
  AddSerializable("offSetPosition", offSetPosition, "", state);
  AddSerializable("warmthUpTime", warmthUpTime, "", state).display = false;
  AddSerializable("lifeTime", lifeTime, "", state).display = false;
  AddSerializable("totalParticles", totalParticles, "", state).display = false;
  AddSerializable("emitRate", emitRate, "", state).display = false;
  AddSerializable("particlesPerEmit", particlesPerEmit, "", state).display = false;
  AddSerializable("gravityStrength", gravityStrength, "", state).display = false;
  AddSerializable("emiting", emiting, "", state).display = false;
  AddSerializable("Emissive_Particle", emissive_render, "", state);
  AddSerializable("rangeVelocity", rangeVelocity, "", state).display = false;
  AddSerializable("velocityStrength", velocityStrength, "", state).display = false;
  AddSerializable("velocityStrength_2", velocityStrength_2, "", state).display = false;
  AddSerializable("rangeSize", rangeSize, "", state).display = false;
  AddSerializable("size", size, "", state).display = false;
  AddSerializable("size_2", size_2, "", state).display = false;
  AddSerializable("growth", growth, "", state).display = false;
  AddSerializable("emitStyle", emitStyle, "", state).display = false;
  AddSerializable("directionVector", directVector, "", state).display = false;
  AddSerializable("degree", degree, "", state).display = false;
  AddSerializable("particlesPerCyclic", particlesPerCyclic, "", state).display = false;
  AddSerializable("lookAtCam", lookAtCam, "", state).display = false;
  AddSerializable("emitOnceStart", emitOnceStart, "", state).display = false;
  AddSerializable("directionVector", directVector, "", state).display = false;
  AddSerializable("initialOpacity", initialOpacity, "", state).display = false;
  AddSerializable("rangeInitialDist", rangeInitialDist, "", state).display = false;
  AddSerializable("initialDistance", initialDistance, "", state).display = false;
  AddSerializable("initialDistance_2", initialDistance_2, "", state).display = false;
  AddSerializable("Texture", mTexture, "", state).display = false;
  AddSerializable("randomRotation", randomRotation, "", state).display = false;
  AddSerializable("directionRotation", directionRotation, "", state).display = false;
  AddSerializable("randomDistance", randomDistance, "", state).display;
  
  AddCallableFunction("EmitOnce", &ParticleEmitterC::EmitOnce, {}, state);
  AddCallableFunction("Reset", &ParticleEmitterC::Reset, {}, state);

}

bool ParticleEmitterC::EditorGUI(Editor&)
{
	if (ImGui::InputFloat("TotalParticles", &totalParticles))
	{
		if (totalParticles > MaxParticle)
			totalParticles = MaxParticle;
		if (totalParticles < 1)
			totalParticles = 1;
		UpdateSerializables(totalParticles);
		particlesVel.clear();
		GL_particles.clear();
		particlesVel.reserve(static_cast<unsigned>(ceilf(totalParticles)));
		particlesVel.assign(static_cast<unsigned>(ceilf(totalParticles)), Vector3{});
		GL_particles.reserve(static_cast<unsigned>(ceilf(totalParticles)));
		GL_particles.assign(static_cast<unsigned>(ceilf(totalParticles)), ParticleGL{});
	}
	if(ImGui::InputInt("PerEmit", &particlesPerEmit))
		UpdateSerializables(particlesPerEmit);
	if(ImGui::InputFloat("EmissiveRate", &emitRate))
		UpdateSerializables(emitRate);
	if (ImGui::InputFloat("LifeTime", &lifeTime))
		UpdateSerializables(lifeTime);

	ImGui::Text("Initial_Alpha");
	if(ImGui::SliderFloat("", &initialOpacity,0,1.0f))
		UpdateSerializables(initialOpacity);
	ImGui::Separator();



	std::string styleMode[5] = { "Default_Style", "Radial_Style", "Cone_Style", "line_Spawn", "trailing_Particle_Spawn" };
	std::string tmp = "Current Style : " + styleMode[emitStyle];
	ImGui::Text(tmp.c_str());
	//TOOLS::PushID(UITYPE::COLLAPSINGHEADER);
	
	for (int i = 0; i < 5; i++)
	{
		if (ImGui::Button(styleMode[i].c_str()))
		{
			emitStyle = i;
			UpdateSerializables(emitStyle);
		}
	}

	//ImGui::PopID();

	ImGui::Separator();

	if (emitStyle == directionParticle || emitStyle == cyclicParticle)
	{
		float dir[3] = { directVector.x, directVector.y, directVector.z };

		if (ImGui::SliderFloat3("DirectVec", dir, -1.0f, 1.0f, "%.3f", 1.f))
		{
			directVector = dir;
			UpdateSerializables(directVector);
		}
		Vector3 objPos{ base.transform->GetGlobalPosition() + offSetPosition };
		Vector3 tmpVec = directVector;
		LineSegment lineDrawer{ objPos, objPos+ tmpVec*20.0f};
		auto& line_draw = DEBUGDRAW_S.DrawLine(lineDrawer);
		line_draw.color.x = 0.6f;
		line_draw.color.y = 0.4f;
		line_draw.color.z = 0.4f;
		
		if (ImGui::SliderFloat("Degree", &degree, 0, 180.f, "%.3f", 1.f))
		{
			UpdateSerializables(degree);
		}

		if (emitStyle == 2)
		{
			if (ImGui::InputInt("ParticlesPerCycle", &particlesPerCyclic))
			{
				UpdateSerializables(particlesPerCyclic);
			}
		}

	}

	ImGui::Separator();

	if (ImGui::Checkbox("LookAtCam", &lookAtCam, true))
	{
		UpdateSerializables(lookAtCam);
		if (lookAtCam)
		{
			randomRotation = false;
			UpdateSerializables(randomRotation);
			directionRotation = false;
			UpdateSerializables(directionRotation);
		}
	}

	if (ImGui::Checkbox("randomRotation", &randomRotation, true))
	{
		UpdateSerializables(randomRotation);
		if (randomRotation)
		{
			lookAtCam = false;
			UpdateSerializables(lookAtCam);
			directionRotation = false;
			UpdateSerializables(directionRotation);
		}
	}

	if (ImGui::Checkbox("directionRotation", &directionRotation, true))
	{
		UpdateSerializables(directionRotation);
		if (directionRotation)
		{
			lookAtCam = false;
			UpdateSerializables(lookAtCam);
			randomRotation = false;
			UpdateSerializables(randomRotation);
		}
	}


	if (ImGui::Checkbox("Emitting", &emiting, true))
	{
		UpdateSerializables(emiting);
	}

	if (ImGui::Checkbox("EmitAtStart", &emitOnceStart, true))
	{
		UpdateSerializables(emitOnceStart);
	}


	if (ImGui::InputFloat("GravityStrength", &gravityStrength))
		UpdateSerializables(gravityStrength);

	bool singleMode = !rangeVelocity;

	if (ImGui::Checkbox("SingleVelocity", &singleMode, true))
	{
		rangeVelocity = !singleMode;
		UpdateSerializables(rangeVelocity);
	}


	if (ImGui::InputFloat("Velocity_1", &velocityStrength))
		UpdateSerializables(velocityStrength);
	
	if (rangeVelocity)
	{
		if (ImGui::InputFloat("Velocity_2", &velocityStrength_2))
			UpdateSerializables(velocityStrength_2);
	}

	singleMode = !rangeInitialDist;

	if (ImGui::Checkbox("SingleStartDist", &singleMode, true))
	{
		rangeInitialDist = !singleMode;
		UpdateSerializables(rangeInitialDist);
	}

	if (ImGui::InputFloat("Dist_1", &initialDistance))
		UpdateSerializables(initialDistance);

	if (rangeInitialDist)
	{
		if (ImGui::InputFloat("Dist_2", &initialDistance_2))
			UpdateSerializables(initialDistance_2);
	}
	
	singleMode = !rangeSize;

	if (ImGui::Checkbox("SingleSize", &singleMode, true))
	{
		rangeSize = !singleMode;
		UpdateSerializables(rangeSize);
	}

	if (ImGui::InputFloat("Size_1", &size))
		UpdateSerializables(size);

	if (rangeSize)
	{
		if (ImGui::InputFloat("Size_2", &size_2))
			UpdateSerializables(size_2);
	}
	
	if (ImGui::InputFloat("Growth", &growth))
		UpdateSerializables(growth);

  if (static_cast<unsigned>(ceilf(totalParticles)) != particlesVel.size())
  {
    particlesVel.clear();
    GL_particles.clear();
    particlesVel.reserve(static_cast<unsigned>(ceilf(totalParticles)));
    particlesVel.assign(static_cast<unsigned>(ceilf(totalParticles)), Vector3{});
    GL_particles.reserve(static_cast<unsigned>(ceilf(totalParticles)));
    GL_particles.assign(static_cast<unsigned>(ceilf(totalParticles)), ParticleGL{});
  }

  base.ChooseModelGUI();
  base.ChooseMaterialGUI();

  if (ImGui::Button("Emit Once"))
  {
    EmitOnce();
  }
  if (ImGui::Button("Reset "))
  {
    Reset();
  }

  ChooseTextureGUI();

  return false;
}

void ParticleEmitterC::EditorSetup()
{
  Initialize();
  base.Setup();
}

void ParticleEmitterC::EditorUpdate(float dt)
{
  base.UpdateTransformMinMax();
  Update(dt);
}

void ParticleEmitterC::EmitOnce()
{
  if (totalParticles != particlesVel.size())
  {
    particlesVel.clear();
    GL_particles.clear();
    particlesVel.reserve(static_cast<unsigned>(ceilf(totalParticles)));
    particlesVel.assign(static_cast<unsigned>(ceilf(totalParticles)), Vector3{});
    GL_particles.reserve(static_cast<unsigned>(ceilf(totalParticles)));
    GL_particles.assign(static_cast<unsigned>(ceilf(totalParticles)), ParticleGL{});
  }

  if (emitStyle == trailingParticle)
  {
	  return;
  }

  Vector3 objPos{ offSetPosition };
  int i = counter;
  for (; i < counter + particlesPerEmit; i++)
  {

	  Vector3 randomloc;
	  randomloc.x = TOOLS::RandFloat(-randomDistance.x, randomDistance.x);
	  randomloc.y = TOOLS::RandFloat(-randomDistance.y, randomDistance.y);
	  randomloc.z = TOOLS::RandFloat(-randomDistance.z, randomDistance.z);
	  objPos += randomloc;
    if (i >= totalParticles)
      break;

	if (randomRotation)
	{
		GL_particles[i].rotation = Vector3{ RandomFloat() * 360.f, RandomFloat() * 360.f, RandomFloat() * 360.f };

	}
	else
		GL_particles[i].rotation = Vector3{ 0,0,0 };
    GL_particles[i].Percentage = 0;
    GL_particles[i].init_alpha = initialOpacity;
    Vector3 directVec{ Vector3{ 0,1,0 } };
    Vector3 rotateVec{ Vector3{ 0,1,0 } };

    bool directionTrans = false;
    Vector3 transSpace[3]{ directVector,{1.0f,0,0},{0,0,1.0f} };
    Vector3 rosSpace[3]{ directVector,{1.0f,0,0},{0,0,1.0f} };

	Vector3 tmpDirectVector = directVector;
	tmpDirectVector = tmpDirectVector.Normalize();

    float rev{ 360.0f / particlesPerCyclic };

    int styleMode = emitStyle;
    if (directVector == Vector3{ 0,0,0 })
      styleMode = defaultParticle;
    else
    {
      if (degree != 0)
      {
        if (tmpDirectVector != Vector3{ 0,1,0 })
        {
          directionTrans = true;

          transSpace[1] = tmpDirectVector.Cross(Vector3{ 0,1,0 }).Normalize();
          transSpace[2] = transSpace[1].Cross(transSpace[0]);

        }
        directVec = Matrix3x3::RotateZ(DegreePI*degree) * directVec;
      }
    }
    switch (styleMode)
    {
    case defaultParticle:
      //Vector3 directVec{}
      do
      {
        directVec.x = RandomRangeFloat();
        directVec.y = RandomRangeFloat();
        directVec.z = RandomRangeFloat();
      } while (directVec == Vector3{ 0,0,0 });

      directVec.Normalize();

	  if (directionRotation)
	  {
		  lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, directVec, Vector3{ 0,1.0f,0 });

		  Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
		  GL_particles[i].rotation *= 360.f;
	  }

      if (rangeVelocity)
      {
        particlesVel[i] = directVec * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
      }
      else
        particlesVel[i] = directVec * velocityStrength;

      if (rangeSize)
      {
        GL_particles[i].initSize = (size_2 - size) * RandomFloat() + size;
        GL_particles[i].finalSize = (size_2 - size) * RandomFloat() + size + growth;
      }
      else
      {
        GL_particles[i].initSize = size;
        GL_particles[i].finalSize = size + growth;
      }

	  if (rangeInitialDist)
      GL_particles[i].pos = objPos + directVec * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
	  else
      GL_particles[i].pos = objPos + directVec * initialDistance;

      break;
    case directionParticle:
      if (degree != 0)
      {
        rotateVec = Matrix3x3::RotateY(360 * DegreePI * RandomFloat())* directVec;

        rotateVec -= Vector3{ 0,1,0 };

        rotateVec = Vector3{ 0,1,0 } +RandomFloat() * rotateVec;

        rosSpace[0] = transSpace[0] * rotateVec.y;
        rosSpace[1] = transSpace[1] * rotateVec.x;
        rosSpace[2] = transSpace[2] * rotateVec.z;

        rotateVec = (rosSpace[0] + rosSpace[1] + rosSpace[2]).Normalize();

		if (directionRotation)
		{
			lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, rotateVec, Vector3{ 0,1.0f,0 });

			Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
			GL_particles[i].rotation *= 360.f;
		}

        if (rangeVelocity)
        {
          particlesVel[i] = rotateVec * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
        }
        else
          particlesVel[i] = rotateVec * velocityStrength;

		if (rangeInitialDist)
      GL_particles[i].pos = objPos + rotateVec * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
		else
      GL_particles[i].pos = objPos + rotateVec * initialDistance;
      }
      else
      {
		  if (directionRotation)
		  {
			  lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, tmpDirectVector, Vector3{ 0,1.0f,0 });

			  Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
			  GL_particles[i].rotation *= 360.f;
		  }

        if (rangeVelocity)
        {
          particlesVel[i] = tmpDirectVector * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
        }
        else
          particlesVel[i] = tmpDirectVector * velocityStrength;


		if (rangeInitialDist)
      GL_particles[i].pos = objPos + tmpDirectVector * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
		else
      GL_particles[i].pos = objPos + tmpDirectVector * initialDistance;
      }


      if (rangeSize)
      {
        GL_particles[i].initSize = (size_2 - size) * RandomFloat() + size;
        GL_particles[i].finalSize = (size_2 - size) * RandomFloat() + size + growth;
      }
      else
      {
        GL_particles[i].initSize = size;
        GL_particles[i].finalSize = size + growth;
      }

	  

      break;
    case cyclicParticle:
      if (degree != 0)
      {
        curCyclic += rev;
        while (curCyclic > 360.0f)
          curCyclic -= 360.0f;


        rotateVec = Matrix3x3::RotateY(curCyclic * DegreePI)* directVec;

        //rotateVec -= Vector3{ 0,1,0 };
        //
        //rotateVec = Vector3{ 0,1,0 } +RandomFloat() * rotateVec;

        rosSpace[0] = transSpace[0] * rotateVec.y;
        rosSpace[1] = transSpace[1] * rotateVec.x;
        rosSpace[2] = transSpace[2] * rotateVec.z;

        rotateVec = (rosSpace[0] + rosSpace[1] + rosSpace[2]).Normalize();

		if (directionRotation)
		{
			lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, rotateVec, Vector3{ 0,1.0f,0 });

			Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
			GL_particles[i].rotation *= 360.f;
		}

        if (rangeVelocity)
        {
          particlesVel[i] = rotateVec * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
        }
        else
          particlesVel[i] = rotateVec * velocityStrength;

		if (rangeInitialDist)
      GL_particles[i].pos = objPos + rotateVec * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
		else
      GL_particles[i].pos = objPos + rotateVec * initialDistance;
      }
      else
      {
		  if (directionRotation)
		  {
			  lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, tmpDirectVector, Vector3{ 0,1.0f,0 });

			  Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
			  GL_particles[i].rotation *= 360.f;
		  }

        if (rangeVelocity)
        {
          particlesVel[i] = tmpDirectVector * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
        }
        else
          particlesVel[i] = tmpDirectVector * velocityStrength;

		if (rangeInitialDist)
      GL_particles[i].pos = objPos + tmpDirectVector * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
		else
      GL_particles[i].pos = objPos + tmpDirectVector * initialDistance;

      }


      if (rangeSize)
      {
        GL_particles[i].initSize = (size_2 - size) * RandomFloat() + size;
        GL_particles[i].finalSize = (size_2 - size) * RandomFloat() + size + growth;
      }
      else
      {
        GL_particles[i].initSize = size;
        GL_particles[i].finalSize = size + growth;
      }
	  
      break;
	case lineSpawnParticle:
		break;
	case trailingParticle:

		break;
    }

	objPos -= randomloc;
  }
  counter = i;
}

void ParticleEmitterC::ParticlesUpdater(float dt)
{
  /******************************************************************************************/
	base.transform->UpdateTransformationMatrix();
	Vector3 currentLoc = base.transform->GetGlobalPosition();
	Vector3 fixVec = currentLoc - objectPos;
  int shift = 0;
  for (int itr = 0; itr < counter; itr++)
  {
    
      

    GL_particles[itr].Percentage += dt/lifeTime;

    if (GL_particles[itr].Percentage >= 1)
    {
      shift += particlesPerEmit;
      itr += particlesPerEmit;
      continue;
    }

    particlesVel[itr].y -= gravityStrength * dt;
	if(emitStyle != trailingParticle)
		GL_particles[itr].pos += particlesVel[itr] * dt;
	else
	{
		
		GL_particles[itr].pos += particlesVel[itr] * dt - fixVec;
	}

  }

  int workingSpace = counter - shift;

  if (workingSpace >= 0)
  {

    
    std::memcpy(&particlesVel[0], &particlesVel[shift], workingSpace * sizeof(Vector3));
    std::memcpy(&GL_particles[0], &GL_particles[shift], workingSpace * sizeof(ParticleGL));

	counter = workingSpace;
  }
  else
	counter = 0;
}

void ParticleEmitterC::Reset()
{
  counter = 0;
  return;
}

void ParticleEmitterC::Render()
{
	if (counter == 0)
		return;
  ParticleDetails md;
  md.model = base.modelDirectory;
  md.texture = mTexture;
  md.emissive = this->emissive_render;
  md.particles.resize(static_cast<unsigned>(counter));
  memcpy(&md.particles[0].pos.x, &GL_particles[0].pos.x, counter * sizeof(ParticleGL));
  Vector3 tmp{};
  if (lookAtCam)
  {
	  lookAtM = Matrix4x4::LookAtRm(base.transform->GetGlobalPosition(), owner->entityList->GetActiveCamera()->GetCameraPosition(), Vector3{ 0,1.0f,0 });

	  Matrix4x4::GetRotationAngle(lookAtM, tmp.x, tmp.y, tmp.z);
	  //tmp *= DegreePI;
	  md.transformation = Matrix4x4::Translate(base.transform->GetGlobalPosition()) * Matrix4x4 { Matrix3x3::GetQuatMatrix(Vector4::QuaternionEuler(tmp)) };
  }
  else
	md.transformation = Matrix4x4::Translate(base.transform->GetGlobalPosition());
  
  //if (lookAtCam)
  //{
  //  md.transformation = Matrix4x4::Translate(particlesPos[itr]) *
  //    (Matrix4x4{ Matrix3x3::GetQuatMatrix(Vector4::QuaternionEuler(tmp)) } *Matrix4x4::Scale((1.0f - particlesData[itr].life / lifeTime) * growth + particlesData[itr].InitSize));
  //
  //}
  //else
  //  md.transformation = Matrix4x4::Translate(particlesPos[itr]) *
  //  particlesData[itr].rot * Matrix4x4::Scale((1.0f - particlesData[itr].life / lifeTime) * growth + particlesData[itr].InitSize);
  //


  BROADCAST_MESSAGE(GraphicsMessage{ GFXMSG::DRAWPARTICLES, std::move(md), GetGameSpaceID() });
}

void ParticleEmitterC::EmitParticles(float dt)
{
  if (static_cast<unsigned>(ceilf(totalParticles)) != particlesVel.size())
  {
    particlesVel.clear();
    GL_particles.clear();
    particlesVel.reserve(static_cast<unsigned>(ceilf(totalParticles)));
    particlesVel.assign(static_cast<unsigned>(ceilf(totalParticles)), Vector3{});
    GL_particles.reserve(static_cast<unsigned>(ceilf(totalParticles)));
    GL_particles.assign(static_cast<unsigned>(ceilf(totalParticles)), ParticleGL{});
  }

  ParticlesUpdater(dt);

  if (!emiting || emitRate == 0)
    return;

  float dtCounter = dt;
  emitterTimer += dt;
  //for constantly emitting particles
  while (emitterTimer > emitRate)
  {
    emitterTimer -= emitRate;
    dtCounter -= emitRate;

	if (dtCounter / lifeTime >= 1.0f)
		continue;
	counter += particlesPerEmit;
	if (counter > totalParticles) counter = totalParticles;
    for (int i = counter - particlesPerEmit; i < counter; ++i)
    {
		Vector3 randomloc;
		randomloc.x = TOOLS::RandFloat(-randomDistance.x, randomDistance.x);
		randomloc.y = TOOLS::RandFloat(-randomDistance.y, randomDistance.y);
		randomloc.z = TOOLS::RandFloat(-randomDistance.z, randomDistance.z);
		offSetPosition += randomloc;
      if (randomRotation)
      {
        GL_particles[i].rotation = Vector3{ RandomFloat() * 360.f, RandomFloat() * 360.f, RandomFloat() * 360.f };

      }
	  else
		  GL_particles[i].rotation = Vector3{ 0,0,0 };
      GL_particles[i].init_alpha = initialOpacity;
      GL_particles[i].Percentage = dtCounter / lifeTime;
      Vector3 directVec{ Vector3{ 0,1,0 } };
      Vector3 rotateVec{ Vector3{ 0,1,0 } };

      bool directionTrans = false;
      Vector3 transSpace[3]{ directVector,{1.0f,0,0},{0,0,1.0f} };
      Vector3 rosSpace[3]{ directVector,{1.0f,0,0},{0,0,1.0f} };

      float rev{ 360.0f / particlesPerCyclic };

	  Vector3 tmpDirectVector = directVector;
	  tmpDirectVector = tmpDirectVector.Normalize();

      int styleMode = emitStyle;
      if (directVector == Vector3{ 0,0,0 })
        styleMode = defaultParticle;
      else
      {
        if (degree != 0)
        {
          if (tmpDirectVector != Vector3{ 0,1,0 })
          {
            directionTrans = true;

            transSpace[1] = tmpDirectVector.Cross(Vector3{ 0,1,0 }).Normalize();
            transSpace[2] = transSpace[1].Cross(transSpace[0]);
          }
          directVec = Matrix3x3::RotateZ(DegreePI*degree) * directVec;
        }
      }
      switch (styleMode)
      {
      case defaultParticle:
		  RenderDefaultParticle(i, directVec);
        break;
      case directionParticle:
		  RenderDirectionParticle(i, directVec);
        break;
      case cyclicParticle:
		  RenderCyclicParticle(i, directVec);
        break;
	  case lineSpawnParticle:
		  break;
	  case trailingParticle:
		  RenderTrailingParticle(i, directVec);
		  break;
      }

	  if (dtCounter > 0)
	  {
		  particlesVel[i].y -= gravityStrength * dtCounter;
		  GL_particles[i].pos += particlesVel[i] * dtCounter;
	  }
	  offSetPosition -= randomloc;
    }
  }
  objectPos = base.transform->GetGlobalPosition();
}


void ParticleEmitterC::Update(float dt)
{
  
  if (particlesPerEmit < 0 || totalParticles < 0)
    return;

  EmitParticles(dt);



 // RenderParticles();
}

bool ParticleEmitterC::ChooseTextureGUI()
{
  bool returnBool = false;
  auto& tList = GFX_S.GetTextureList();
  ImGui::Text("Texture");
  TOOLS::PushID(UITYPE::DEFAULT);
  
  if (ImGui::Combo("", &pos, tList.data(), int(tList.size()), int(tList.size())))
  {
    returnBool = true;
    mTexture = tList[pos];
    UpdateSerializables(mTexture);
  }
  ImGui::PopID();
  return returnBool;
}

void ParticleEmitterC::RenderDefaultParticle(int i, Vector3 directVec )
{
	do
	{
		directVec.x = RandomRangeFloat();
		directVec.y = RandomRangeFloat();
		directVec.z = RandomRangeFloat();
	} while (directVec == Vector3{ 0,0,0 });

	directVec.Normalize();

	if (directionRotation)
	{
		lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, directVec, Vector3{ 0,1.0f,0 });

		Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
		GL_particles[i].rotation *= 360.f;
	}

	if (rangeVelocity)
	{
		particlesVel[i] = directVec * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
	}
	else
		particlesVel[i] = directVec * velocityStrength;

	if (rangeSize)
	{
		GL_particles[i].initSize = (size_2 - size) * RandomFloat() + size;
		GL_particles[i].finalSize = (size_2 - size) * RandomFloat() + size + growth;
	}
	else
	{
		GL_particles[i].initSize = size;
		GL_particles[i].finalSize = size + growth;
	}

	if (rangeInitialDist)
		GL_particles[i].pos = offSetPosition + directVec * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
	else
		GL_particles[i].pos = offSetPosition + directVec * initialDistance;

}

void ParticleEmitterC::RenderDirectionParticle(int i, Vector3 directVec)
{
	Vector3 rotateVec{ Vector3{ 0,1,0 } };
	Vector3 rosSpace[3]{ directVector,{ 1.0f,0,0 },{ 0,0,1.0f } };
	Vector3 transSpace[3]{ directVector,{ 1.0f,0,0 },{ 0,0,1.0f } };
	Vector3 directVectorN = directVector;
	directVectorN = directVectorN.Normalize();

	if (directVector != Vector3{ 0,0,0 })
	{
		if (degree != 0)
		{
			if (directVectorN != Vector3{ 0,1,0 })
			{
				transSpace[1] = directVectorN.Cross(Vector3{ 0,1,0 }).Normalize();
				transSpace[2] = transSpace[1].Cross(transSpace[0]);
			}
			directVec = Matrix3x3::RotateZ(DegreePI*degree) * directVec;
		}
	}


	if (degree != 0)
	{
		rotateVec = Matrix3x3::RotateY(360 * DegreePI * RandomFloat())* directVec;

		rotateVec -= Vector3{ 0,1,0 };

		rotateVec = Vector3{ 0,1,0 } +RandomFloat() * rotateVec;

		rosSpace[0] = transSpace[0] * rotateVec.y;
		rosSpace[1] = transSpace[1] * rotateVec.x;
		rosSpace[2] = transSpace[2] * rotateVec.z;

		rotateVec = (rosSpace[0] + rosSpace[1] + rosSpace[2]).Normalize();
		if (directionRotation)
		{
			lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, rotateVec, Vector3{ 0,1.0f,0 });

			Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
			GL_particles[i].rotation *= 360.f;
		}
		if (rangeVelocity)
		{
			particlesVel[i] = rotateVec * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
		}
		else
			particlesVel[i] = rotateVec * velocityStrength;

		if (rangeInitialDist)
			GL_particles[i].pos = offSetPosition + rotateVec * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
		else
			GL_particles[i].pos = offSetPosition + rotateVec * initialDistance;
	}
	else
	{
		if (directionRotation)
		{
			lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, directVectorN, Vector3{ 0,1.0f,0 });

			Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
			GL_particles[i].rotation *= 360.f;
		}
		if (rangeVelocity)
		{
			particlesVel[i] = directVectorN * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
		}
		else
			particlesVel[i] = directVectorN * velocityStrength;

		if (rangeInitialDist)
			GL_particles[i].pos = offSetPosition + directVectorN * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
		else
			GL_particles[i].pos = offSetPosition + directVectorN * initialDistance;
	}

	if (rangeSize)
	{
		GL_particles[i].initSize = (size_2 - size) * RandomFloat() + size;
		GL_particles[i].finalSize = (size_2 - size) * RandomFloat() + size + growth;
	}
	else
	{
		GL_particles[i].initSize = size;
		GL_particles[i].finalSize = size + growth;
	}
}

void ParticleEmitterC::RenderCyclicParticle(int i, Vector3 directVec)
{
	Vector3 rotateVec{ Vector3{ 0,1,0 } };
	Vector3 rosSpace[3]{ directVector,{ 1.0f,0,0 },{ 0,0,1.0f } };
	Vector3 transSpace[3]{ directVector,{ 1.0f,0,0 },{ 0,0,1.0f } };
	float rev{ 360.0f / particlesPerCyclic };
	Vector3 directVectorN = directVector;
	directVectorN = directVectorN.Normalize();
	if (degree != 0)
	{
		curCyclic += rev;
		while (curCyclic > 360.0f)
			curCyclic -= 360.0f;


		rotateVec = Matrix3x3::RotateY(curCyclic * DegreePI)* directVec;

		//rotateVec -= Vector3{ 0,1,0 };
		//
		//rotateVec = Vector3{ 0,1,0 } +RandomFloat() * rotateVec;

		rosSpace[0] = transSpace[0] * rotateVec.y;
		rosSpace[1] = transSpace[1] * rotateVec.x;
		rosSpace[2] = transSpace[2] * rotateVec.z;

		rotateVec = (rosSpace[0] + rosSpace[1] + rosSpace[2]).Normalize();
		if (directionRotation)
		{
			lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, rotateVec, Vector3{ 0,1.0f,0 });

			Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
			GL_particles[i].rotation *= 360.f;
		}
		if (rangeVelocity)
		{
			particlesVel[i] = rotateVec * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
		}
		else
			particlesVel[i] = rotateVec * velocityStrength;


		if (rangeInitialDist)
			GL_particles[i].pos = offSetPosition + rotateVec * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
		else
			GL_particles[i].pos = offSetPosition + rotateVec * initialDistance;
	}
	else
	{
		if (directionRotation)
		{
			lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, directVectorN, Vector3{ 0,1.0f,0 });

			Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
			GL_particles[i].rotation *= 360.f;
		}
		if (rangeVelocity)
		{
			particlesVel[i] = directVectorN * ((velocityStrength_2 - velocityStrength) * RandomFloat() + velocityStrength);
		}
		else
			particlesVel[i] = directVectorN * velocityStrength;


		if (rangeInitialDist)
			GL_particles[i].pos = offSetPosition + directVectorN * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
		else
			GL_particles[i].pos = offSetPosition + directVectorN * initialDistance;

	}

	if (rangeSize)
	{
		GL_particles[i].initSize = (size_2 - size) * RandomFloat() + size;
		GL_particles[i].finalSize = (size_2 - size) * RandomFloat() + size + growth;
	}
	else
	{
		GL_particles[i].initSize = size;
		GL_particles[i].finalSize = size + growth;
	}

	if (rangeInitialDist)
		GL_particles[i].pos = offSetPosition + directVectorN * ((initialDistance_2 - initialDistance) * RandomFloat() + initialDistance);
	else
		GL_particles[i].pos = offSetPosition + directVectorN * initialDistance;
}

void ParticleEmitterC::RenderTrailingParticle(int i, Vector3 directVec)
{
	Vector3 fixVec = base.transform->GetGlobalPosition() - objectPos;
	//hardcode for when player jumps, no particles will come out
	/*if (fixVec == Vector3{ 0,0,0 } || fixVec.y != 0)
		return;
	fixVec.y = 0;*/

	if (directionRotation)
	{
		lookAtM = Matrix4x4::LookAtRm(Vector3{ 0,0,0 }, fixVec, Vector3{ 0,1.0f,0 });

		Matrix4x4::GetRotationAngle(lookAtM, GL_particles[i].rotation.x, GL_particles[i].rotation.y, GL_particles[i].rotation.z);
		GL_particles[i].rotation *= 360.f;
	}

	if (rangeSize)
	{
		GL_particles[i].initSize = (size_2 - size) * RandomFloat() + size;
		GL_particles[i].finalSize = (size_2 - size) * RandomFloat() + size + growth;
	}
	else
	{
		GL_particles[i].initSize = size;
		GL_particles[i].finalSize = size + growth;
	}

	particlesVel[i] = -fixVec * velocityStrength;

	GL_particles[i].pos = offSetPosition;

}

//int ParticleEmitterC::totalParticlesNeeded()
//{
//  if (emitRate != 0 || particlesPerEmit != 0 || lifeTime == 0)
//    return ceilf(lifeTime / emitRate) * particlesPerEmit + particlesPerEmit;
//  
//
//  return 0;
//}

