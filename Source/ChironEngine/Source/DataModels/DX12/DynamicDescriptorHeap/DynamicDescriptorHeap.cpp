#include "Pch.h"
#include "DynamicDescriptorHeap.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/RootSignature/RootSignature.h"

DynamicDescriptorHeap::DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptorsPerHeap) :
    _descriptorHeapType(heapType), _numDescriptorsPerHeap(numDescriptorsPerHeap),
    _descriptorHandleCache(std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(numDescriptorsPerHeap)),
    _descriptorTableBitMask(0), _staleDescriptorTableBitMask(0), _currentCPUDescriptorHandle(D3D12_DEFAULT),
    _currentGPUDescriptorHandle(D3D12_DEFAULT), _numFreeHandles(0)
{
    _device = App->GetModule<ModuleID3D12>()->GetDevice();
    _descriptorHandleIncrementSize = _device->GetDescriptorHandleIncrementSize(_descriptorHeapType);
}

DynamicDescriptorHeap::~DynamicDescriptorHeap()
{
}

void DynamicDescriptorHeap::StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors,
    const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors)
{
    if (numDescriptors > _numDescriptorsPerHeap || rootParameterIndex >= MaxDescriptorTables)
    {
        throw std::bad_alloc();
    }

    DescriptorTableCache& descriptorTableCache = _descriptorTableCache[rootParameterIndex];

    // Check that the number of descriptors to copy does not exceed the number of descriptors expected in the
    // descriptor table.
    if ((offset + numDescriptors) > descriptorTableCache.NumDescriptors)
    {
        throw std::length_error("Number of descriptors exceeds the number of descriptors in the descriptor table.");
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* dstDescriptor = (descriptorTableCache.BaseDescriptor + offset);
    for (uint32_t i = 0; i < numDescriptors; ++i)
    {
        dstDescriptor[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srcDescriptors, i, _descriptorHandleIncrementSize);
    }

    // Set the root parameter index bit to make sure the descriptor table at that index is bound to the command list.
    _staleDescriptorTableBitMask |= (1 << rootParameterIndex);
}

void DynamicDescriptorHeap::CommitStagedDescriptorsForDraw(CommandList& commandList)
{
    CommitStagedDescriptors(commandList, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
}

void DynamicDescriptorHeap::CommitStagedDescriptorsForDispatch(CommandList& commandList)
{
    CommitStagedDescriptors(commandList, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
}

void DynamicDescriptorHeap::CommitStagedDescriptors(CommandList& commandList,
    std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc)
{
    // Compute the number of descriptors that need to be copied
    uint32_t numDescriptorsToCommit = ComputeStaleDescriptorCount();

    if (numDescriptorsToCommit > 0)
    {
        auto d3d12GraphicsCommandList = commandList.GetGraphicsCommandList().Get();
        assert(d3d12GraphicsCommandList != nullptr);

        if (!_currentDescriptorHeap || _numFreeHandles < numDescriptorsToCommit)
        {
            _currentDescriptorHeap = RequestDescriptorHeap();
            _currentCPUDescriptorHandle = _currentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            _currentGPUDescriptorHandle = _currentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
            _numFreeHandles = _numDescriptorsPerHeap;

            commandList.SetDescriptorHeap(_descriptorHeapType, _currentDescriptorHeap.Get());

            // When updating the descriptor heap on the command list, all descriptor tables must be (re)recopied to the
            // new descriptor heap (not just the stale descriptor tables).
            _staleDescriptorTableBitMask = _descriptorTableBitMask;
        }

        DWORD rootIndex;
        // Scan from LSB to MSB for a bit set in staleDescriptorsBitMask
        while (_BitScanForward(&rootIndex, _staleDescriptorTableBitMask))
        {
            UINT numSrcDescriptors = _descriptorTableCache[rootIndex].NumDescriptors;
            D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorHandles = _descriptorTableCache[rootIndex].BaseDescriptor;

            D3D12_CPU_DESCRIPTOR_HANDLE pDestDescriptorRangeStarts[] =
            {
                _currentCPUDescriptorHandle
            };
            UINT pDestDescriptorRangeSizes[] =
            {
                numSrcDescriptors
            };

            // Copy the staged CPU visible descriptors to the GPU visible descriptor heap.
            _device->CopyDescriptors(1, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes,
                numSrcDescriptors, pSrcDescriptorHandles, nullptr, _descriptorHeapType);

            // Set the descriptors on the command list using the passed-in setter function.
            setFunc(d3d12GraphicsCommandList, rootIndex, _currentGPUDescriptorHandle);

            // Offset current CPU and GPU descriptor handles.
            _currentCPUDescriptorHandle.Offset(numSrcDescriptors, _descriptorHandleIncrementSize);
            _currentGPUDescriptorHandle.Offset(numSrcDescriptors, _descriptorHandleIncrementSize);
            _numFreeHandles -= numSrcDescriptors;

            // Flip the stale bit so the descriptor table is not recopied again unless it is updated with a new descriptor.
            _staleDescriptorTableBitMask ^= (1 << rootIndex);
        }
    }
}

D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorHeap::CopyDescriptor(CommandList& comandList,
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor)
{
    if (!_currentDescriptorHeap || _numFreeHandles < 1)
    {
        _currentDescriptorHeap = RequestDescriptorHeap();
        _currentCPUDescriptorHandle = _currentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        _currentGPUDescriptorHandle = _currentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        _numFreeHandles = _numDescriptorsPerHeap;

        comandList.SetDescriptorHeap(_descriptorHeapType, _currentDescriptorHeap.Get());

        // When updating the descriptor heap on the command list, all descriptor tables must be (re)recopied
        // to the new descriptor heap (not just the stale descriptor tables).
        _staleDescriptorTableBitMask = _descriptorTableBitMask;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE hGPU = _currentGPUDescriptorHandle;
    _device->CopyDescriptorsSimple(1, _currentCPUDescriptorHandle, cpuDescriptor, _descriptorHeapType);

    _currentCPUDescriptorHandle.Offset(1, _descriptorHandleIncrementSize);
    _currentGPUDescriptorHandle.Offset(1, _descriptorHandleIncrementSize);
    _numFreeHandles -= 1;

    return hGPU;
}

void DynamicDescriptorHeap::ParseRootSignature(const RootSignature* rootSignature)
{
    _staleDescriptorTableBitMask = 0;

    const auto& rootDesc = rootSignature->GetRootSignatureDesc();

    _descriptorTableBitMask = rootSignature->GetDescriptorTableBitMask(_descriptorHeapType);
    uint32_t descriptorTableBitMask = _descriptorTableBitMask;

    uint32_t currentOffset = 0;
    DWORD rootIndex;
    while (_BitScanForward(&rootIndex, descriptorTableBitMask) && rootIndex < rootDesc.NumParameters)
    {
        uint32_t numDescriptors = rootSignature->GetNumDescriptors(rootIndex);

        DescriptorTableCache& descriptorTableCache = _descriptorTableCache[rootIndex];
        descriptorTableCache.NumDescriptors = numDescriptors;
        descriptorTableCache.BaseDescriptor = _descriptorHandleCache.get() + currentOffset;

        currentOffset += numDescriptors;

        // Flip the descriptor table bit so it's not scanned again for the current index.
        descriptorTableBitMask ^= (1 << rootIndex);
    }
    // Make sure the maximum number of descriptors per descriptor heap has not been exceeded.
    assert(currentOffset <= _numDescriptorsPerHeap &&
        "The root signature requires more than the maximum number of descriptors per descriptor heap.");
}

void DynamicDescriptorHeap::Reset()
{
    _availableDescriptorHeaps = _descriptorHeapPool;

    _currentDescriptorHeap = nullptr;
    _currentCPUDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE();
    _currentGPUDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE();
    _numFreeHandles = 0;

    _descriptorTableBitMask = 0;
    _staleDescriptorTableBitMask = 0;

    // Reset the table cache
    for (int i = 0; i < MaxDescriptorTables; ++i)
    {
        _descriptorTableCache[i].Reset();
    }
}

ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::RequestDescriptorHeap()
{
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    if (!_availableDescriptorHeaps.empty())
    {
        descriptorHeap = _availableDescriptorHeaps.front();
        _availableDescriptorHeaps.pop();
    }
    else
    {
        descriptorHeap = CreateDescriptorHeap();
        _descriptorHeapPool.push(descriptorHeap);
    }

    return descriptorHeap;
}

ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::CreateDescriptorHeap() const
{
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
    descriptorHeapDesc.Type = _descriptorHeapType;
    descriptorHeapDesc.NumDescriptors = _numDescriptorsPerHeap;
    descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    Chiron::Utils::ThrowIfFailed(_device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap)));

    return descriptorHeap;
}

uint32_t DynamicDescriptorHeap::ComputeStaleDescriptorCount() const
{
    uint32_t numStaleDescriptors = 0;
    DWORD i;
    DWORD staleDescriptorsBitMask = _staleDescriptorTableBitMask;

    while (_BitScanForward(&i, staleDescriptorsBitMask))
    {
        numStaleDescriptors += _descriptorTableCache[i].NumDescriptors;
        staleDescriptorsBitMask ^= (1 << i);
    }

    return numStaleDescriptors;
}