#include "Pch.h"
#include "GenerateMipsProgram.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"

GenerateMipsProgram::GenerateMipsProgram(const std::string& name) : Program(name, false)
{
    InitRootSignature();
    InitPipelineState();

    auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    _descriptorAllocation = App->GetModule<ModuleID3D12>()->
        GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(4);
    for (uint32_t i = 0; i < 4; ++i)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        uavDesc.Texture2D.MipSlice = i;
        uavDesc.Texture2D.PlaneSlice = 0;

        device->CreateUnorderedAccessView(nullptr, nullptr, &uavDesc, _descriptorAllocation.GetCPUDescriptorHandle(i));
    }
}

GenerateMipsProgram::~GenerateMipsProgram()
{
}

void GenerateMipsProgram::InitRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE1 srcMip(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0,
        D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
    CD3DX12_DESCRIPTOR_RANGE1 outMip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 4, 0, 0,
        D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

    CD3DX12_ROOT_PARAMETER1 rootParameters[static_cast<int>(GenerateMipsRootParameters::SIZE)]{};
    rootParameters[static_cast<int>(GenerateMipsRootParameters::MIPS_CONFIGURATION)].InitAsConstants(sizeof(MipsConfiguration) / 4, 0);
    rootParameters[static_cast<int>(GenerateMipsRootParameters::SRC_MIP)].InitAsDescriptorTable(1, &srcMip);
    rootParameters[static_cast<int>(GenerateMipsRootParameters::OUT_MIP)].InitAsDescriptorTable(1, &outMip);

    CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription(static_cast<int>(GenerateMipsRootParameters::SIZE), 
        rootParameters, 1, &linearClampSampler);

    CreateRootSignature(rootSignatureDescription.Desc_1_1);
    _rootSignature->GetID3D12RootSignature()->SetName(L"Generate Mips Root Signature");
}

void GenerateMipsProgram::InitPipelineState()
{
    UINT compileFlags = 0;
#ifdef DEBUG
    // Enable better shader debugging with the graphics debugging tools.
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    ComPtr<ID3DBlob> errorBuff;
    if (FAILED(D3DCompileFromFile(L"Shaders/generate_mips_cs.hlsl", nullptr, nullptr, "main", "cs_5_1", compileFlags, 0,
        &_computeShader, &errorBuff)))
    {
        OutputDebugStringA((char*)errorBuff->GetBufferPointer());
    }

    if (errorBuff)
    {
        errorBuff = nullptr;
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = _rootSignature->GetID3D12RootSignature();
    psoDesc.CS = CD3DX12_SHADER_BYTECODE(_computeShader.Get());

    CreateComputePipelineState(&psoDesc);
    _pipelineState->SetName(L"Generate Mips Pipeline");
}
