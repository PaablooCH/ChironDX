#pragma once
#include "Module.h"

class CommandList;
class CommandQueue;
class DescriptorAllocator;
class Texture;

class ModuleID3D12 : public Module
{
public:
    ModuleID3D12();
    ~ModuleID3D12() override;

    bool Init() override;
    bool CleanUp() override;

    UpdateStatus PostUpdate() override;

    // The caller will lose ownership of the commandList shared_ptr after calling this function.
    uint64_t ExecuteCommandList(std::shared_ptr<CommandList>& commandList);

    // ------------- WINDOW FUNC ----------------------

    void ToggleVSync();
    void ResizeBuffers(unsigned newWidth, unsigned newHeight);
    void PresentAndSwapBuffer();

    // ------------- SYNCHRONIZATION ----------------------

    void SaveCurrentBufferFenceValue(const uint64_t& fenceValue);
    // Waits until all the events are reached.
    void Flush();
    void WaitForFenceValue(D3D12_COMMAND_LIST_TYPE type, uint64_t fenceValue);

    // ------------- GETTERS ----------------------

    inline ID3D12Device5* GetDevice() const;
    inline CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const;
    ID3D12CommandQueue* GetID3D12CommandQueue(D3D12_COMMAND_LIST_TYPE type) const;
    std::shared_ptr<CommandList> GetCommandList(D3D12_COMMAND_LIST_TYPE type) const;
    inline IDXGISwapChain4* GetSwapChain() const;
    inline UINT GetCurrentBuffer() const;
    inline Texture* GetRenderBuffer() const;
    inline Texture* GetDepthStencilBuffer() const;
    inline DescriptorAllocator* GetDescriptorAllocator(const D3D12_DESCRIPTOR_HEAP_TYPE& type) const;

private:
    // ------------- CREATORS ----------------------

    bool CreateFactory();
    bool CreateDevice();
    bool CreateCommandQueues();
    bool CreateSwapChain();
    void CreateDepthStencil(unsigned width, unsigned height);

    // ------------- UPDATES ----------------------

    void UpdateRenderTargetViews();

    // ------------- INITS ---------------------------

    void InitFrameBuffer();
    void InitDescriptorAllocator();

    void PrintMessages();

private:
    // Is the entry point to the DirectX 12 API.
    ComPtr<IDXGIFactory5> _factory;

    // Provides information on the physical properties of a given DirectX device.
    // Can query the current GPU's name, manufacturer, how much memory it has, and much more.
    ComPtr<IDXGIAdapter4> _adapter;

    // Primary entry point to the DirectX 12 API, gives access to the inner parts of the API.
    ComPtr<ID3D12Device5> _device;
#ifdef DEBUG
    // Debug Purposes
    ComPtr<ID3D12DebugDevice> _debugDevice;
    ComPtr<ID3D12InfoQueue1> _infoQueue;
#endif // DEBUG

    bool _vSync;
    bool _tearingSupported;
    bool _supportsRT;
    UINT _currentBuffer;

    // Allows submit groups of draw calls, together to execute in order.

    std::unique_ptr<CommandQueue> _commandQueueDirect;  // command buffer that the GPU can execute
    std::unique_ptr<CommandQueue> _commandQueueCompute; // command buffer for computing
    std::unique_ptr<CommandQueue> _commandQueueCopy;    // command buffer for copying

    // Lets the program know when certain tasks have been executed by the GPU,
    // when it uploads to GPU exclusive memory, or when it've finished presenting to the screen.
    UINT64 _bufferFenceValues[NUM_FRAMES_IN_FLIGHT];

    // Handle swapping and allocating back and front buffers to display what it is rendering (back) and what is showed (front).
    ComPtr<IDXGISwapChain4> _swapChain;

    // The texture result of drawing in the swapChain.
    std::unique_ptr<Texture> _renderBuffers[NUM_FRAMES_IN_FLIGHT];

    // Depth Stencil buffer.
    std::unique_ptr<Texture> _depthStencilBuffer;

    std::vector<std::unique_ptr<DescriptorAllocator>> _descriptorAllocators;
};

inline ID3D12Device5* ModuleID3D12::GetDevice() const
{
    CHIRON_TODO("Reduce calls, saving the comptr where its used mostly");
#if DEBUG
    HRESULT reason = _device->GetDeviceRemovedReason();

    if (reason != S_OK)
    {
        assert(false);
    }
#endif
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

inline Texture* ModuleID3D12::GetRenderBuffer() const
{
    return _renderBuffers[_currentBuffer].get();
}

inline Texture* ModuleID3D12::GetDepthStencilBuffer() const
{
    return _depthStencilBuffer.get();
}

inline DescriptorAllocator* ModuleID3D12::GetDescriptorAllocator(const D3D12_DESCRIPTOR_HEAP_TYPE& type) const
{
    return _descriptorAllocators[type].get();
}