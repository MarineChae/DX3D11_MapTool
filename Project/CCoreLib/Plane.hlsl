struct VS_IN
{
	float4 Pos : POSITION;
	float2 Tex : TEXTURE;

};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXTURE;

};

cbuffer ConstantBuffer
{
	matrix g_WorldMatrix : packoffset(c0);
	matrix g_ViewMatrix : packoffset(c4);
	matrix g_ProjMatrix : packoffset(c8);
};

VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut = (VS_OUT)0;
	vOut.Pos = mul(vIn.Pos, g_WorldMatrix);
	vOut.Pos = mul(vOut.Pos, g_ViewMatrix);
	vOut.Pos = mul(vOut.Pos, g_ProjMatrix);

	vOut.Tex = vIn.Tex;

	return vOut;
}

Texture2D TexDifuse1 : register(t0);
SamplerState Sample0 : register(s0);

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXTURE;

};

float4 PS(PS_IN vIn) : SV_Target
{
	return TexDifuse1.Sample(Sample0,vIn.Tex);//float4(0,1,0,0.2f);
}