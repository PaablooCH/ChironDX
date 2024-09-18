#pragma once

#define _KB(x) (x * 1024)
#define _MB(x) (x * 1024 * 1024)

#define _64KB _KB(64)
#define _1MB _MB(1)
#define _2MB _MB(2)
#define _4MB _MB(4)
#define _8MB _MB(8)
#define _16MB _MB(16)
#define _32MB _MB(32)
#define _64MB _MB(64)
#define _128MB _MB(128)
#define _256MB _MB(256)

class UploadBuffer
{
public:
    struct Allocation
    {
        void* CPU;
        D3D12_GPU_VIRTUAL_ADDRESS GPU;
    };

	explicit UploadBuffer(size_t pageSize = _2MB);

    Allocation Allocate(size_t sizeInBytes, size_t alignment);

    void Reset();

    inline size_t GetPageSize() const;
private:
    struct Page
    {
        Page(size_t pageSize);
        ~Page();

        Allocation Allocate(size_t sizeInBytes, size_t alignment);
        bool HasSpace(size_t sizeInBytes, size_t alignment) const;

        void Reset();
    private:
        ComPtr<ID3D12Resource> _gpuMemory;

        void* _cpuPtr;
        D3D12_GPU_VIRTUAL_ADDRESS _gpuPtr;
        size_t _pageSize;
        size_t _offset;
    };

private:
    std::shared_ptr<Page> RequestPage();

private:

    size_t _pageSize;

    // This stores all the pages created
    std::deque<std::shared_ptr<Page>> _pages;
    // This stores all the pages available for use
    std::deque<std::shared_ptr<Page>> _pagesAvailable;

    std::shared_ptr<Page> _currentPage;
};

inline size_t UploadBuffer::GetPageSize() const
{
    return _pageSize;
}

