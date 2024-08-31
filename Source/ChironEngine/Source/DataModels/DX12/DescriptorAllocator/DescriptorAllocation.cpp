#include "Pch.h"
#include "DescriptorAllocation.h"

#include "Application.h"

#include "DescriptorAllocatorPage.h"

DescriptorAllocation::DescriptorAllocation() : _descriptorHandle{ 0 }, _numHandles(0), _descriptorSize(0), _page(nullptr)
{
}

DescriptorAllocation::DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t numHandles, 
	uint32_t descriptorSize, std::shared_ptr<DescriptorAllocatorPage> page) : _descriptorHandle(descriptor), 
	_numHandles(numHandles), _descriptorSize(descriptorSize), _page(page)
{
}

DescriptorAllocation::~DescriptorAllocation()
{
	Free();
}

DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& allocation) noexcept : 
	_descriptorHandle(allocation._descriptorHandle), _numHandles(allocation._numHandles), 
	_descriptorSize(allocation._descriptorSize), _page(std::move(allocation._page))
{
	allocation._descriptorHandle.ptr = 0;
	allocation._numHandles = 0;
	allocation._descriptorSize = 0;
}

DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other) noexcept
{
	Free();

	_descriptorHandle = other._descriptorHandle;
	_numHandles = other._numHandles;
	_descriptorSize = other._descriptorSize;
	_page = std::move(other._page);

	other._descriptorHandle.ptr = 0;
	other._numHandles = 0;
	other._descriptorSize = 0;

	return *this;
}

bool DescriptorAllocation::IsNull() const
{
	return _descriptorHandle.ptr == 0;
}

void DescriptorAllocation::Free()
{
	if (!IsNull() && _page)
	{
		_page->Free(std::move(*this), App->GetFrameCount());

		_descriptorHandle.ptr = 0;
		_numHandles = 0;
		_descriptorSize = 0;
		_page.reset();
	}
}