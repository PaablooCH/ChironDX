#include "Pch.h"
#include "UploadBuffer.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

//namespace 
//{
//	std::mutex mutex;
//}

UploadBuffer::UploadBuffer(size_t pageSize) : _pageSize(pageSize)
{
}

UploadBuffer::Allocation UploadBuffer::Allocate(size_t sizeInBytes, size_t alignment)
{
	if (sizeInBytes > _pageSize)
	{
		throw std::bad_alloc();
	}

	if (!_currentPage || !_currentPage->HasSpace(sizeInBytes, alignment))
	{
		_currentPage = RequestPage();
	}
	return _currentPage->Allocate(sizeInBytes, alignment);
}

void UploadBuffer::Reset()
{
	_currentPage = nullptr;

	_pagesAvailable = _pages;

	for (std::shared_ptr<Page> page : _pagesAvailable)
	{
		page->Reset();
	}
}

std::shared_ptr<UploadBuffer::Page> UploadBuffer::RequestPage()
{
	std::shared_ptr<Page> page;

	if (!_pagesAvailable.empty())
	{
		page = _pagesAvailable.front();
		_pagesAvailable.pop_front();
	}
	else
	{
		page = std::make_shared<Page>(_pageSize);
		_pages.push_back(page);
	}

	return page;
}

UploadBuffer::Page::Page(size_t pageSize) : _pageSize(pageSize), _offset(0), _cpuPtr(nullptr), 
_gpuPtr(D3D12_GPU_VIRTUAL_ADDRESS(0))
{
	auto device = App->GetModule<ModuleID3D12>()->GetDevice();
	CD3DX12_HEAP_PROPERTIES heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(_pageSize);
	Chiron::Utils::ThrowIfFailed(device->CreateCommittedResource(&heap,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_gpuMemory)));

	_gpuPtr = _gpuMemory->GetGPUVirtualAddress();
	_gpuMemory->Map(0, nullptr, &_cpuPtr);
}

UploadBuffer::Page::~Page()
{
	_gpuMemory->Unmap(0, nullptr);
	_cpuPtr = nullptr;
	_gpuPtr = D3D12_GPU_VIRTUAL_ADDRESS(0);
}

UploadBuffer::Allocation UploadBuffer::Page::Allocate(size_t sizeInBytes, size_t alignment)
{
	// Check this if thread safe is required
	// std::lock_guard<std::mutex> guard(mutex);
	size_t alignedSize = Chiron::Utils::AlignUp(sizeInBytes, alignment);
	_offset = Chiron::Utils::AlignUp(_offset, alignment);

	Allocation allocation{};
	allocation.CPU = static_cast<uint8_t*>(_cpuPtr) + _offset;
	allocation.GPU = _gpuPtr + _offset;

	_offset += alignedSize;
	return allocation;
}

bool UploadBuffer::Page::HasSpace(size_t sizeInBytes, size_t alignment) const
{
	size_t alignedSize = Chiron::Utils::AlignUp(sizeInBytes, alignment);
	size_t alignedOffset = Chiron::Utils::AlignUp(_offset, alignment);

	return alignedOffset + alignedSize <= _pageSize;
}

void UploadBuffer::Page::Reset()
{
	_offset = 0;
}
