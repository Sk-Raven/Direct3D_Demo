#include"Light.hlsli"

Texture2D g_Tex : register(t0);
SamplerState g_SamLinear : register(s0);

cbuffer CBChangesEveryDrawing : register(b0)
{
	matrix g_World;
	matrix g_WorldInvTranspose;
}

cbuffer CBChangesEveryFrame : register(b1)
{
	matrix g_View;
	float3 g_EyePosW;
}

cbuffer CBChangesOnResize : register(b2)
{
	matrix g_Proj;
}

cbuffer CBChangesRarely : register(b3)
{
	DirectionalLight g_DirLight[10];
	PointLight g_PointLight[10];
	SpotLight g_SpotLight[10];
	Material g_Material;
	int g_NumDirLight;
	int g_NumPointLight;
	int g_NumSpotLight;
}
