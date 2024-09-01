#pragma once
#include "Resource.h"

#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"

CHIRON_TODO("temporary");
enum class TextureType 
{
	ALBEDO,
	DIFFUSE = ALBEDO,
	SPECULAR,
	METALLIC = SPECULAR,
	NORMAL_MAP,
	DEPTH,
	RENDER_TARGET
};

class Texture : public Resource
{
public:
	Texture();
	Texture(const D3D12_RESOURCE_DESC& resourceDesc, const std::wstring& name = L"", 
		const D3D12_CLEAR_VALUE* clearValue = nullptr);
	Texture(ComPtr<ID3D12Resource> resource);
	Texture(const Texture& copy);

	~Texture() override;

	Texture& operator=(const Texture& other);
	Texture& operator=(Texture&& other) noexcept;

	void Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize = 1);

	void CreateViews();

	inline bool CompatibleWithRTV() const;
	inline bool CompatibleWithDSV() const;
	inline bool CompatibleWithUAV() const;
	inline bool CompatibleWithSRV() const;

	static DXGI_FORMAT GetUAVCompatibleFormat(DXGI_FORMAT format);
	inline static DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT format);
	inline static bool IsSRGBFormat(DXGI_FORMAT format);

	// ------------- GETTERS ----------------------

	inline D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const override;
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const override;
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const override;
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mips) const override;
	inline TextureType GetTextureType() const;

	// ------------- SETTERS ----------------------

	void SetTexture(ComPtr<ID3D12Resource> texture);

private:
	D3D12_RENDER_TARGET_VIEW_DESC CreateRTVDesc(const D3D12_RESOURCE_DESC& resourceDesc);
	D3D12_DEPTH_STENCIL_VIEW_DESC CreateDSVDesc(const D3D12_RESOURCE_DESC& resourceDesc);
	D3D12_SHADER_RESOURCE_VIEW_DESC CreateSRVDesc(const D3D12_RESOURCE_DESC& resourceDesc);
	D3D12_UNORDERED_ACCESS_VIEW_DESC CreateUAVDesc(const D3D12_RESOURCE_DESC& resourceDesc, UINT mipSlice, 
		UINT arraySlice = 0, UINT planeSlice = 0);

private:
	mutable DescriptorAllocation _renderTargetView;
	mutable DescriptorAllocation _depthStencilView;
	mutable DescriptorAllocation _shaderResourceView;
	mutable DescriptorAllocation _unorderedAccessView;

	TextureType _type;
};

inline bool Texture::CompatibleWithRTV() const
{
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
}

inline bool Texture::CompatibleWithDSV() const
{
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
}

inline bool Texture::CompatibleWithUAV() const
{
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
		CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
		CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
}

inline bool Texture::CompatibleWithSRV() const
{
	return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
}

inline DXGI_FORMAT Texture::GetSRGBFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	case DXGI_FORMAT_BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM_SRGB;
	}

	return format;
}

inline bool Texture::IsSRGBFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return true;
	}
	return false;
}

inline D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRenderTargetView() const
{
	return _renderTargetView.GetDescriptorHandle();
}

inline D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDepthStencilView() const
{
	return _depthStencilView.GetDescriptorHandle();
}

inline D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetShaderResourceView() const
{
	return _shaderResourceView.GetDescriptorHandle();
}

inline D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUnorderedAccessView(uint32_t mips) const
{
	return _unorderedAccessView.GetDescriptorHandle(mips);
}

inline TextureType Texture::GetTextureType() const
{
	return TextureType::ALBEDO;
}