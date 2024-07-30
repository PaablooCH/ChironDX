#define WIDTH_EVEN_HEIGHT_EVEN 0
#define WIDTH_ODD_HEIGHT_EVEN 1
#define WIDTH_EVEN_HEIGHT_ODD 2
#define WIDTH_ODD_HEIGHT_ODD 3

#define Common_RootSig \
    "RootFlags(0), " \
    "RootConstants(b0, num32BitConstants = 4)," \
    "DescriptorTable(SRV(t0, numDescriptors = 1))," \
    "DescriptorTable(UAV(u0, numDescriptors = 4))," \
    "StaticSampler(s0," \
        "addressU = TEXTURE_ADDRESS_CLAMP," \
        "addressV = TEXTURE_ADDRESS_CLAMP," \
        "addressW = TEXTURE_ADDRESS_CLAMP," \
        "filter = FILTER_MIN_MAG_MIP_LINEAR)"

Texture2D<float4> SrcMip : register(t0);

RWTexture2D<float4> OutMip1 : register(u0);
RWTexture2D<float4> OutMip2 : register(u1);
RWTexture2D<float4> OutMip3 : register(u2);
RWTexture2D<float4> OutMip4 : register(u3);

SamplerState BilinearClamp : register(s0);

struct MipsConfiguration
{
    uint SrcMipLevel; // Texture level of source mip
    uint NumMipLevels; // Number of OutMips to write: [1, 4]
    bool IsSRGB;
    uint DimensionCase;
    float2 TexelSize; // 1.0 / OutMip1.Dimensions
};
ConstantBuffer<MipsConfiguration> cb0 : register(b0);

// The reason for separating channels is to reduce bank conflicts in the
// local data memory controller.  A large stride will cause more threads
// to collide on the same memory bank.
groupshared float gsR[64];
groupshared float gsG[64];
groupshared float gsB[64];
groupshared float gsA[64];

void StoreColor(uint index, float4 color)
{
    gsR[index] = color.r;
    gsG[index] = color.g;
    gsB[index] = color.b;
    gsA[index] = color.a;
}

float4 LoadColor(uint index)
{
    return float4(gsR[index], gsG[index], gsB[index], gsA[index]);
}

float3 ApplySRGBCurve(float3 x)
{
    // This is exactly the sRGB curve
    //return select(x < 0.0031308, 12.92 * x, 1.055 * pow(abs(x), 1.0 / 2.4) - 0.055);
     
    // This is cheaper but nearly equivalent
    return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(abs(x - 0.00228)) - 0.13448 * x + 0.005719;
}

float4 PackColor(float4 Linear)
{
    if (cb0.IsSRGB)
    {
        return float4(ApplySRGBCurve(Linear.rgb), Linear.a);
    }
    else
    {
        return Linear;
    }
}

