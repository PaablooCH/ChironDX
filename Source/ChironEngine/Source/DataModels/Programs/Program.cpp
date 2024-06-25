#include "Pch.h"
#include "Program.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

Program::Program(const std::string& name) : _name(name), _rootSignature(std::make_unique<RootSignature>())
{
}

Program::~Program()
{
}

void Program::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDescription)
{
    _rootSignature->SetRootSignatureDesc(rootSignatureDescription, GetRootSignatureVersion());
}

void Program::CreateGraphicPipelineState(const D3D12_INPUT_ELEMENT_DESC inputElementDescs[], UINT elements, 
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc)
{
    auto device = App->GetModule<ModuleID3D12>()->GetDevice();

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, elements };
    psoDesc.pRootSignature = _rootSignature->GetID3D12RootSignature();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(_vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(_pixelShader.Get());
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc = { 1, 0 };
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.NumRenderTargets = 1;
    psoDesc.DepthStencilState = depthStencilDesc;

    Chiron::Utils::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState)));
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