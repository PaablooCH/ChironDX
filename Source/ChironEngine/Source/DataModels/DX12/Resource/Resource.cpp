#include "Pch.h"
#include "Resource.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

#include "DataModels/DX12/ResourceStateTracker/ResourceStateTracker.h"

Resource::Resource(const D3D12_RESOURCE_DESC& resourceDesc, const std::wstring& name) : _name(name)
{
	auto device = App->GetModule<ModuleID3D12>()->GetDevice();

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	Chiron::Utils::ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
		D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&_resource)));

	_resource.Get()->SetName(_name.c_str());

	ResourceStateTracker::AddGlobalResourceState(_resource.Get(), D3D12_RESOURCE_STATE_COMMON);
}

Resource::Resource(ComPtr<ID3D12Resource> resource, const std::wstring& name) : _resource(resource), _name(name)
{
}

Resource::Resource(const Resource& copy) : _resource(copy._resource), _name(copy._name)
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
	}
	return *this;
}

Resource& Resource::operator=(Resource&& other)
{
	if (&other != this)
	{
		_resource = other._resource;
		_name = other._name;

		other._resource.Reset();
		other._name.clear();
	}
	return *this;
}

void Resource::Reset()
{
	_resource.Reset();
	_name.clear();
}
void Resource::SetResource(ComPtr<ID3D12Resource> resource)
{
	_resource = resource;
	ResourceStateTracker::AddGlobalResourceState(_resource.Get(), D3D12_RESOURCE_STATE_COMMON);
}