[RootSignature(Common_RootSig)]
[numthreads( 8, 8, 1 )]
void main(uint groupIndex : SV_GroupIndex, uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float4 src1;
    // One bilinear sample is insufficient when scaling down by more than 2x.
    // You will slightly undersample in the case where the source dimension
    // is odd.  This is why it's a really good idea to only generate mips on
    // power-of-two sized textures.  Trying to handle the undersampling case
    // will force this shader to be slower and more complicated as it will
    // have to take more source texture samples.
    switch (cb0.DimensionCase)
    {
    case WIDTH_EVEN_HEIGHT_EVEN:
        float2 UV = cb0.TexelSize * (dispatchThreadID.xy + 0.5);
        src1 = SrcMip.SampleLevel(BilinearClamp, UV, cb0.SrcMipLevel);
        break;
    case WIDTH_ODD_HEIGHT_EVEN:
        // > 2:1 in X dimension
        // Use 2 bilinear samples to guarantee we don't undersample when downsizing by more than 2x horizontally.
        float2 uv1_1 = cb0.TexelSize * (dispatchThreadID.xy + float2(0.25, 0.5));
        float2 off_1 = cb0.TexelSize * float2(0.5, 0.0);
        src1 = 0.5 * (SrcMip.SampleLevel(BilinearClamp, uv1_1, cb0.SrcMipLevel) +
        SrcMip.SampleLevel(BilinearClamp, uv1_1 + off_1, cb0.SrcMipLevel));
        break;
    case WIDTH_EVEN_HEIGHT_ODD:
        // > 2:1 in Y dimension
        // Use 2 bilinear samples to guarantee we don't undersample when downsizing by more than 2x vertically.
        float2 uv1_2 = cb0.TexelSize * (dispatchThreadID.xy + float2(0.5, 0.25));
        float2 off_2 = cb0.TexelSize * float2(0.0, 0.5);
        src1 = 0.5 * (SrcMip.SampleLevel(BilinearClamp, uv1_2, cb0.SrcMipLevel) +
        SrcMip.SampleLevel(BilinearClamp, uv1_2 + off_2, cb0.SrcMipLevel));
        break;
    case WIDTH_ODD_HEIGHT_ODD:
        // > 2:1 in in both dimensions
        // Use 4 bilinear samples to guarantee we don't undersample when downsizing by more than 2x in both directions.
        float2 uv1_3 = cb0.TexelSize * (dispatchThreadID.xy + float2(0.25, 0.25));
        float2 off_3 = cb0.TexelSize * 0.5;
        src1 = SrcMip.SampleLevel(BilinearClamp, uv1_3, cb0.SrcMipLevel);
        src1 += SrcMip.SampleLevel(BilinearClamp, uv1_3 + float2(off_3.x, 0.0), cb0.SrcMipLevel);
        src1 += SrcMip.SampleLevel(BilinearClamp, uv1_3 + float2(0.0, off_3.y), cb0.SrcMipLevel);
        src1 += SrcMip.SampleLevel(BilinearClamp, uv1_3 + float2(off_3.x, off_3.y), cb0.SrcMipLevel);
        src1 *= 0.25;
        break;
    }
    OutMip1[dispatchThreadID.xy] = PackColor(src1);

    // A scalar (constant) branch can exit all threads coherently.
    if (cb0.NumMipLevels == 1)
    {
        return;
    }

    // Without lane swizzle operations, the only way to share data with other
    // threads is through LDS.
    StoreColor(groupIndex, src1);

    // This guarantees all LDS writes are complete and that all threads have
    // executed all instructions so far (and therefore have issued their LDS
    // write instructions.)
    GroupMemoryBarrierWithGroupSync();

    // With low three bits for X and high three bits for Y, this bit mask
    // (binary: 001001) checks that X and Y are even.
    if ((groupIndex & 0x9) == 0)
    {
        float4 src2 = LoadColor(groupIndex + 0x01);
        float4 src3 = LoadColor(groupIndex + 0x08);
        float4 src4 = LoadColor(groupIndex + 0x09);
        src1 = 0.25 * (src1 + src2 + src3 + src4);

        OutMip2[dispatchThreadID.xy / 2] = PackColor(src1);
        StoreColor(groupIndex, src1);
    }

    if (cb0.NumMipLevels == 2)
    {
        return;
    }

    GroupMemoryBarrierWithGroupSync();

    // This bit mask (binary: 011011) checks that X and Y are multiples of four.
    if ((groupIndex & 0x1B) == 0)
    {
        float4 src2 = LoadColor(groupIndex + 0x02);
        float4 src3 = LoadColor(groupIndex + 0x10);
        float4 src4 = LoadColor(groupIndex + 0x12);
        src1 = 0.25 * (src1 + src2 + src3 + src4);

        OutMip3[dispatchThreadID.xy / 4] = PackColor(src1);
        StoreColor(groupIndex, src1);
    }

    if (cb0.NumMipLevels == 3)
    {
        return;
    }

    GroupMemoryBarrierWithGroupSync();

    // This bit mask would be 111111 (X & Y multiples of 8), but only one
    // thread fits that criteria.
    if (groupIndex == 0)
    {
        float4 src2 = LoadColor(groupIndex + 0x04);
        float4 src3 = LoadColor(groupIndex + 0x20);
        float4 src4 = LoadColor(groupIndex + 0x24);
        src1 = 0.25 * (src1 + src2 + src3 + src4);

        OutMip4[dispatchThreadID.xy / 8] = PackColor(src1);
    }
}
