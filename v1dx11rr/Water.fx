Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

cbuffer cbChangerEveryFrame : register(b0)
{
	matrix worldMatrix;
};

cbuffer cbNeverChanges : register(b1)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
{
	matrix viewMatrix;
};

cbuffer cbChangeOnResize : register(b2)
{
	matrix projMatrix;
};

cbuffer TimerBuffer : register(b3)
{
    float4 timer;
};

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 tangente : NORMAL1;
	float3 binormal : NORMAL2;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 tangent : NORMAL1;
	float3 binorm : NORMAL2;
};

float getWaveValue(float x)
{
    return exp(0.1f * sin(x + timer.x / 20.0f) + 0.05f * sin(2.0f * x + timer.x / 10.0f) + 0.025f * sin(4.0f * x + timer.x / 30.0f) * 0.25f) + 0.05f * -sin(2.0f * x + timer.x / 10.0f);
}

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	
    vertex.pos.y += exp(0.1f * sin(vertex.pos.x + timer.x / 20.0f)
						+ 0.05f * sin(2.0f * vertex.pos.x + timer.x / 10.0f)
						+ 0.025f * sin(4.0f * vertex.pos.x + timer.x / 30.0f));
	
    vertex.pos.y += exp(0.1f * sin(vertex.pos.z + timer.x / 20.0f)
						+ 0.05f * sin(2.0f * vertex.pos.z + timer.x / 10.0f)
						+ 0.04f * -sin(3.0f * vertex.pos.z + timer.x / 30.0f));
	
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;
	vsOut.normal = normalize(mul(vertex.normal, worldMatrix));
	vsOut.tangent = normalize(mul(vertex.tangente, worldMatrix));
	vsOut.binorm = normalize(mul(vertex.binormal, worldMatrix));

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
	float4 fColor = float4(1,0,0,1);

	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	float4 text = colorMap.Sample(colorSampler, pix.tex0 * 0.5f);
    
    float3 DiffuseDirection = float3(0.0f, -1.0f, 0.0f);
    if (timer.y > 0)
    {
        DiffuseDirection = float3(timer.x, -1.0f, 0.0f);
    }
    else
    {
        DiffuseDirection = float3(timer.x * -1, -1.0f, 0.0f);
    }
	
    float4 DiffuseColor = float4((timer.x - 1) * -1, (timer.x - 1) * -1, (timer.x - 1) * -1, (timer.x - 1) * -1);

	float3 diffuse = dot(-DiffuseDirection, pix.normal);
	diffuse = saturate(diffuse*DiffuseColor.rgb);
	diffuse = saturate(diffuse + ambient);

    fColor = float4(text.rgb * diffuse, 0.5f);

	return fColor;
}