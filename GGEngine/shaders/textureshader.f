#version 430 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 emissive_mask;

in VS_OUT
{
  vec4 frag_pos;
  vec4 frag_normal;
  vec4 shadow_pos;
  vec2 uv;
}fs_in;

// Uniform blocks
struct Light
{
    vec3 LightPosition;
    float LightIntensity;
    vec4 LightAmbient;
    vec4 LightDiffuse;
    vec4 LightSpecular;
    vec3 LightDirection;
    float LightRadius;
    int LightType;
    float InnerCOS;
    float OuterCOS;
    float falloff;
};

layout (std140) uniform LightArray
{
	Light lights[32];
} LA;

const bool material_system = true;

uniform int num_lights;
uniform vec3 camera_position;
uniform float shininess;
uniform int DirectionalLightIndex;

uniform bool gSoftShadow;
uniform bool ReceiveShadow;

uniform float zNear;
uniform float zFar;
uniform vec3 gAmbient;
uniform bool gFogEnabled;
uniform vec4 gFog;
uniform float gFogDistance;
uniform ivec2 frameDimension;
uniform int frameIndex;


const int POINT_LIGHT = 0;
const int DIRECTIONAL_LIGHT = 1;
const int SPOT_LIGHT = 2;



uniform sampler2D   m_diffuse;
uniform sampler2D   m_specular;
uniform float       m_specular_power;
uniform sampler2D   m_normal;
uniform float       m_normal_power;
uniform sampler2D   m_opacity_mask;
uniform sampler2D   m_self_illumination;
uniform vec4        m_emissive_clr;
uniform float       m_illumination_power;
uniform float       m_global_transparency;
uniform vec4        m_tint;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D shadowMap;

vec3 Ka = vec3(0.15f);

vec3 CalcPointLight(int i, vec3 Kd, vec3 Ks, vec4 N, vec3 V)
{
  // Ambient
  vec3 I_ambient = clamp(LA.lights[i].LightAmbient.xyz * Ka, 0.0f, 1.0f) * LA.lights[i].LightAmbient.w;

  // Diffuse
  vec4 light = vec4( LA.lights[i].LightPosition, 1.0 );
  float D = length(light - fs_in.frag_pos);
  float denom = D / (LA.lights[i].LightRadius * LA.lights[i].LightRadius) + 1.0f;
  vec4 L = (light - fs_in.frag_pos) / D;
  float N_dot_L = max( dot(N, L ), 0.0f );
  vec3 I_diffuse = clamp(LA.lights[i].LightDiffuse.xyz * Kd * N_dot_L, 0.0f, 1.0f);

  // Specular (complete the implementation)
  vec3 R = (2 * dot(N, L) * N - L).xyz;
  
  float specIntensity = pow(max(dot(R, V), 0.0), Ks.r * Ks.r * shininess);
  vec3 I_specular = clamp(LA.lights[i].LightSpecular.xyz * Ks * specIntensity, 0.0f, 1.0f);


  // Attenutation terms
  float attenuation = LA.lights[i].LightIntensity / (denom * denom);
  attenuation = (attenuation - 0.25) / (1.0 - 0.25);
  attenuation = clamp(attenuation, 0.0f, 1.0f);
	
  // Final color
  return attenuation * (I_ambient + I_diffuse + I_specular);
}

vec3 CalcDirectionalLight(int i, vec3 Kd, vec3 Ks, vec4 N, vec3 V, vec4 L)
{
  // Ambient
  vec3 I_ambient = clamp(LA.lights[i].LightAmbient.xyz * Ka, 0.0f, 1.0f) * LA.lights[i].LightAmbient.w;

  // Diffuse
  vec4 light = vec4( LA.lights[i].LightPosition, 1.0 );
  float N_dot_L = max( dot(N, L), 0.0f );
  vec3 I_diffuse = clamp(LA.lights[i].LightDiffuse.xyz * Kd * N_dot_L, 0.0f, 1.0f);

  // Specular (complete the implementation)
  vec3 R = (2 * dot(N, L) * N - L).xyz;
  
  float specIntensity = pow(max(dot(R, V), 0.0), Ks.r * Ks.r * shininess);
  vec3 I_specular = clamp(LA.lights[i].LightSpecular.xyz * Ks * specIntensity, 0.0f, 1.0f);
	
  // Final color
  return (I_ambient + I_diffuse + I_specular) * LA.lights[i].LightIntensity;
}

