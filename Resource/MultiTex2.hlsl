
Texture2D g_txDiffuse : register(t0);
Texture2D g_txSandDiffuse : register(t1);
Texture2D g_txGrassDiffuse : register(t2);
Texture2D g_txRockDiffuse : register(t3);
Texture2D g_txSnowDiffuse : register(t4);
SamplerState Sample0 : register(s0);
struct VS_IN
{
	float4 Pos : POSITION;
	float3 Nor : NORMAL;
	float4 Col : COLOR;
    float4 Tex : TEXCOORD;
	
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float4 Col : COLOR;
    float4 Tex : TEXCOORD;

};

cbuffer ConstantBuffer
{
	matrix g_WorldMatrix : packoffset(c0);
	matrix g_ViewMatrix : packoffset(c4);
	matrix g_ProjMatrix : packoffset(c8);
    float g_fZonePlains : packoffset(c12.x);
    float g_fZoneHills : packoffset(c12.y);
    float g_fZoneAlpine : packoffset(c12.z);
    float g_fZoneMountain : packoffset(c12.w);

};

float2 ProjectionTexCoord(float4 proj)
{
    float2 t;
    t = proj.xy / proj.w;
    t.x = t.x * 0.5f + 0.5f;
    t.y = 1.0f - (t.y * 0.5f + 0.5f);
    return t;
}

float2 GenTexCoord(float4 Wolrd)
{
    float2 tex;
    matrix matTex = 0;
	
    matTex._11 = 1.0f / ((128.0f / 9.0f) * 50.0f);
    matTex._32 = -1.0f / ((128.0f / 9.0f) * 50.0f);
	// 타일 개수가 홀수일 경우 사용한다.(1개의 타일의 중심으로 이동시킨다.)
    matTex._41 = 0.5f;
    matTex._42 = 0.5f;
    float4 vView2 = mul(Wolrd, matTex);
    tex = vView2.xy;
    return tex;
}
float2 MatrixTexCoord(float4 vWorld, matrix matTex)
{
    float2 tex;
    float4 vView2 = mul(vWorld, matTex);
    tex = vView2.xy;
    return tex;
}

VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut = (VS_OUT)0;
	float4 vWorld = mul(vIn.Pos, g_WorldMatrix);
	float4 vView = mul(vWorld, g_ViewMatrix);
	float4 vProj = mul(vView, g_ProjMatrix);
	vOut.Pos = vProj;
	vOut.Nor = vIn.Nor;
	vOut.Col = vIn.Col;
	vOut.Tex = vIn.Tex*10;
    vOut.Col.w = clamp(vIn.Pos.y / (255 * 128.0f), 0.0f, 1.0f);
    vOut.Tex.xy = vIn.Tex * 10.0f;
	
    vOut.Tex.zw = ProjectionTexCoord(vProj);
	
    float2 tex;
    
    matrix matTex = 0;
    matTex._11 = 1.0f / ((128.0f / 3.0f) * 50.0f);
    matTex._32 = -1.0f / ((128.0f / 3.0f) * 50.0f);
	// 타일 개수가 홀수일 경우 사용한다.(1개의 타일의 중심으로 이동시킨다.)
    matTex._41 = 0.5f;
    matTex._42 = 0.5f;
    vOut.Tex.zw = MatrixTexCoord(vWorld, matTex);
    
	return vOut;
}

float4 TexturePresenceSand(in float2 texCoord,
	in float lowB, //-0.1
	in float upB, // 0.3
	in float opt, // 0.0f
	in float elev) //0.3
{
	//  e < -0.1f || e > 0.3f
    if ((elev < lowB) || (elev > upB))
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float4 k = (elev <= opt) ? smoothstep(lowB, opt, elev) : smoothstep(upB, opt, elev);

    return k * g_txSandDiffuse.Sample(Sample0, texCoord); // tex2D(SandTextureSampler, texCoord);
}

float4 TexturePresenceGrass(in float2 texCoord,
	in float lowB,
	in float upB,
	in float opt,
	in float elev)
{
    if ((elev < lowB) || (elev > upB))
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float4 k = (elev <= opt) ? smoothstep(lowB, opt, elev) : smoothstep(upB, opt, elev);

    return k * g_txGrassDiffuse.Sample(Sample0, texCoord); //tex2D(GrassTextureSampler, texCoord);
}

float4 TexturePresenceRock(in float2 texCoord,
	in float lowB,
	in float upB,
	in float opt,
	in float elev)
{
    if ((elev < lowB) || (elev > upB))
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float4 k = (elev <= opt) ? smoothstep(lowB, opt, elev) : smoothstep(upB, opt, elev);

    return k * g_txRockDiffuse.Sample(Sample0, texCoord); //tex2D(RockTextureSampler, texCoord);
}

float4 TexturePresenceSnow(in float2 texCoord,
	in float lowB,
	in float upB,
	in float opt,
	in float elev)
{
    if ((elev < lowB) || (elev > upB))
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float4 k = (elev <= opt) ? smoothstep(lowB, opt, elev) : smoothstep(upB, opt, elev);

    return k * g_txSnowDiffuse.Sample(Sample0, texCoord); //tex2D(SnowTextureSampler, texCoord);
}



float4 PS(VS_OUT vIn) : SV_Target
{
   
    float4 vColor = float4(0, 0, 0, 0);
    vColor = TexturePresenceSand(vIn.Tex.xy, -0.1f, g_fZoneHills, g_fZonePlains, vIn.Col.w);
    vColor += TexturePresenceGrass(vIn.Tex.xy, g_fZonePlains, g_fZoneAlpine, g_fZoneHills, vIn.Col.w);
    vColor += TexturePresenceRock(vIn.Tex.xy, g_fZoneHills, g_fZoneMountain, g_fZoneAlpine, vIn.Col.w);
    vColor += TexturePresenceSnow(vIn.Tex.xy, g_fZoneAlpine, 100.1f, g_fZoneMountain, vIn.Col.w);

    vColor *= vIn.Col;
    vColor.a = 1.0f;

    float4 vBase = g_txDiffuse.Sample(Sample0, vIn.Tex.zw) * (1.0f - vIn.Col.w);
    vBase.a = 1.0f;
    return lerp(vBase * vColor, vColor, vIn.Col.w);
	
    

}