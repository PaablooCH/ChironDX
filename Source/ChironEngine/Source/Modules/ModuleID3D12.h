#pragma once
#include "Module.h"

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
    void ResizeSwapChain(unsigned newWidth, unsigned newHeight);

    // ------------- CREATORS ----------------------

    ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ID3D12CommandAllocator* commandAllocator,
        D3D12_COMMAND_LIST_TYPE type, const LPCWSTR& name = NULL);
    bool CreateBarrier(ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_RESOURCE_BARRIER_TYPE type);

    // ------------- SYNCHRONIZATION ----------------------

    HANDLE CreateEventHandle();
    // Waits until all the events are reached.
    void Flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
        uint64_t& fenceValue, HANDLE fenceEvent);

    // ------------- GETTERS ----------------------

    inline IDXGIFactory4* GetFactory() const;
    inline IDXGIAdapter1* GetAdapter() const;
    inline ID3D12Device* GetDevice() const;
    inline ID3D12CommandQueue* GetCommandQueue() const;
    inline ID3D12CommandAllocator* GetCommandAllocator() const;
    inline ID3D12Fence* GetFence() const;
    inline HANDLE GetFenceEvent() const;
    inline UINT64 GetCurrentFenceValue() const;
    inline IDXGISwapChain4* GetSwapChain() const;
    inline UINT GetCurrentBuffer() const;
    inline ID3D12Resource* GetRenderBuffer() const;
    inline ID3D12DescriptorHeap* GetRenderTargetViewHeap() const;
    inline UINT GetRtvSize() const;
    inline ID3D12RootSignature* GetRootSignature() const;

private:
    // ------------- CREATORS ----------------------

    bool CreateFactory();
    bool CreateAdapter();
    bool CreateDevice();
    bool CreateCommandAllocator();
    bool CreateCommandQueue();
    bool CreateFence();
    bool CreateSwapChain();

    // ------------- UPDATES ----------------------

    void UpdateRenderTargetViews();
    
    // ------------- INITS ---------------------------

    void InitFrameBuffer();
    bool InitResources();
    
    // ------------- SYNCHRONIZATION ----------------------

    uint64_t Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
        uint64_t& fenceValue);
    void WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
        std::chrono::milliseconds duration = std::chrono::milliseconds::max());
private:
    static const UINT backbufferCount = 2;

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
    ComPtr<ID3D12Device> _device;
#ifdef DEBUG
    // Debug Purposes
    ComPtr<ID3D12DebugDevice> _debugDevice;
#endif // DEBUG

    bool _vSync;
    bool _tearingSupported;
    UINT _currentBuffer;

    // Allows submit groups of draw calls, together to execute in order.
    ComPtr<ID3D12CommandQueue> _commandQueue;

    // Create command lists where define the functions that the GPU execute. Can not be used until all the commands are 
    // executed.
    ComPtr<ID3D12CommandAllocator> _commandAllocator[backbufferCount];

    // Lets the program know when certain tasks have been executed by the GPU, 
    // when it uploads to GPU exclusive memory, or when it've finished presenting to the screen.
    ComPtr<ID3D12Fence> _fence;
    HANDLE _fenceEvent;
    UINT64 _frameValues[backbufferCount];
    UINT64 _fenceValue;

    // Handle swapping and allocating back and front buffers to display what it is rendering (back) and what is showed (front).
    ComPtr<IDXGISwapChain4> _swapChain;

    // The texture result of drawing in the swapChain.
    ComPtr<ID3D12Resource> _renderBuffers[backbufferCount];
    // The heap where the rtv is located
    ComPtr<ID3D12DescriptorHeap> _renderTargetViewHeap;
    // The descriptor size. Depends on device.
    UINT _renderTargetViewDesciptorSize;

    // ------------- RESOURCES ----------------------

    // Objects that define what type of resources are accessible to the shaders, 
    // be it constant buffers, structured buffers, samplers, textures, structured buffers, etc.
    ComPtr<ID3D12RootSignature> _rootSignature;

    ComPtr<ID3D12Resource> _uploadBuffer;
};

inline IDXGIFactory4* ModuleID3D12::GetFactory() const
{
    return _factory.Get();
}

inline IDXGIAdapter1* ModuleID3D12::GetAdapter() const
{
    return _adapter.Get();
}

inline ID3D12Device* ModuleID3D12::GetDevice() const
{
    return _device.Get();
}

inline ID3D12CommandQueue* ModuleID3D12::GetCommandQueue() const
{
    return _commandQueue.Get();
}

inline ID3D12CommandAllocator* ModuleID3D12::GetCommandAllocator() const
{
    return _commandAllocator[_currentBuffer].Get();
}

inline ID3D12Fence* ModuleID3D12::GetFence() const
{
    return _fence.Get();
}

inline HANDLE ModuleID3D12::GetFenceEvent() const
{
    return _fenceEvent;
}

inline UINT64 ModuleID3D12::GetCurrentFenceValue() const
{
    return _frameValues[_currentBuffer];
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

inline ID3D12RootSignature* ModuleID3D12::GetRootSignature() const
{
    return _rootSignature.Get();
}
