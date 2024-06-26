#include "Pch.h"
#include "ProgramDefault.h"

ProgramDefault::ProgramDefault(const std::string& name) : Program(name)
{
    InitRootSignature();
    InitPipelineState();
}

ProgramDefault::~ProgramDefault()
{
}

void ProgramDefault::InitRootSignature()
{
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_ROOT_PARAMETER1 rootParameters[1]{};
    rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription{};
    rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

    CreateRootSignature(rootSignatureDescription);
}

void ProgramDefault::InitPipelineState()
{
    UINT compileFlags = 0;
#ifdef DEBUG
    // Enable better shader debugging with the graphics debugging tools.
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> errorBuff;
    if (FAILED(D3DCompileFromFile(L"Shaders/default.hlsl", nullptr, nullptr, "VSmain", "vs_5_0", compileFlags, 0, 
        &_vertexShader, &errorBuff)))
    {
        OutputDebugStringA((char*)errorBuff->GetBufferPointer());
    }
    
    if (errorBuff)
    {
        errorBuff->Release();
    }

    errorBuff.Reset();

    if (FAILED(D3DCompileFromFile(L"Shaders/default.hlsl", nullptr, nullptr, "PSmain", "ps_5_0", compileFlags, 0, 
        &_pixelShader, &errorBuff)))
    {
        OutputDebugStringA((char*)errorBuff->GetBufferPointer());
    }
    
    if (errorBuff)
    {
        errorBuff->Release();
    }

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    CreateGraphicPipelineState(inputElementDescs, 2);
}
