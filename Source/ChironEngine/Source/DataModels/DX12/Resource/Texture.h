#pragma once
#include "Resource.h"

class DescriptorAllocation;

class Texture : public Resource
{
public:
	Texture(const D3D12_RESOURCE_DESC& resourceDesc, const std::wstring& name = L"");
	Texture(ComPtr<ID3D12Resource> resource, const std::wstring& name);
	Texture(const Texture& copy);

	virtual ~Texture();

	Texture& operator=(const Texture& other);
	Texture& operator=(Texture&& other);

	void Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize = 1);

	// ------------- GETTERS ----------------------

	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override;

private:
	DescriptorAllocation CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const;

private:
	mutable std::unordered_map<size_t, DescriptorAllocation> _shaderResourceViews;
};

