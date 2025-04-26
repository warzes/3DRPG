#pragma once

//=============================================================================
#pragma region [ Vertex Shaders sources ]
const GLchar* vertexShaderSource = R"glsl(
#version 430 core

layout(binding = 0) uniform TransformData
{
	mat4 model;
};

layout(binding = 1) uniform CameraData
{
	mat4 view;
	mat4 projection;
	vec3 cameraPosition;
};

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoords;

layout(location = 0) smooth out vec3 PositionOut;
layout(location = 1) smooth out vec3 NormalOut;
layout(location = 2) smooth out vec2 TexCoordsOut;

void main()
{
	// Transform vertex
	vec4 position = model * vec4(VertexPosition, 1.0f);
	gl_Position = projection * view * position;
	PositionOut = position.xyz;

	// Transform normal
	vec4 normal = model * vec4(VertexNormal, 0.0f);
	NormalOut = normal.xyz;

	// Pass-through UV coordinates
	TexCoordsOut = VertexTexCoords;
}
)glsl";
#pragma endregion
//=============================================================================
#pragma region [ Fragment Shaders sources ]
const GLchar* fragmentShaderSource = R"glsl(
#version 430 core

struct PointLight
{
	vec3 v3LightPosition;
	vec3 v3LightIntensity;
	vec3 v3Falloff;
	int enable;
};
#define MAX_LIGHTS 16

layout(binding = 1) uniform CameraData
{
	mat4 view;
	mat4 projection;
	vec3 cameraPosition;
};

layout(std140, binding = 2) uniform PointLightData
{
	PointLight PointLights[MAX_LIGHTS];
};

layout(location = 0) uniform int iNumPointLights;
layout(location = 1) uniform float EmissivePower;

layout(binding = 0) uniform sampler2D s2DiffuseTexture;
layout(binding = 1) uniform sampler2D s2SpecularTexture;
layout(binding = 2) uniform sampler2D s2RoughnessTexture;
layout(binding = 3) uniform samplerCube scRefractMapTexture;

#define M_RCPPI 0.31830988618379067153776752674503f
#define M_PI 3.1415926535897932384626433832795f

layout(location = 0) in vec3 PositionIn;
layout(location = 1) in vec3 NormalIn;
layout(location = 2) in vec2 TexCoordsIn;

out vec4 FragColorOut;

subroutine vec3 RefractMap(vec3, vec3, vec3, vec4, vec3);

layout(location = 0) subroutine uniform RefractMap RefractMapUniform;

vec3 lightFalloff(in vec3 v3LightIntensity, in vec3 v3Falloff, in vec3 v3LightPosition, in vec3 v3Position)
{
	// Calculate distance from light
	float fDist = distance(v3LightPosition, v3Position);

	// Return falloff
	float fFalloff = v3Falloff.x + (v3Falloff.y * fDist) + (v3Falloff.z * fDist * fDist);
	return v3LightIntensity / fFalloff;
}

vec3 schlickFresnel(in vec3 v3LightDirection, in vec3 v3Normal, in vec3 v3SpecularColour)
{
	// Schlick Fresnel approximation
	float fLH = dot(v3LightDirection, v3Normal);
	return v3SpecularColour + (1.0f - v3SpecularColour) * pow(1.0f - fLH, 5);
}

float TRDistribution(in vec3 v3Normal, in vec3 v3HalfVector, in float fRoughness)
{
	// Trowbridge-Reitz Distribution function
	float fNSq = fRoughness * fRoughness;
	float fNH = max(dot(v3Normal, v3HalfVector), 0.0f);
	float fDenom = fNH * fNH * (fNSq - 1.0f) + 1.0f;
	return fNSq / (M_PI * fDenom * fDenom);
}

float GGXVisibility(in vec3 v3Normal, in vec3 v3LightDirection, in vec3 v3ViewDirection, in float fRoughness)
{
	// GGX Visibility function
	float fNL = max(dot(v3Normal, v3LightDirection), 0.0f);
	float fNV = max(dot(v3Normal, v3ViewDirection), 0.0f);
	float fRSq = fRoughness * fRoughness;
	float fRMod = 1.0f - fRSq;
	float fRecipG1 = fNL + sqrt(fRSq + (fRMod * fNL * fNL));
	float fRecipG2 = fNV + sqrt(fRSq + (fRMod * fNV * fNV));

	return 1.0f / (fRecipG1 * fRecipG2);
}

