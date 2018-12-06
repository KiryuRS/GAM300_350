#pragma once
#include "common.h"
#include "Component.h"
#include "StaticMeshC.h"


enum particleMode : int
{
  defaultParticle = 0,
  directionParticle,
  cyclicParticle,
  lineSpawnParticle,
  trailingParticle
};

struct Particles
{
  Vector3 InitSize;
  Vector3 velocity;
  float life;
  Matrix4x4 rot;

  Particles() 
  :
    InitSize{0,0,0},
    velocity{0,0,0},
    life{0},
    rot{}
  {};
};


struct ParticleGL
{
  Vector3 pos;
  float initSize;
  float finalSize;
  float Percentage;
  float init_alpha;
  float final_alpha = 0.f;
  Vector3 rotation; /// radians
};






class ParticleEmitterC : public Component
{
public:
  //TransformC* tranformObject;
  BaseMeshC<StaticMesh, ParticleEmitterC> base;
  int pos = 0;
  Vector3 offSetPosition;
  float warmthUpTime;
  float lifeTime;
  float totalParticles;
  float emitRate;
  int particlesPerEmit;
  
  float gravityStrength;
  
  bool emiting;
  bool emissive_render = { false };
  bool haha = { true };
  bool rangeVelocity;
  float velocityStrength;
  float velocityStrength_2;
  float emitterTimer;
  float initialOpacity;
  bool rangeInitialDist;
  float initialDistance;
  float initialDistance_2;
  bool rangeSize;
  bool randomRotation;
  bool directionRotation;
  float size;
  float size_2;
  float growth;
  Vector3 randomDistance;
  
  int emitStyle;

  // cone
  Vector3 directVector;
  float degree;

  // cyclic
  int particlesPerCyclic;
  
  bool lookAtCam;
  bool emitOnceStart;


  int counter;

  Vector3 objectPos;

  std::vector<Vector3> particlesVel;

  //std::vector< Particles> particlesData;

  std::vector<ParticleGL> GL_particles;

  GLuint particles_VBO;

  float curCyclic;

  Matrix4x4 lookAtM;

  // functionality


  // calculate particle

  void EmitOnce();

  void ParticlesUpdater(float dt);
  void RenderParticles();
  bool ChooseTextureGUI();
  void RenderDefaultParticle(int, Vector3 directVec);
  void RenderDirectionParticle(int, Vector3 directVec);
  void RenderCyclicParticle(int, Vector3 directVec);
  void RenderTrailingParticle(int, Vector3 directVec);

  std::string mTexture = "CompiledAssets/Sprites/sample.dds";


  void EmitParticles(float dt);
  GLuint GetDataVBO() const
  {
    return particles_VBO;
  }
  
  ParticleEmitterC();

  void AddSerializeData(LuaScript* state = nullptr) override;
  static constexpr auto GetType() { return COMPONENTTYPE::PARTICLEEMITTER; }
  bool EditorGUI(Editor& edit) override;
  void EditorSetup() override;
  void EditorUpdate(float) override;
  void Update(float) override;
  void Reset();
  void DestroyUpdate() override;
  void Render() override;

  void Initialize() override;
};
