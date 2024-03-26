#define FOGMODE_NONE    0
#define FOGMODE_LINEAR  1
#define FOGMODE_EXP     2
#define FOGMODE_EXP2    3
#define E 2.71828

Texture2D TexDifuse1 : register(t0);
Texture2D TexDifuse2 : register(t1);
Texture2D TexDifuse3[4] : register(t2);
Texture2D TexDifuse4[4] : register(t6);
Texture2D g_txShadow1 : register(t10);
SamplerState Sample0 : register(s0);
SamplerState ShadowSS : register(s1);


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
    float2 t : TEXCOORD0;
    float2 t1 : TEXCOORD1;
    float3 Tan : TANGENT;
    float3 vLightDir : LIGHT;
    float   Emi : EMISSIVE;
    float4 TexShadow1 : TEXCOORD2;
    float4 TexShadow2 : TEXCOORD3;
    float4 FogDist : TEXCOORD4;
    float clip : SV_ClipDistance0;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix g_WorldMatrix : packoffset(c0);
    matrix g_ViewMatrix  : packoffset(c4);
    matrix g_ProjMatrix  : packoffset(c8);
   
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
cbuffer FogCB : register(b4)
{
    float4 g_ClipPlane : packoffset(c0);
    float4 g_vCameraPos : packoffset(c1);
    float4 g_FogColor : packoffset(c2);
    float  g_FogMode : packoffset(c3.x); // = FOGMODE_LINEAR;
    float  g_FogDensity : packoffset(c3.y);
    float  g_FogStart : packoffset(c3.z);
    float  g_FogEnd : packoffset(c3.w);
};

VS_OUT VS(VS_IN vIn)
{
    VS_OUT vOut = (VS_OUT) 0;
    float4 vWorld = mul(vIn.Pos, g_WorldMatrix);
    float4 vView = mul(vWorld, g_ViewMatrix);
    float4 vProj = mul(vView, g_ProjMatrix);
    vOut.Pos = vProj;
    vOut.t = vIn.Tex;
    vOut.t1 = vIn.Tex * 50;
    float3 vNormal = normalize(mul(vIn.Nor, (float3x3) g_WorldMatrix));
    vOut.Col = saturate(dot(vIn.Nor, -LightDirection));
    float3 T = normalize(mul(vIn.Tan, (float3x3) g_WorldMatrix));
    float3 B = normalize(cross(vNormal, T));
    float3x3 tanMat =
    {
        T.x, B.x, vNormal.x,
		T.y, B.y, vNormal.y,
		T.z, B.z, vNormal.z
    };
   
    vOut.vLightDir = normalize(mul(-LightDirection, tanMat));
    vOut.TexShadow1 = mul(vIn.Pos, g_matShadow1);
    //fog
    vWorld.w = 1;
    vOut.clip = dot(vWorld, g_ClipPlane);
    float vDistance = distance(vWorld.xyz, g_vCameraPos.xyz);
    vOut.FogDist.w = vIn.Pos.y;
    vOut.FogDist.x = g_FogStart;
    vOut.FogDist.y = g_FogEnd;
    vOut.FogDist.z = vDistance;
    
    return vOut;
}

float CalcFogFactor(float4 vValue)
{
    float fogCoeff = 1.0;

    if (FOGMODE_LINEAR == g_FogMode)
    {
        fogCoeff = (vValue.y - vValue.z) / (vValue.y - vValue.x);
    }
    else if (FOGMODE_EXP == g_FogMode)
    {
        fogCoeff = 1.0 / pow(E, vValue.z * g_FogDensity);
    }
    else if (FOGMODE_EXP2 == g_FogMode)
    {
        fogCoeff = 1.0 / pow(E, vValue.z * vValue.z * g_FogDensity * g_FogDensity);
    }

    return clamp(fogCoeff, 0, 1);
}


float4 PS(VS_OUT vIn) : SV_Target
{
    float4 shadow1 = g_txShadow1.Sample(ShadowSS, vIn.TexShadow1.xy / vIn.TexShadow1.w);
    
    float4 vDetail = TexDifuse1.Sample(Sample0, vIn.t1);
    float4 mask = TexDifuse2.Sample(Sample0, vIn.t);
    float4 vColor = TexDifuse3[0].Sample(Sample0, vIn.t1);
    float4 normal = TexDifuse4[0].Sample(Sample0, vIn.t1);
    normal = normalize((normal - 0.5f) * 2.0f);
    float fDot = saturate(dot(normal.xyz, vIn.vLightDir));
    float3 LightColor = diffuse.rgb * fDot;
   
    
    float4 vColor1 = TexDifuse3[1].Sample(Sample0, vIn.t1);
    float4 vColor2 = TexDifuse3[2].Sample(Sample0, vIn.t1);
    float4 vColor3 = TexDifuse3[3].Sample(Sample0, vIn.t1); 
             
    
    float4 vFinalColor = lerp(vDetail, vColor , mask.x);
    vFinalColor = lerp(vFinalColor, vColor1,  mask.y);
    vFinalColor = lerp(vFinalColor, vColor2,  mask.z);
    vFinalColor = lerp(vFinalColor, vColor3,  mask.w);
    vFinalColor = vFinalColor * float4(LightColor.xyz, 1.0f);

    
    //fog
    float fFogDistance = 1.0f - CalcFogFactor(vIn.FogDist);
    float fValue = (-g_ClipPlane.w - vIn.FogDist.w) / (-g_ClipPlane.w - vIn.FogDist.w - 2.0f);
    float fFogHeight = 1.0f - fValue;
    float4 vFogColor = g_FogColor * (fFogDistance + fFogHeight);

    
    if (shadow1.r > 0.01f )
    {
        vFinalColor = vFinalColor * float4(0.5f, 0.5f, 0.5f, 1.0f) * vIn.Col;
        vFinalColor.w = 1.0f;
    }
  
    return vFinalColor + vFogColor * vIn.Col ;
}