vec3 CalcSpotLight(int i, vec3 Kd, vec3 Ks, vec4 N, vec3 V, vec4 L)
{
  vec4 ModelToLight = vec4(normalize(LA.lights[i].LightPosition.xyz - fs_in.frag_pos.xyz), 0.0f);
  float SpotLightEffect = max(LA.lights[i].falloff == 0 ? 0.0 : pow((max(dot(L, ModelToLight), 0.0f) - LA.lights[i].OuterCOS) / (LA.lights[i].InnerCOS - LA.lights[i].OuterCOS), LA.lights[i].falloff), 0.0f);
  
  // Ambient
  vec3 I_ambient = LA.lights[i].LightAmbient.xyz * Ka * LA.lights[i].LightAmbient.w;

  // Diffuse
  vec4 light = vec4( LA.lights[i].LightPosition.xyz, 1.0 );
  float D = length(light - fs_in.frag_pos);
  vec4 Lv = (light - fs_in.frag_pos) / D; 
  float N_dot_L = max( dot(N, Lv ), 0.0f );
  vec3 I_diffuse = LA.lights[i].LightDiffuse.xyz * Kd * N_dot_L;

  // Specular (complete the implementation)
  vec3 R = (2 * dot(N, Lv) * N - Lv).xyz;
  
  float specIntensity = pow(max(dot(R, V), 0.0), Ks.r * Ks.r * shininess);
  vec3 I_specular = LA.lights[i].LightSpecular.xyz * Ks * specIntensity;


  // Attenutation terms
  float attenuation = D / (1.0 + 0.22 * D + 0.15 * D * D);
	
  // Final color
  return attenuation * SpotLightEffect * (I_ambient + I_diffuse + I_specular);
}

float calculateShadow(vec4 shadowpos, vec4 N)
{
  vec3 projCoords = shadowpos.xyz / shadowpos.w;
  projCoords = projCoords * 0.5 + 0.5; 
  float closestDepth = texture(shadowMap, projCoords.xy).r;  
  float currentDepth = projCoords.z;    
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  vec4 L = vec4(normalize(LA.lights[DirectionalLightIndex].LightDirection), 0.0f);
  
  float bias = max(0.005 * (1.0 - dot(N, L)), 0.0005);  
  float shadow = 0;

  if (gSoftShadow)
  {
    for(int x = -8; x <= 8; ++x)
    {
      for(int y = -8; y <= 8; ++y)
      {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
        shadow += currentDepth - bias > pcfDepth ? 0.85 : 0.0;        
      }    
    }
    shadow /= 289.0f;
  }
  else
  { 
    shadow = currentDepth - bias > closestDepth ? 0.85 : 0.0;
  }
  
  if (projCoords.z > 1.0)
    shadow = 0.0;
  if (projCoords.x < 0.0)
    shadow = 0.0;
  if (projCoords.y < 0.0)
    shadow = 0.0;
  if (projCoords.x < 0.0)
    shadow = 0.0;
  if (projCoords.y < 0.0)
    shadow = 0.0;
  
  return 1.0 - shadow;
}

void main(void) {
  vec2 multiplier = vec2(frameIndex / frameDimension.x, frameIndex % frameDimension.x);
  vec2 finaluv = vec2(fs_in.uv.x / frameDimension.x, fs_in.uv.y / frameDimension.y);
  finaluv += vec2(1.0 / frameDimension.x * multiplier.x, 1.0 / frameDimension.y * multiplier.y);
  finaluv.y = 1.0f - finaluv.y;

  vec3 Ilocal = vec3(0.0f);
  vec4 texclr = texture(m_diffuse, finaluv);
  vec3 Kd = texclr.rgb;
  vec3 Ks = texture(m_specular, finaluv).rgb;
  vec4 emissive = vec4(m_emissive_clr.rgb * m_illumination_power, m_emissive_clr.a);

  vec4 N = normalize(fs_in.frag_normal);
  vec3 V = normalize(camera_position - fs_in.frag_pos.xyz);

  for( int i = 0; i < num_lights; ++i )
  {
    vec4 L = vec4(normalize(LA.lights[i].LightDirection), 0.0f);
    switch(LA.lights[i].LightType)
    {
      case POINT_LIGHT:
        Ilocal += CalcPointLight(i, Kd, Ks, N, V);
      break;
      case DIRECTIONAL_LIGHT:
        Ilocal += CalcDirectionalLight(i, Kd, Ks, N, V, L);
      break;
      case SPOT_LIGHT:
        Ilocal += CalcSpotLight(i, Kd, Ks, N, V, L);
      break;
    }
  }
  
  vec3 result = Ilocal + gAmbient;
  result = mix(result, m_tint.rgb, m_tint.a);
  
  if (gFogEnabled)
  {
	  vec3 oldResult = result;
      float tocamlen = length(fs_in.frag_pos.xyz - camera_position);
      if (tocamlen > gFogDistance)
      {
        float S = (zFar - tocamlen + gFogDistance) / (zFar - zNear);
        result = S * result + (1 - S) * gFog.xyz;
      }
	  result = mix(oldResult, result, gFog.w);
  }
  
  
  if (ReceiveShadow && DirectionalLightIndex != 8)
    result *= calculateShadow(fs_in.shadow_pos, N);
    
  // result = mix(result, emissive.rgb, emissive.a);
  result.rgb += emissive.rgb;

  vec4 result4 = vec4(result, 1.0f);
  
  vec4 transparent = vec4(0.0, 0.0, 0.0, 0.0);
  color = mix(transparent, result4, texclr.a);
  
  emissive_mask = m_emissive_clr;
  color *= m_global_transparency;


  // color = vec4(finaluv.x, finaluv.y, 0.0f, 1.0f); // debug uv
  // color = vec4(fs_in.frag_normal.x, fs_in.frag_normal.y, fs_in.frag_normal.z, 1.0f); // debug normal
  // color = vec4(texclr.w, texclr.w, texclr.w, 1.0f); // debug tint
}