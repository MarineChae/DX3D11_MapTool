Texture2D TexDifuse1[6] : register(t0);
SamplerState Sample0 : register(s0);


struct VS_IN
{
	float4 Pos : POSITION;
	float3 Nor : NORMAL;
	float4 Col : COLOR;
    float2 Tex : TEXCOORD;
    float3 Tan : TANGENT;

};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float4 Col : COLOR0;
    float2 Tex : TEXCOORD;
    
};
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Nor : NORMAL;
    float4 Col : COLOR0;
    float2 Tex : TEXCOORD;
    uint PrimitiveID : SV_PrimitiveID;
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
	float4 vWorld = mul(vIn.Pos, g_WorldMatrix);
    matrix boxview = g_ViewMatrix;
    boxview._41 = 0;
    boxview._42 = 0;
    boxview._43 = 0;
    float4 vView = mul(vWorld, boxview);
	float4 vProj = mul(vView, g_ProjMatrix);
    
	vOut.Pos = vProj;
	vOut.Nor = vIn.Nor;
	vOut.Col = vIn.Col;
	vOut.Tex = vIn.Tex;
    

	return vOut;
}

float4 GetTextureColor(uint index, float2 uv)
{
    float4 fColor = float4(1, 1, 1, 1);
    switch (index)
    {
        case 0:
            fColor = TexDifuse1[0].Sample(Sample0, uv);
            break;
        case 1:
            fColor = TexDifuse1[1].Sample(Sample0, uv);
            break;
        case 2:
            fColor = TexDifuse1[2].Sample(Sample0, uv);
            break;
        case 3:
            fColor = TexDifuse1[3].Sample(Sample0, uv);
            break;
        case 4:
            fColor = TexDifuse1[4].Sample(Sample0, uv);
            break;
        case 5:
            fColor = TexDifuse1[5].Sample(Sample0, uv);
            break;
    }
    return fColor;
}
float4 PS(PS_INPUT pIn) : SV_Target
{
    return TexDifuse1[0].Sample(Sample0, pIn.Tex);
	
    //return vIn.Col;
}