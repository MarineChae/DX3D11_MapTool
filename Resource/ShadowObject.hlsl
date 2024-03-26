//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D g_txDiffuse : register(t0);
Texture2D g_txShadow1 : register(t1);
SamplerState samLinear : register(s0);
cbuffer ConstantBuffer : register(b0)
{
    matrix g_WorldMatrix : packoffset(c0);
    matrix g_ViewMatrix : packoffset(c4);
    matrix g_ProjMatrix : packoffset(c8);

};
cbuffer cb1 : register(b1)
{
    float4x4 g_matShadow1;
    float4x4 g_matShadow2;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 p : POSITION;
    float3 n : NORMAL;
    float4 c : COLOR;
    float2 t : TEXCOORD;
};
struct VS_OUTPUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL;
    float4 c : COLOR0;
    float2 t : TEXCOORD0;
    float4 TexShadow1 : TEXCOORD1;
    float4 TexShadow2 : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT vIn)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.p = mul(vIn.p, g_WorldMatrix);
    output.p = mul(output.p, g_ViewMatrix);
    output.p = mul(output.p, g_ProjMatrix);
    output.n = vIn.n;
    output.t = vIn.t;
    output.c = vIn.c;
	// ÅØ½ºÃ³ÁÂÇ¥
    output.TexShadow1 = mul(vIn.p, g_matShadow1);
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT vIn) : SV_Target
{
	//float4 shadow1 = g_txShadow1.Sample(samLinear, vIn.TexShadow1.xy / vIn.TexShadow1.w);
	//float4 shadow2 = g_txShadow2.Sample(samLinear, vIn.TexShadow2.xy / vIn.TexShadow2.w);
    float4 FinalColor = g_txDiffuse.Sample(samLinear, vIn.t);
	/*if (shadow1.r > 0.01f || shadow2.r > 0.01f)
	{
		FinalColor = FinalColor * float4(0.5f, 0.5f, 0.5f, 1.0f);
		FinalColor.w = 1.0f;
	}*/
    return vIn.c;
}
float4 PS_Color(VS_OUTPUT vIn) : SV_Target
{
    return vIn.c;
}