vec3 GGX(in vec3 v3Normal, in vec3 v3LightDirection, in vec3 v3ViewDirection, in vec3 v3LightIrradiance, in vec3 v3DiffuseColour, in vec3 v3SpecularColour, in float fRoughness)
{
	// Calculate diffuse component
	vec3 v3Diffuse = v3DiffuseColour * M_RCPPI;

	// Calculate half vector
	vec3 v3HalfVector = normalize(v3ViewDirection + v3LightDirection);

	// Calculate Toorance-Sparrow components
	vec3 v3F = schlickFresnel(v3LightDirection, v3HalfVector, v3SpecularColour);
	float fD = TRDistribution(v3Normal, v3HalfVector, fRoughness);
	float fV = GGXVisibility(v3Normal, v3LightDirection, v3ViewDirection, fRoughness);

	// Modify diffuse by Fresnel reflection
	v3Diffuse *= (1.0f - v3F);

	// Combine diffuse and specular
	vec3 v3RetColour = v3Diffuse + (v3F * fD * fV);

	// Multiply by view angle
	v3RetColour *= max(dot(v3Normal, v3LightDirection), 0.0f);

	// Combine with incoming light value
	v3RetColour *= v3LightIrradiance;

	return v3RetColour;
}

vec3 SpecularTransmit(in vec3 v3Normal, in vec3 v3ViewDirection, in vec3 v3DiffuseColour, in vec3 v3SpecularColour)
{
	// Calculate index of refraction from Fresnel term
	float fRootF0 = sqrt(v3SpecularColour.x);
	float fIOR = (1.0f - fRootF0) / (1.0f + fRootF0);

	// Get refraction direction
	vec3 v3Refract = refract(-v3ViewDirection, v3Normal, fIOR);

	// Get refraction map data
	vec3 v3RefractColour = texture(scRefractMapTexture, v3Refract).rgb;

	// Evaluate specular transmittance
	vec3 v3RetColour = fIOR * (1.0f - schlickFresnel(v3Refract, -v3Normal, v3SpecularColour));
	v3RetColour *= v3DiffuseColour;

	// Combine with incoming light value
	v3RetColour *= v3RefractColour;

	return v3RetColour;
}

vec3 Emissive(vec3 v3ColourOut, vec3 v3DiffuseColour)
{
	// Add in emissive contribution
	return v3ColourOut + (EmissivePower * v3DiffuseColour);
}

layout(index = 0) subroutine(RefractMap) vec3 noRefractMap(vec3 v3ColourOut, vec3 v3Normal, vec3 v3ViewDirection, vec4 v4DiffuseColour, vec3 v3SpecularColour)
{
	// Return colour unmodified
	return v3ColourOut;
}

layout(index = 1) subroutine(RefractMap) vec3 textureRefractMap(vec3 v3ColourOut, vec3 v3Normal, vec3 v3ViewDirection, vec4 v4DiffuseColour, vec3 v3SpecularColour)
{
	// Get specular transmittance term
	vec3 v3Transmit = SpecularTransmit(v3Normal, v3ViewDirection, v4DiffuseColour.rgb, v3SpecularColour);

	// Add in transparent contribution and blend with existing
	return mix(v3Transmit, v3ColourOut, v4DiffuseColour.w);
}

void main()
{
	// Normalize the inputs
	vec3 v3Normal = normalize(NormalIn);
	vec3 v3ViewDirection = normalize(cameraPosition - PositionIn);

	// Get texture data
	vec4 DiffuseColour = texture(s2DiffuseTexture, TexCoordsIn);
	vec3 v3SpecularColour = texture(s2SpecularTexture, TexCoordsIn).rgb;
	float fRoughness = texture(s2RoughnessTexture, TexCoordsIn).r;

	// Loop over each point light
	vec3 v3RetColour = vec3(0.0f);
	for (int i = 0; i < iNumPointLights; i++)
	{
		if (PointLights[i].enable == 1)
		{
			vec3 v3LightDirection = normalize(PointLights[i].v3LightPosition - PositionIn);

			// Calculate light falloff
			vec3 v3LightIrradiance = lightFalloff(PointLights[i].v3LightIntensity, PointLights[i].v3Falloff, PointLights[i].v3LightPosition, PositionIn);

			// Perform shading
			v3RetColour += GGX(v3Normal, v3LightDirection, v3ViewDirection, v3LightIrradiance, DiffuseColour.rgb, v3SpecularColour, fRoughness);
		}
	}

	// Add in ambient contribution
	v3RetColour += DiffuseColour.rgb * vec3(0.3f);

	// Add in any refraction contribution
	v3RetColour = RefractMapUniform(v3RetColour, v3Normal, v3ViewDirection, DiffuseColour, v3SpecularColour);

	// Add in any emissive contribution
	v3RetColour = Emissive(v3RetColour, DiffuseColour.rgb);

	FragColorOut = vec4(v3RetColour, DiffuseColour.a);
}
)glsl";
#pragma endregion
//=============================================================================