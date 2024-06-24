#pragma once

class Resource
{
public:
	Resource(const D3D12_RESOURCE_DESC& resourceDesc, const std::wstring& name = L"");
	Resource(ComPtr<ID3D12Resource> resource, const std::wstring& name);
	Resource(const Resource& copy);
	~Resource();

	inline bool IsValid();

	void Reset();

	// ------------- GETTERS ----------------------

	inline ID3D12Resource* GetResource() const;
	inline const std::wstring& GetName() const;

private:
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
