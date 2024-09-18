#include "Pch.h"
#include "ConstantBuffer.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

ConstantBuffer::ConstantBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t sizeInBytes, const std::wstring& name) :
	Resource(resourceDesc, name), _sizeInBytes(sizeInBytes)
{
    CreateView();
}

ConstantBuffer::~ConstantBuffer()
{
}

void ConstantBuffer::CreateView()
{
	auto id3d12 = App->GetModule<ModuleID3D12>();
	auto device = id3d12->GetDevice();

	auto descriptorAllocator = id3d12->GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_constantBufferView = descriptorAllocator->Allocate();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _resource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = Chiron::Utils::AlignUp(_sizeInBytes, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	// Crear la CBV
	device->CreateConstantBufferView(&cbvDesc, _constantBufferView.GetCPUDescriptorHandle());
}
