Texture2D WaterDiffuse : register(t0);
Texture2D RefractMap : register(t1);
Texture2D ReflectMap : register(t2);
Texture2D NormalMap : register(t3);
SamplerState Sample0 : register(s0);

struct VS_IN
{
    float4 Pos : POSITION;
    float3 Nor : NORMAL;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD;
    float3 Tan : TANGENT;
    float   Emi : EMISSIVE;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float3 Nor : NORMAL;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD0;
    float4 tW  : TEXCOORD1;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix g_WorldMatrix : packoffset(c0);
    matrix g_ViewMatrix  : packoffset(c4);
    matrix g_ProjMatrix  : packoffset(c8);
    float4 g_MeshColor   : packoffset(c12);
};
cbuffer LightBuffer : register(b1)
{
    float4 ambient : packoffset(c0);
    float4 diffuse : packoffset(c1);
    float4 specular : packoffset(c2);

};
cbuffer ChangeEveryFrame : register(b2)
{
    float3 CameraPos;
    float  padding0;
    float3 CamereaDir;//look
    float  padding1;
    float3 LightDirection;
    float  padding2;

};
cbuffer ShadowCB : register(b3)
{
    float4x4 g_matShadow1;
    float4x4 g_matShadow2;
}

VS_OUT VS(VS_IN vIn)
{
    VS_OUT vOut = (VS_OUT) 0;
    float4 vWorld = mul(vIn.Pos, g_WorldMatrix);
    float fTime = cos(g_MeshColor.w) * 0.1f;
    float fAngle = fTime * length(vIn.Pos / 256.0f);
    float fCos, fSin;
    sincos(fAngle, fSin, fCos);
    vWorld.y = min(vWorld.y, vWorld.y - 2.0f - cos(fTime * vIn.Pos.x) + sin(fTime * vIn.Pos.z));
    
    float4 vView = mul(vWorld, g_ViewMatrix);
    float4 vProj = mul(vView, g_ProjMatrix);
    vOut.Pos = vProj;
    vOut.Nor = vIn.Nor;
    vOut.Col = vIn.Col;
    vOut.Tex = vIn.Tex;
    vOut.tW = vOut.Pos;

    
    return vOut;
}



float4 PS(VS_OUT vIn) : SV_Target
{
    float2 tex = vIn.tW.xy / vIn.tW.w;
    tex.x = tex.x * 0.5f + 0.5f;
    tex.y = -tex.y * 0.5f + 0.5f;

    float4 vFinalColor = WaterDiffuse.Sample(Sample0, vIn.Tex);
    float4 vNormal = NormalMap.Sample(Sample0, vIn.Tex*10.0f);
    vNormal = normalize((vNormal - 0.5f) * 2.0f);
    float4 vRefractColor = RefractMap.Sample(Sample0, tex + vNormal.xy * 0.05f);
    float4 vReflectColor = ReflectMap.Sample(Sample0, tex + vNormal.xy * 0.05f);
    vFinalColor = vFinalColor * vReflectColor * vRefractColor * vIn.Col;
    vFinalColor.a = 1.0f;


    return vFinalColor ;
}