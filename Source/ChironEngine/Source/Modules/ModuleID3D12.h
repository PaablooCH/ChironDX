#pragma once
#include "Module.h"

class CommandQueue;

class ModuleID3D12 : public Module
{
public:
    ModuleID3D12();
    ~ModuleID3D12() override;

    bool Init() override;
    UpdateStatus PreUpdate() override;
    UpdateStatus Update() override;
    UpdateStatus PostUpdate() override;
    bool CleanUp() override;

    void SwapCurrentBuffer();

    // ------------- WINDOW FUNC ----------------------

    void ToggleVSync();
    void ResizeBuffers(unsigned newWidth, unsigned newHeight);

    // ------------- CREATORS ----------------------

    ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ID3D12CommandAllocator* commandAllocator,
        D3D12_COMMAND_LIST_TYPE type, const LPCWSTR& name = NULL);
    void CreateTransitionBarrier(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12Resource> resource, 
        D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
    void CreateAliasingBarrier(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12Resource> resourceBefore, 
        ComPtr<ID3D12Resource> resourceAfter);
    void CreateUAVBarrier(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12Resource> resource);

    // ------------- SYNCHRONIZATION ----------------------

    void SaveCurrentBufferFenceValue(const uint64_t& fenceValue);
    // Waits until all the events are reached.
    void Flush();

    // ------------- GETTERS ----------------------

    inline ID3D12Device2* GetDevice() const;
    inline CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const;
    inline IDXGISwapChain4* GetSwapChain() const;
    inline UINT GetCurrentBuffer() const;
    inline ID3D12Resource* GetRenderBuffer() const;
    inline ID3D12DescriptorHeap* GetRenderTargetViewHeap() const;
    inline UINT GetRtvSize() const;
    inline ID3D12Resource* GetDepthStencilBuffer() const;
    inline ID3D12DescriptorHeap* GetDepthStencilViewHeap() const;

private:
    // ------------- CREATORS ----------------------

    bool CreateFactory();
    bool CreateAdapter();
    bool CreateDevice();
    bool CreateCommandQueue();
    bool CreateSwapChain();
    void CreateDepthStencil(unsigned width, unsigned height);

    // ------------- UPDATES ----------------------

    void UpdateRenderTargetViews();
    
    // ------------- INITS ---------------------------

    void InitFrameBuffer();
    
    // ------------- SYNCHRONIZATION ----------------------

    uint64_t Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
        uint64_t& fenceValue);
    void WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
        std::chrono::milliseconds duration = std::chrono::milliseconds::max());
private:
    static const UINT backBufferCount = 2;

    // Is the entry point to the DirectX 12 API.
    ComPtr<IDXGIFactory4> _factory;
#ifdef DEBUG
    // Debug Purposes
    ComPtr<ID3D12Debug1> _debugController;
#endif // DEBUG

    // Provides information on the physical properties of a given DirectX device. 
    // Can query the current GPU's name, manufacturer, how much memory it has, and much more.
    ComPtr<IDXGIAdapter1> _adapter;

    // Primary entry point to the DirectX 12 API, gives access to the inner parts of the API.
    ComPtr<ID3D12Device2> _device;
#ifdef DEBUG
    // Debug Purposes
    ComPtr<ID3D12DebugDevice> _debugDevice;
#endif // DEBUG

    bool _vSync;
    bool _tearingSupported;
    UINT _currentBuffer;

    // Allows submit groups of draw calls, together to execute in order.

    std::unique_ptr<CommandQueue> _commandQueueDirect;  // command buffer that the GPU can execute
    std::unique_ptr<CommandQueue> _commandQueueCompute; // command buffer for computing
    std::unique_ptr<CommandQueue> _commandQueueCopy;    // command buffer for copying

    // Lets the program know when certain tasks have been executed by the GPU, 
    // when it uploads to GPU exclusive memory, or when it've finished presenting to the screen.
    UINT64 _bufferFenceValues[backBufferCount];

    // Handle swapping and allocating back and front buffers to display what it is rendering (back) and what is showed (front).
    ComPtr<IDXGISwapChain4> _swapChain;

    // The texture result of drawing in the swapChain.
    ComPtr<ID3D12Resource> _renderBuffers[backBufferCount];
    // The heap where the rtv is located
    ComPtr<ID3D12DescriptorHeap> _renderTargetViewHeap;
    // The descriptor size. Depends on device.
    UINT _renderTargetViewDesciptorSize;

    // Depth Stencil buffer.
    ComPtr<ID3D12Resource> _depthStencilBuffer;
    // Descriptor heap for depth buffer.
    ComPtr<ID3D12DescriptorHeap> _dsvHeap;
};

inline ID3D12Device2* ModuleID3D12::GetDevice() const
{
    return _device.Get();
}

inline CommandQueue* ModuleID3D12::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
{
    switch (type)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        return _commandQueueDirect.get();
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        return _commandQueueCompute.get();
    case D3D12_COMMAND_LIST_TYPE_COPY:
        return _commandQueueCopy.get();
    default:
        assert(false && "Incorrect queue type.");
        break;
    }
    return nullptr;
}

inline IDXGISwapChain4* ModuleID3D12::GetSwapChain() const
{
    return _swapChain.Get();
}

inline UINT ModuleID3D12::GetCurrentBuffer() const
{
    return _currentBuffer;
}

inline ID3D12Resource* ModuleID3D12::GetRenderBuffer() const
{
    return _renderBuffers[_currentBuffer].Get();
}

inline ID3D12DescriptorHeap* ModuleID3D12::GetRenderTargetViewHeap() const
{
    return _renderTargetViewHeap.Get();
}

inline UINT ModuleID3D12::GetRtvSize() const
{
    return _renderTargetViewDesciptorSize;
}

inline ID3D12Resource* ModuleID3D12::GetDepthStencilBuffer() const
{
    return _depthStencilBuffer.Get();
}

inline ID3D12DescriptorHeap* ModuleID3D12::GetDepthStencilViewHeap() const
{
    return _dsvHeap.Get();
}

{
}
