#include "Pch.h"
#include "Texture.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"
#include "DataModels/DX12/ResourceStateTracker/ResourceStateTracker.h"

#include "StdSpecialization/HashSpecialization.h"

namespace 
{
	std::mutex mutex;
}

Texture::Texture(const D3D12_RESOURCE_DESC& resourceDesc, const std::wstring& name) : Resource(resourceDesc, name)
{
}

Texture::Texture(ComPtr<ID3D12Resource> resource, const std::wstring& name) : Resource(resource, name)
{
}

Texture::Texture(const Texture& copy) : Resource(copy)
{
}

Texture::~Texture()
{
}

Texture& Texture::operator=(const Texture& other)
{
	Resource::operator=(other);

	return *this;
}

Texture& Texture::operator=(Texture&& other)
{
	Resource::operator=(other);

	return *this;
}

void Texture::Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize)
{
	if (_resource)
	{
		ResourceStateTracker::RemoveGlobalResourceState(_resource.Get());

		CD3DX12_RESOURCE_DESC textureDesc(_resource->GetDesc());

		textureDesc.Width = width > 1U ? width : 1U;
		textureDesc.Height = height > 1U ? height : 1U;
		textureDesc.DepthOrArraySize = depthOrArraySize > 1U ? depthOrArraySize : 1U;

		auto device = App->GetModule<ModuleID3D12>()->GetDevice();

		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		Chiron::Utils::ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &textureDesc,
			D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&_resource)));

		_resource->SetName(_name.c_str());

		ResourceStateTracker::AddGlobalResourceState(_resource.Get(), D3D12_RESOURCE_STATE_COMMON);
	}
}
void Texture::SetTexture(ComPtr<ID3D12Resource> texture)
{
	SetResource(texture);
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const
{
	size_t hash = 0;

	if (srvDesc)
	{
		hash = std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*srvDesc);
	}

	std::lock_guard<std::mutex> lock(mutex);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHndl{};
	auto it = _shaderResourceViews.find(hash);
	if (it == _shaderResourceViews.end())
	{
		auto allocator = CreateShaderResourceView(srvDesc);
		cpuDescHndl = _shaderResourceViews.insert({ hash, std::move(allocator) }).first->second.GetDescriptorHandle();
	}
	else 
	{
		cpuDescHndl = it->second.GetDescriptorHandle();
	}

	return cpuDescHndl;
}

DescriptorAllocation Texture::CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const
{
	auto id3d12 = App->GetModule<ModuleID3D12>();
	auto device = id3d12->GetDevice();
	auto descriptorAllocator = id3d12->GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto allocator = descriptorAllocator->Allocate();

	device->CreateShaderResourceView(_resource.Get(), srvDesc, allocator.GetDescriptorHandle());

	return allocator;
}
