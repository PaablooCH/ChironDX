#pragma once

#include "DataModels/DX12/RootSignature/RootSignature.h"

class Program
{
public:
	Program() = delete;
	Program(const std::string& name);
	virtual ~Program();

	// ------------- GETTERS ----------------------

	inline ID3DBlob* GetVertex();
	inline ID3DBlob* GetPixel();
	inline ID3D12RootSignature* GetRootSignature();
	inline ID3D12PipelineState* GetPipelineState();
protected:

	// ------------- INITS ----------------------

	virtual void InitRootSignature() {}

	virtual void InitPipelineState() {}

	// ------------- CREATORS ----------------------

	void CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDescription);
	// In this state shaders must be loaded
	void CreateGraphicPipelineState(const D3D12_INPUT_ELEMENT_DESC inputElementDescs[], UINT elements,
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));

private:
	D3D_ROOT_SIGNATURE_VERSION GetRootSignatureVersion();

protected:
	std::string _name;

	ComPtr<ID3DBlob> _vertexShader;
	ComPtr<ID3DBlob> _pixelShader;
	
	std::unique_ptr<RootSignature> _rootSignature;
	ComPtr<ID3D12PipelineState> _pipelineState;
};

inline ID3DBlob* Program::GetVertex()
{
	return _vertexShader.Get();
}

inline ID3DBlob* Program::GetPixel()
{
	return _pixelShader.Get();
}

inline ID3D12RootSignature* Program::GetRootSignature()
{
	return _rootSignature->GetRootSignature();
}

inline ID3D12PipelineState* Program::GetPipelineState()
{
	return _pipelineState.Get();
}
