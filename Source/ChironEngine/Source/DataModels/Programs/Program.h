#pragma once

#include "DataModels/DX12/RootSignature/RootSignature.h"

class Program
{
public:
    Program() = delete;
    virtual ~Program();

    // ------------- GETTERS ----------------------

    inline ID3DBlob* GetVertex() const;
    inline ID3DBlob* GetPixel() const;
    inline RootSignature* GetRootSignature() const;
    inline ID3D12PipelineState* GetPipelineState() const;
    inline bool IsGraphic() const;

protected:
    Program(const std::string& name, bool isGraphic = true);

    // ------------- INITS ----------------------

    virtual void InitRootSignature() = 0;

    virtual void InitPipelineState() = 0;

    // ------------- CREATORS ----------------------

    void CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDescription);
    // In this state shaders must be loaded
    void CreateGraphicPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* psoDesc);
    void CreateComputePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC* psoDesc);

private:
    D3D_ROOT_SIGNATURE_VERSION GetRootSignatureVersion();

protected:
    std::string _name;

    ComPtr<ID3DBlob> _vertexShader;
    ComPtr<ID3DBlob> _pixelShader;
    ComPtr<ID3DBlob> _computeShader;

    std::unique_ptr<RootSignature> _rootSignature;
    ComPtr<ID3D12PipelineState> _pipelineState;

    bool _isGraphic;

    ID3D12Device5* _device;
};

inline ID3DBlob* Program::GetVertex() const
{
    return _vertexShader.Get();
}

inline ID3DBlob* Program::GetPixel() const
{
    return _pixelShader.Get();
}

inline RootSignature* Program::GetRootSignature() const
{
    return _rootSignature.get();
}

inline ID3D12PipelineState* Program::GetPipelineState() const
{
    return _pipelineState.Get();
}

inline bool Program::IsGraphic() const
{
    return _isGraphic;
}
