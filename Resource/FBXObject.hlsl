Texture2D TexDifuse1 : register(t0);
Texture2D TexNormal : register(t1);
SamplerState Sample0 : register(s0);


struct VS_IN
{
    float4 Pos : POSITION;
    float3 Nor : NORMAL;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD;
    float3 Tan : TANGENT;
    uint InstanceID : SV_INSTANCEID;
    row_major matrix mTransform : INST;
    float  Emi : EMISSIVE;

};
struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float4 Col : COLOR;
    float2 Tex : TEXCOORD;
    float3 Tan : TANGENT;
    float3 vLightDir : LIGHT;
    float4 wolrdpos : TEXCOORD1;
    float   Emi : EMISSIVE;
    uint instanceID : INST;

};

cbuffer ConstantBuffer
{
	matrix g_WorldMatrix : packoffset(c0);
	matrix g_ViewMatrix : packoffset(c4);
	matrix g_ProjMatrix : packoffset(c8);

};
cbuffer LightBuffer : register(b1)
{
    float4 ambient : packoffset(c0);
    float4 diffuse : packoffset(c1);
    float4 specular : packoffset(c2);

};
cbuffer ChangeEveryFrame : register(b2)
{
    float3 g_CameraPos;
    float  padding0;
    float3 g_CamereaDir;//look
    float  padding1;
    float3 g_LightDirection;
    float  padding2;

};
//VS_OUT VS(VS_IN vIn)
//{
//	VS_OUT vOut = (VS_OUT)0;
//	float4 vWorld = mul(vIn.Pos, g_WorldMatrix);
//	float4 vView = mul(vWorld, g_ViewMatrix);
//	float4 vProj = mul(vView, g_ProjMatrix);
//	vOut.Pos = vProj;
//	vOut.Nor = vIn.Nor;
//	vOut.Col = vIn.Col;
//	vOut.Tex = vIn.Tex;

//	return vOut;
//}
VS_OUT VS(VS_IN vIn)
{
    VS_OUT vOut = (VS_OUT) 0;
   float4 vWorld = mul(vIn.Pos, vIn.mTransform);
    //float4 vWorld = mul(vIn.Pos, g_WorldMatrix);
    float4 vView = mul(vWorld, g_ViewMatrix);
    float4 vProj = mul(vView, g_ProjMatrix);
    vOut.Pos = vProj;
    vOut.Tex = vIn.Tex;
    vOut.wolrdpos = vWorld;
    float3 vNormal = normalize(mul(vIn.Nor, (float3x3) vIn.mTransform));
    vOut.Nor = vNormal;
    vOut.Col = saturate(dot(vNormal, -g_LightDirection));
    vOut.Tan = vIn.Tan;
    vOut.instanceID = vIn.InstanceID;
    float3 T = normalize(mul(vIn.Tan.xyz, (float3x3) vIn.mTransform));
    float3 B = normalize(cross(vNormal, T));
    float3x3 tanMat =
    {
        T.x, B.x, vNormal.x,
		T.y, B.y, vNormal.y,
		T.z, B.z, vNormal.z
    };
   
    vOut.vLightDir = normalize(mul(-g_LightDirection, tanMat));
    vOut.Emi = vIn.Emi;
    return vOut;
}

float4 ComputeRimLight(float3 CameraPos, VS_OUT vIn, float rimsize)
{

    float3 pos = normalize(CameraPos - vIn.wolrdpos.xyz);
   // float ReturnColor = smoothstep(1.0f - rimsize, 1.0f, 1 - max(0, dot(vin.Nor, pos)));
   //
    float fRimLightcolor = smoothstep(1.0f - rimsize, 1.0f, 1.0f - max(0, dot(vIn.Nor, pos)));
    float4 vFinalColor = float4(fRimLightcolor, fRimLightcolor, fRimLightcolor, 1.0f);
    float4 rimcolor = float4(-2.0f, -2.0f, 0.0f, 0.0f);
    return -(vFinalColor * rimcolor);
}

float4 Diffuse(float3 vNormal)
{
    float fIntensity = max(0, dot(vNormal, normalize(-g_LightDirection)));
    float4 _diffuse = float4(0.1f,0.1f,0.1f,0.1f) * ambient +
		(diffuse * ambient * fIntensity);
    return _diffuse;
}

float4 PS(VS_OUT vIn) : SV_Target
{
    float4 color = TexDifuse1.Sample(Sample0, vIn.Tex);
    float4 normal = TexNormal.Sample(Sample0, vIn.Tex);
    normal = normalize((normal - 0.5f) * 2.0f);
    float fDot = saturate(dot(normal.xyz, vIn.vLightDir));
    float3 LightColor = diffuse.rgb * fDot;
  
    //using normalmap
    float4 finalcolor = lerp(vIn.Col, float4(LightColor.xyz, 1), 1);
    if(vIn.Emi < 0.001)
    {
        return color * vIn.Col ;
    }
    else
    {
        float4 rim = ComputeRimLight(g_CameraPos, vIn, 0.3f);
        return color * vIn.Col  + rim;
    }
  

   
	
    //return vIn.Col;
}