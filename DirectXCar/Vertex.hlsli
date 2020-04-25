struct VertexIn
{
	float3 posL : POSITION;
	float4 color : COLOR;
};

struct VertexOut
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};

struct VertexPosNormalTex
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexPosTex
{
	float3 PosL : POSITION;
	float2 Tex : TEXCOORD;
};

struct VertexPosHWNormalTex
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;     // �������е�λ��
	float3 NormalW : NORMAL;    // �������������еķ���
	float2 Tex : TEXCOORD;
};

struct VertexPosHTex
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};