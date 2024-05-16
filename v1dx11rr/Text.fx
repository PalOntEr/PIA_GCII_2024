Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

cbuffer constantBuffer : register(b0)
{
    matrix worldMatrix;
}

cbuffer colorText : register(b1)
{
    float4 color;
};

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

float4 PS_Main(PS_Input vertex) : SV_TARGET
{
    float4 textura = colorMap.Sample(colorSampler, vertex.uv);
    
    return (textura + color) * textura.a;

}