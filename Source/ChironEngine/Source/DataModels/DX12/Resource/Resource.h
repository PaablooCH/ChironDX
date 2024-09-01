#pragma once

class Resource
{
public:
	Resource& operator=(const Resource& other);
	Resource& operator=(Resource&& other) noexcept;

	inline bool IsValid();

	void Reset();

	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const;
	bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const;

	// ------------- GETTERS ----------------------

	inline ID3D12Resource* GetResource() const;
	inline const std::wstring& GetName() const;

	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const = 0;
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const = 0;
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const = 0;
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mips) const = 0;

	// ------------- SETTERS ----------------------

	void SetResource(ComPtr<ID3D12Resource> resource);
	inline void SetName(std::wstring name);

protected:
	Resource();
	Resource(const D3D12_RESOURCE_DESC& resourceDesc, const std::wstring& name = L"",
		const D3D12_CLEAR_VALUE* clearValue = nullptr);
	Resource(ComPtr<ID3D12Resource> resource);
	Resource(const Resource& copy);

	virtual ~Resource();

private:
	void CheckFeatureSupport();

protected:
	ComPtr<ID3D12Resource> _resource;
	std::wstring _name;
private:
	D3D12_FEATURE_DATA_FORMAT_SUPPORT _featureSupport;
};

inline bool Resource::IsValid()
{
	return _resource;
}

inline ID3D12Resource* Resource::GetResource() const
{
	return _resource.Get();
}

inline const std::wstring& Resource::GetName() const
{
	return _name;
}

inline void Resource::SetName(std::wstring name)
{
	_name = name;
	if (_resource)
	{
		_resource->SetName(_name.c_str());
	}
}
