struct ModelViewProjection
{
    matrix MVP;
};
ConstantBuffer<ModelViewProjection> myCB1 : register(b0);

struct VertexPosColor
{
    float3 Position : POSITION;
    float4 Color    : COLOR;
};

struct VertexShaderOutput
{
    float4 Color    : COLOR;
    float4 Position : SV_Position;
};

VertexShaderOutput VSmain(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(myCB1.MVP, float4(IN.Position, 1.0f));
    OUT.Color = IN.Color;

    return OUT;
}

struct PixelShaderInput
{
    float4 Color : COLOR;
};

float4 PSmain(PixelShaderInput IN) : SV_Target
{
    return IN.Color;
}