#include "Pch.h"
#include "DescriptorAllocation.h"

#include "Application.h"

#include "DescriptorAllocatorPage.h"

DescriptorAllocation::DescriptorAllocation() : _cpuDescriptorHandle{ 0 }, _gpuDescriptorHandle{ 0 }, _numHandles(0), 
_descriptorSize(0), _page(nullptr)
{
}

DescriptorAllocation::DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor, 
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptor, uint32_t numHandles, uint32_t descriptorSize, 
	std::shared_ptr<DescriptorAllocatorPage> page) : _cpuDescriptorHandle(cpuDescriptor), _gpuDescriptorHandle(gpuDescriptor),
	_numHandles(numHandles), _descriptorSize(descriptorSize), _page(page)
{
}

DescriptorAllocation::~DescriptorAllocation()
{
	Free();
}

DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& allocation) noexcept : 
	_cpuDescriptorHandle(allocation._cpuDescriptorHandle), _gpuDescriptorHandle(allocation._gpuDescriptorHandle),
	_numHandles(allocation._numHandles), _descriptorSize(allocation._descriptorSize), _page(std::move(allocation._page))
{
	allocation._cpuDescriptorHandle.ptr = 0;
	allocation._gpuDescriptorHandle.ptr = 0;
	allocation._numHandles = 0;
	allocation._descriptorSize = 0;
}

DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other) noexcept
{
	Free();

	_cpuDescriptorHandle = other._cpuDescriptorHandle;
	_gpuDescriptorHandle = other._gpuDescriptorHandle;
	_numHandles = other._numHandles;
	_descriptorSize = other._descriptorSize;
	_page = std::move(other._page);

	other._cpuDescriptorHandle.ptr = 0;
	other._gpuDescriptorHandle.ptr = 0;
	other._numHandles = 0;
	other._descriptorSize = 0;

	return *this;
}

bool DescriptorAllocation::IsNull() const
{
	return _cpuDescriptorHandle.ptr == 0;
}

void DescriptorAllocation::Free()
{
	if (!IsNull() && _page)
	{
		_page->Free(std::move(*this), App->GetFrameCount());

		_cpuDescriptorHandle.ptr = 0;
		_gpuDescriptorHandle.ptr = 0;
		_numHandles = 0;
		_descriptorSize = 0;
		_page.reset();
	}
}