// ------------- VERTEX SHADER ----------------------

struct ModelViewProjection
{
    matrix model;
    matrix view;
    matrix projection;
};
ConstantBuffer<ModelViewProjection> myCB0 : register(b0);

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

VS_OUTPUT VSmain(VS_INPUT input)
{
    VS_OUTPUT OUT;
    float4 position = float4(input.position, 1.0f);
    matrix mvp = mul(myCB0.model, mul(myCB0.view, myCB0.projection));
    OUT.position = mul(position, mvp);
    OUT.texCoord = input.texCoord;

    return OUT;
}

// ------------- PIXEL SHADER ----------------------

Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 PSmain(PS_INPUT input) : SV_Target
{
    float2 coord = float2(input.texCoord.x, 1.0f - input.texCoord.y);
    return t1.Sample(s1, coord);
}