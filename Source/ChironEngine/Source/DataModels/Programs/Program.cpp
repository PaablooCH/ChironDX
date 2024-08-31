#include "Pch.h"
#include "Program.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

Program::Program(const std::string& name, bool isGraphic) : _name(name), _rootSignature(std::make_unique<RootSignature>()),
_isGraphic(isGraphic)
{
}

Program::~Program()
{
}

void Program::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDescription)
{
    _rootSignature->SetRootSignatureDesc(rootSignatureDescription, GetRootSignatureVersion());
}

void Program::CreateGraphicPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc)
{
    auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    Chiron::Utils::ThrowIfFailed(device->CreateGraphicsPipelineState(psoDesc, IID_PPV_ARGS(&_pipelineState)));
}

void Program::CreateComputePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC* psoDesc)
{
    auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    Chiron::Utils::ThrowIfFailed(device->CreateComputePipelineState(psoDesc, IID_PPV_ARGS(&_pipelineState)));
}

D3D_ROOT_SIGNATURE_VERSION Program::GetRootSignatureVersion()
{
    auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    // Check compatibility
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
    return featureData.HighestVersion;
}