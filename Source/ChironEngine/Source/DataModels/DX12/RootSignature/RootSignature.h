#pragma once

class RootSignature
{
public:
    RootSignature();
    RootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion);

    ~RootSignature();

    void SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, 
        D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion);

    void Destroy();

    // ------------- GETTERS ----------------------

    inline ID3D12RootSignature* GetRootSignature();
    inline const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const;
    inline uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const;
    inline uint32_t GetNumDescriptors(uint32_t rootIndex) const;

private:
    ComPtr<ID3D12RootSignature> _rootSignature;
    D3D12_ROOT_SIGNATURE_DESC1 _rootSignatureDesc;

    // 32 because bit mask has 32 bits to store data.
    uint32_t _numDescriptorsPerTable[32];

    // Bit mask that represents the indices that are descriptor tables for Samplers.
    uint32_t _samplerTableBitMask;
    // Bit mask that represents the indices that are CBV, UAV, and SRV descriptor tables.
    uint32_t _descriptorTableBitMask;
};

inline ID3D12RootSignature* RootSignature::GetRootSignature()
{
    return _rootSignature.Get();
}

inline const D3D12_ROOT_SIGNATURE_DESC1& RootSignature::GetRootSignatureDesc() const
{
    return _rootSignatureDesc;
}

inline uint32_t RootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const
{
    switch (descriptorHeapType)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        return _descriptorTableBitMask;
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        return _samplerTableBitMask;
    }
    return 0;
}

inline uint32_t RootSignature::GetNumDescriptors(uint32_t rootIndex) const
{
    return _numDescriptorsPerTable[rootIndex];
}

