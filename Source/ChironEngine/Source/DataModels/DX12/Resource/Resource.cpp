#include "Pch.h"
#include "Resource.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

#include "DataModels/DX12/ResourceStateTracker/ResourceStateTracker.h"

Resource::Resource() : _resource(nullptr), _name(L"")
{
}

Resource::Resource(const D3D12_RESOURCE_DESC& resourceDesc, const std::wstring& name, const D3D12_CLEAR_VALUE* clearValue)
{
	auto device = App->GetModule<ModuleID3D12>()->GetDevice();

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	Chiron::Utils::ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
		D3D12_RESOURCE_STATE_COMMON, clearValue, IID_PPV_ARGS(&_resource)));

	_resource->SetName(_name.c_str());

	ResourceStateTracker::AddGlobalResourceState(_resource.Get(), D3D12_RESOURCE_STATE_COMMON);

	CheckFeatureSupport();
}

Resource::Resource(ComPtr<ID3D12Resource> resource, const std::wstring& name) : _resource(resource), _name(name)
{
	ResourceStateTracker::AddGlobalResourceState(_resource.Get(), D3D12_RESOURCE_STATE_COMMON);
	
	CheckFeatureSupport();
}

Resource::Resource(const Resource& copy) : _resource(copy._resource), _name(copy._name), _featureSupport(copy._featureSupport)
{
}

Resource::~Resource()
{
	ResourceStateTracker::RemoveGlobalResourceState(_resource.Get());
}

Resource& Resource::operator=(const Resource& other)
{
	if (&other != this)
	{
		_resource = other._resource;
		_name = other._name;
		_featureSupport = other._featureSupport;
	}
	return *this;
}

Resource& Resource::operator=(Resource&& other) noexcept
{
	if (&other != this)
	{
		_resource = other._resource;
		_name = other._name;
		_featureSupport = other._featureSupport;

		other._resource.Reset();
		other._name.clear();
	}
	return *this;
}

void Resource::Reset()
{
	_resource.Reset();
	_name.clear();
	_featureSupport = { DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE };
}

bool Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const
{
	return (_featureSupport.Support1 & formatSupport) != 0;
}

bool Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const
{
	return (_featureSupport.Support2 & formatSupport) != 0;
}

void Resource::SetResource(ComPtr<ID3D12Resource> resource)
{
	_resource = resource;
	ResourceStateTracker::AddGlobalResourceState(_resource.Get(), D3D12_RESOURCE_STATE_COMMON);
	CheckFeatureSupport();
}

void Resource::CheckFeatureSupport()
{
	auto device = App->GetModule<ModuleID3D12>()->GetDevice();

	_featureSupport.Format = _resource->GetDesc().Format;
	Chiron::Utils::ThrowIfFailed(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &_featureSupport,
		sizeof(_featureSupport)));
}
