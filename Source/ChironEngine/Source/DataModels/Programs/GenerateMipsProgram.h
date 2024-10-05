#pragma once
#include "Program.h"

#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocation.h"

struct MipsConfiguration
{
    uint32_t SrcMipLevel; // Texture level of source mip
    uint32_t NumMipLevels; // Number of OutMips to write: [1, 4]
    uint32_t IsSRGB; // Number of OutMips to write: [1, 4]
    uint32_t DimensionCase; // Number of OutMips to write: [1, 4]
    Vector2 TexelSize; // 1.0 / OutMip1.Dimensions
};

enum class GenerateMipsRootParameters
{
    MIPS_CONFIGURATION,
    SRC_MIP,
    OUT_MIP,
    SIZE
};

class GenerateMipsProgram : public Program
{
public:
    GenerateMipsProgram() = delete;
    GenerateMipsProgram(const std::string& name);
    ~GenerateMipsProgram() override;

    inline const DescriptorAllocation& GetDescriptorAllocation() const;

protected:
    void InitRootSignature() override;
    void InitPipelineState() override;

private:
    DescriptorAllocation _descriptorAllocation;
};

inline const DescriptorAllocation& GenerateMipsProgram::GetDescriptorAllocation() const
{
    return _descriptorAllocation;
}