// ------------- BUFFERS ----------------------

struct Camera
{
    matrix view;
    matrix projection;
    float3 position;
};
ConstantBuffer<Camera> camera : register(b0);

// ------------- VERTEX SHADER ----------------------

struct VS_OUTPUT
{
    float3 worldPos : POSITION;
    float4 position : SV_Position;
};

VS_OUTPUT VSmain(uint vertexID : SV_VertexID)
{
    const float3 quad[4] =
    {
        float3(-1.0f, 0.f, -1.0f), // left bottom
        float3(1.0f, 0.f, -1.0f), // right bottom
        float3(-1.0f, 0.f, 1.0f), // left top
        float3(1.0f, 0.f, 1.0f), // right top
    };

    const int indices[6] = { 0, 2, 3, 3, 1, 0 };
    
    // -----------------------------------
    
    VS_OUTPUT OUT;
    
    int index = indices[vertexID];
    float3 worldPos = quad[index];
    worldPos.x = worldPos.x + camera.position.x;
    worldPos.z = worldPos.z + camera.position.z;
    float4 position = float4(worldPos, 1.0f);
    
    matrix mvp = mul(camera.view, camera.projection);
    
    OUT.worldPos = worldPos;
    OUT.position = mul(position, mvp);
    return OUT;
}

// ------------- PIXEL SHADER ----------------------

struct PS_INPUT
{
    float3 worldPos : POSITION;
};

float4 PSmain(PS_INPUT input) : SV_Target
{
    return float4(input.worldPos, 1.0f);
}