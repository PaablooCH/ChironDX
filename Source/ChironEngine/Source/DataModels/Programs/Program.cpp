#include "Pch.h"
#include "Program.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

Program::Program(const std::string& name) : _name(name)
{
}

Program::~Program()
{
}

void Program::CreateRootSignature(CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription)
{
    auto id3d12 = App->GetModule<ModuleID3D12>();
    ComPtr<ID3D12Device> device = id3d12->GetDevice();

    // Check compatibility
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = GetRootSignatureVersion();

    ComPtr<ID3DBlob> rootSignatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    // Compile root signature
    Chiron::Utils::ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
        featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
    // Create the root signature.
    Chiron::Utils::ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&_rootSignature)));
    _rootSignature->SetName(L"Default Root Signature");
}

void Program::CreateGraphicPipelineState(const D3D12_INPUT_ELEMENT_DESC inputElementDescs[], UINT elements)
{
    auto d3d12 = App->GetModule<ModuleID3D12>();
    auto device = d3d12->GetDevice();
    
    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, elements };
    psoDesc.pRootSignature = _rootSignature.Get();
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

    Chiron::Utils::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState)));
    _pipelineState->SetName(L"Default Pipeline");
}

D3D12_FEATURE_DATA_ROOT_SIGNATURE Program::GetRootSignatureVersion()
{
    auto d3d12 = App->GetModule<ModuleID3D12>();
    auto device = d3d12->GetDevice();

    // Check compatibility
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
    return featureData;
}