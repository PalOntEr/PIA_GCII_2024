Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

cbuffer worldMatrixBuffer : register(b0)
{
    matrix worldMatrix;
}

struct VS_Input
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PS_Input VS_Main(VS_Input vertex)
{
    PS_Input psOut;
    
    psOut.pos = mul(vertex.pos, worldMatrix);
    psOut.uv = vertex.uv;
    
    return psOut;
}

float4 PS_Main(PS_Input frag) : SV_TARGET
{
    float4 textureColor = colorMap.Sample(colorSampler, frag.uv);
    
    if (textureColor.a < 0.9)
    {
        clip(-1);
        return 0;
    }
    
    return textureColor;
}