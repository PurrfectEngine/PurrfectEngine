#version 450

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform Camera {
  mat4 projection;
  mat4 view;
  vec4 pos;
} cam;

struct Light {
  vec4 pos;
  vec4 color;
};

layout(std430, set = 2, binding = 0) buffer Lights {
  uvec4 counts;
  Light lights[];
} lights;

layout(set = 3, binding = 0) uniform samplerCube uIrradianceMap;
layout(set = 3, binding = 1) uniform samplerCube uPreFilterMap;
layout(set = 3, binding = 2) uniform sampler2D uBrdfLUT;
layout(set = 4, binding = 0) uniform sampler2D uNormalMap;
layout(set = 5, binding = 0) uniform sampler2D uRoughnessMap;
layout(set = 6, binding = 0) uniform sampler2D uMetallicMap;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
return F0 + (max(vec3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a      = roughness*roughness;
  float a2     = a*a;
  float NdotH  = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float num   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;

  float num   = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2  = GeometrySchlickGGX(NdotV, roughness);
  float ggx1  = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}

void main() {
  vec3 WorldPos = inWorldPos;

  vec3  albedo    = inColor;
  vec3  Normal    = texture(uNormalMap, inUV).rgb * inNormal;
  float roughness = texture(uRoughnessMap, inUV).r;
  float metallic  = texture(uMetallicMap, inUV).r;

  float ao = 1.0;

  vec3 N = normalize(Normal);
  vec3 V = normalize(cam.pos.xyz - WorldPos);
  vec3 R = reflect(-V, N);

  vec3 F0 = vec3(0.04, 0.04, 0.04);
  F0 = mix(F0, albedo, metallic);

  // reflectance equation
  vec3 Lo = vec3(0.0, 0.0, 0.0);
  for(int i = 0; i < lights.counts.x; ++i) {
    Light light = lights.lights[i];
    // calculate per-light radiance
    vec3 L = normalize(light.pos.xyz - WorldPos);
    vec3 H = normalize(V + L);
    float distance    = length(light.pos.xyz - WorldPos);
    float attenuation = 1.0 / (distance * distance);
    //float attenuation = 10.0 / (distance);
    vec3 radiance     = light.color.xyz * attenuation;

    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
  }

  vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

  vec3 kS = F;
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - metallic;

  vec3 irradiance = texture(uIrradianceMap, N).rgb;
  vec3 diffuse = irradiance * albedo;

  const float MAX_REFLECTION_LOD = 4.0;
  vec3 prefilteredColor = textureLod(uPreFilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
  vec2 envBRDF = texture(uBrdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
  vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

  vec3 ambient = (kD * diffuse + specular) * ao;

  vec3 color = ambient + Lo;
  color = color / (color + vec3(1.0, 1.0, 1.0));
  color = pow(color, vec3(1.0/2.2, 1.0/2.2, 1.0/2.2));
  outColor = vec4(color, 1.0);
}