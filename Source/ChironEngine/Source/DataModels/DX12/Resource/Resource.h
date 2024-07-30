#pragma once

class Resource
{
public:
	Resource(const D3D12_RESOURCE_DESC& resourceDesc, const std::wstring& name = L"");
	Resource(ComPtr<ID3D12Resource> resource, const std::wstring& name);
	Resource(const Resource& copy);
	
	virtual ~Resource();

	Resource& operator=(const Resource& other);
	Resource& operator=(Resource&& other);

	inline bool IsValid();

	void Reset();

	// ------------- GETTERS ----------------------

	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const = 0;
	inline ID3D12Resource* GetResource() const;
	inline const std::wstring& GetName() const;

	void SetResource(ComPtr<ID3D12Resource> resource);
	inline void SetName(std::wstring name);
protected:
	ComPtr<ID3D12Resource> _resource;
	std::wstring _name;
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
