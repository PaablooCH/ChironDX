#include "Pch.h"
#include "RootSignature.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

RootSignature::RootSignature() : _rootSignature(nullptr), _rootSignatureDesc({}), _numDescriptorsPerTable{ 0 }, 
_samplerTableBitMask(0), _descriptorTableBitMask(0)
{
}

RootSignature::RootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, 
	D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion)
{
	SetRootSignatureDesc(rootSignatureDesc, rootSignatureVersion);
}

RootSignature::~RootSignature()
{
    Destroy();
}

void RootSignature::SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, 
	D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion)
{
    Destroy();

    auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    UINT numParameters = rootSignatureDesc.NumParameters;
    D3D12_ROOT_PARAMETER1* pParameters = numParameters > 0 ? new D3D12_ROOT_PARAMETER1[numParameters] : nullptr;

    for (UINT i = 0; i < numParameters; ++i)
    {
        const D3D12_ROOT_PARAMETER1& rootParameter = rootSignatureDesc.pParameters[i];
        pParameters[i] = rootParameter;

        if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            UINT numDescriptorRanges = rootParameter.DescriptorTable.NumDescriptorRanges;
            D3D12_DESCRIPTOR_RANGE1* pDescriptorRanges = 
                numDescriptorRanges > 0 ? new D3D12_DESCRIPTOR_RANGE1[numDescriptorRanges] : nullptr;

            if (pDescriptorRanges)
            {
                memcpy(pDescriptorRanges, rootParameter.DescriptorTable.pDescriptorRanges,
                    sizeof(D3D12_DESCRIPTOR_RANGE1) * numDescriptorRanges);
            }
            
            pParameters[i].DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
            pParameters[i].DescriptorTable.pDescriptorRanges = pDescriptorRanges;

            // Set the bit mask depending on the type of descriptor table.
            if (numDescriptorRanges > 0)
            {
                switch (pDescriptorRanges[0].RangeType)
                {
                case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
                case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
                    _descriptorTableBitMask |= (1 << i);
                    break;
                case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
                    _samplerTableBitMask |= (1 << i);
                    break;
                }
            }

            // Count the number of descriptors in the descriptor table.
            for (UINT j = 0; j < numDescriptorRanges; ++j)
            {
                _numDescriptorsPerTable[i] += pDescriptorRanges[j].NumDescriptors;
            }
        }
    }

    _rootSignatureDesc.NumParameters = numParameters;
    _rootSignatureDesc.pParameters = pParameters;

    UINT numStaticSamplers = rootSignatureDesc.NumStaticSamplers;
    D3D12_STATIC_SAMPLER_DESC* pStaticSamplers = 
        numStaticSamplers > 0 ? new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers] : nullptr;

    if (pStaticSamplers)
    {
        memcpy(pStaticSamplers, rootSignatureDesc.pStaticSamplers, sizeof(D3D12_STATIC_SAMPLER_DESC) * numStaticSamplers);
    }

    _rootSignatureDesc.NumStaticSamplers = numStaticSamplers;
    _rootSignatureDesc.pStaticSamplers = pStaticSamplers;

    D3D12_ROOT_SIGNATURE_FLAGS flags = rootSignatureDesc.Flags;
    _rootSignatureDesc.Flags = flags;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionRootSignatureDesc{};
    versionRootSignatureDesc.Init_1_1(numParameters, pParameters, numStaticSamplers, pStaticSamplers, flags);

    // Serialize the root signature.
    ComPtr<ID3DBlob> rootSignatureBlob;
    ComPtr<ID3DBlob> errorBlob;

    Chiron::Utils::ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&versionRootSignatureDesc,
        rootSignatureVersion, &rootSignatureBlob, &errorBlob));

    // Create the root signature.
    Chiron::Utils::ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&_rootSignature)));
}

void RootSignature::Destroy()
{
    for (UINT i = 0; i < _rootSignatureDesc.NumParameters; ++i)
    {
        const D3D12_ROOT_PARAMETER1& rootParameter = _rootSignatureDesc.pParameters[i];
        if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            delete[] rootParameter.DescriptorTable.pDescriptorRanges;
        }
    }

    delete[] _rootSignatureDesc.pParameters;
    _rootSignatureDesc.pParameters = nullptr;
    _rootSignatureDesc.NumParameters = 0;

    delete[] _rootSignatureDesc.pStaticSamplers;
    _rootSignatureDesc.pStaticSamplers = nullptr;
    _rootSignatureDesc.NumStaticSamplers = 0;

    _descriptorTableBitMask = 0;
    _samplerTableBitMask = 0;

    memset(_numDescriptorsPerTable, 0, sizeof(_numDescriptorsPerTable));
}
