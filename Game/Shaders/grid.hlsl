// Adapted from https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid

// ------------- BUFFERS ----------------------

struct Camera
{
    matrix view;
    matrix projection;
    float3 position;
    float pad; // padding
};
ConstantBuffer<Camera> camera : register(b0);

struct CameraInverse
{
    matrix viewInv;
    matrix projectionInv;
};
ConstantBuffer<CameraInverse> cameraInv : register(b1);

// ------------- VERTEX SHADER ----------------------

struct VS_OUTPUT
{
    float3 farPoint : FAR_POINT;
    float3 nearPoint : NEAR_POINT;
    float4 position : SV_Position;
};

float3 UnprojectPoint(float x, float y, float z, matrix viewInv, matrix projInv)
{
    float4 unprojectedPoint = mul(float4(x, y, z, 1.0f), mul(projInv, viewInv));
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

static const float3 gridPlane[6] =
{
    float3(1, 1, 0),
    float3(-1, 1, 0),
    float3(-1, -1, 0),
    float3(-1, -1, 0),
    float3(1, -1, 0),
    float3(1, 1, 0)
};

VS_OUTPUT VSmain(uint vertexID : SV_VertexID)
{   
    VS_OUTPUT OUT;

    float3 p = gridPlane[vertexID];
    
    OUT.nearPoint = UnprojectPoint(p.x, p.y, 0.0, cameraInv.viewInv, cameraInv.projectionInv).xyz; // unprojecting on the near plane
    OUT.farPoint = UnprojectPoint(p.x, p.y, 1.0, cameraInv.viewInv, cameraInv.projectionInv).xyz; // unprojecting on the far plane
    
    OUT.position = float4(p, 1.0);
    return OUT;
}

// ------------- PIXEL SHADER ----------------------

struct NearFarPlane
{
    float nearPlane;
    float farPlane;
};
ConstantBuffer<NearFarPlane> nearFarPlane : register(b2);

float4 grid(float3 fragPos3D, float scale, bool drawAxis)
{
    float2 coord = fragPos3D.xz * scale;
    float2 derivative = max(fwidth(coord), 1e-5);
    float2 grid = abs(frac(coord - 0.5) - 0.5) / derivative;
    float lineGrid = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    float4 color = float4(0.2, 0.2, 0.2, 1.0 - min(lineGrid, 1.0));
    // z axis
    if (fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
    {
        color.z = 1.0;
    }
    // x axis
    if (fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
    {
        color.x = 1.0;
    }
    return color;
}

float computeDepth(float3 pos, matrix projection, matrix view)
{
    float4 clip_space_pos = mul(float4(pos, 1.0f), mul(view, projection));
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(float3 pos, matrix projection, matrix view)
{
    float4 clipPos = mul(float4(pos, 1.0f), mul(view, projection));
    float clipSpaceDepth = (clipPos.z / clipPos.w) * 2.0f - 1.0f;
    float linearDepth = (2.0f * nearFarPlane.nearPlane * nearFarPlane.farPlane) / 
                        (nearFarPlane.farPlane + nearFarPlane.nearPlane - clipSpaceDepth * (nearFarPlane.farPlane - nearFarPlane.nearPlane));
    return linearDepth / nearFarPlane.farPlane; // normalize [0,1]
}

struct PS_INPUT
{
    float3 farPoint : FAR_POINT;
    float3 nearPoint : NEAR_POINT;
};

float4 PSmain(PS_INPUT input, out float depth : SV_Depth) : SV_Target
{       
    float t = -input.nearPoint.y / (input.farPoint.y - input.nearPoint.y);
    float3 fragPos3D = input.nearPoint + t * (input.farPoint - input.nearPoint);

    depth = computeDepth(fragPos3D, camera.projection, camera.view);

    float linearDepth = computeLinearDepth(fragPos3D, camera.projection, camera.view);
    float fading = max(0, (0.5 - linearDepth));

    float4 color = (grid(fragPos3D, 10, true) + grid(fragPos3D, 1, true)) * float(t > 0); // adding multiple resolution for the grid
    color.a *= fading;
    
    return color;
}