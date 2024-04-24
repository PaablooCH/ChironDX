#pragma once

#include <d3dx12.h>

class Program
{
public:
	Program() = delete;
	Program(const std::string& name);
	~Program() = default;

	inline ComPtr<ID3DBlob> GetVertex();
	inline ComPtr<ID3DBlob> GetPixel();
	inline ComPtr<ID3D12PipelineState> GetPipelaneState();
private:
	std::string _name;

	ComPtr<ID3DBlob> _vertexShader;
	ComPtr<ID3DBlob> _pixelShader;
	
	ComPtr<ID3D12PipelineState> _pipelaneState;
};

inline ComPtr<ID3DBlob> Program::GetVertex()
{
	return _vertexShader;
}

inline ComPtr<ID3DBlob> Program::GetPixel()
{
	return _pixelShader;
}

inline ComPtr<ID3D12PipelineState> Program::GetPipelaneState()
{
	return _pipelaneState;
}
