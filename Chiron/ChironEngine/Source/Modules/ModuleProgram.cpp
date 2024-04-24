#include "Pch.h"
#include "ModuleProgram.h"

#include "Application.h"

#include "ModuleID3D12.h"

#include "DataModels/Program/Program.h"

ModuleProgram::ModuleProgram()
{
}

ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Init()
{
    _programs.resize(static_cast<int>(ProgramType::SIZE));
    //_programs[static_cast<int>(ProgramType::DEFAULT)] = CreateDefaultProgram();
	
    return true;
}

bool ModuleProgram::CleanUp()
{
    _programs.clear();
    return true;
}

std::unique_ptr<Program> ModuleProgram::CreateProgram(const LPCWSTR& shaderFile, 
    const D3D12_INPUT_ELEMENT_DESC inputElementDescs[], const std::string& name)
{
    std::unique_ptr<Program> program = std::make_unique<Program>(name);
    ComPtr<ID3DBlob> vertexShader = program->GetVertex();
    ComPtr<ID3DBlob> pixelShader = program->GetPixel();
    ComPtr<ID3D12PipelineState> pipelaneState = program->GetPipelaneState();

    auto id3d12 = App->GetModule<ModuleID3D12>();
    ComPtr<ID3D12RootSignature> rootSignature = id3d12->GetRootSignature();
    ComPtr<ID3D12Device> device = id3d12->GetDevice();

    UINT compileFlags = 0;
#ifdef DEBUG
    // Enable better shader debugging with the graphics debugging tools.
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    
    Chiron::Utils::ThrowIfFailed(D3DCompileFromFile(shaderFile, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, 
        &vertexShader, nullptr));
    Chiron::Utils::ThrowIfFailed(D3DCompileFromFile(shaderFile, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, 
        &pixelShader, nullptr));

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, sizeof(inputElementDescs) / sizeof(D3D12_INPUT_ELEMENT_DESC) };
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    Chiron::Utils::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelaneState)));

	return program;
}

std::unique_ptr<Program> ModuleProgram::CreateDefaultProgram()
{
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
    return CreateProgram(L"Shaders/default.hlsl", inputElementDescs, "Default");
}
