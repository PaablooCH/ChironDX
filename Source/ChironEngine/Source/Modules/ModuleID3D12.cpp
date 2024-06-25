#include "Pch.h"
#include "ModuleID3D12.h"

#include "Application.h"

#include "ModuleWindow.h"

#include "DataModels/DX12/CommandQueue/CommandQueue.h"
#include "DataModels/DX12/DescriptorAllocator/DescriptorAllocator.h"

ModuleID3D12::ModuleID3D12() : _currentBuffer(0), _vSync(true), _tearingSupported(false)
{
}

ModuleID3D12::~ModuleID3D12()
{
}

bool ModuleID3D12::Init()
{
    bool ok = CreateFactory();
    ok = ok && CreateAdapter();
    ok = ok && CreateDevice();
    ok = ok && CreateCommandQueue();
    ok = ok && CreateSwapChain();

    if (ok)
    {
        InitFrameBuffer();
        InitDescriptorAllocator();
        _currentBuffer = _swapChain->GetCurrentBackBufferIndex();
    }

    return ok;
}

UpdateStatus ModuleID3D12::PreUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleID3D12::Update()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleID3D12::PostUpdate()
{
    UINT syncInterval = _vSync ? 1 : 0;
    UINT presentFlags = _tearingSupported && !_vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    Chiron::Utils::ThrowIfFailed(_swapChain->Present(syncInterval, presentFlags));

    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleID3D12::CleanUp()
{
    Flush();

    return true;
}

void ModuleID3D12::SwapCurrentBuffer()
{
    _currentBuffer = _swapChain->GetCurrentBackBufferIndex();

    _commandQueueDirect->WaitForFenceValue(_bufferFenceValues[_currentBuffer]);
}

void ModuleID3D12::SaveCurrentBufferFenceValue(const uint64_t& fenceValue)
{
    _bufferFenceValues[_currentBuffer] = fenceValue;
}

void ModuleID3D12::ToggleVSync()
{
    _vSync = !_vSync;
}

void ModuleID3D12::ResizeBuffers(unsigned newWidth, unsigned newHeight)
{
    Flush();
    
    // ------------- SWAP-CHAIN ---------------------------

    for (int i = 0; i < backBufferCount; ++i)
    {
        // Any references to the back buffers must be released
        // before the swap chain can be resized.
        _renderBuffers[i].Reset();
        _bufferFenceValues[i] = _bufferFenceValues[_currentBuffer];
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    Chiron::Utils::ThrowIfFailed(_swapChain->GetDesc(&swapChainDesc)); // Get the current descr to apply it to the newer.
    Chiron::Utils::ThrowIfFailed(_swapChain->ResizeBuffers(backBufferCount, newWidth, newHeight,
        swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

    _currentBuffer = _swapChain->GetCurrentBackBufferIndex();

    UpdateRenderTargetViews();

    // ------------- DEPTH-STENCIL ---------------------------
 
    CreateDepthStencil(newWidth, newHeight);
}

void ModuleID3D12::CreateTransitionBarrier(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12Resource> resource, 
    D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), stateBefore, stateAfter);
    commandList->ResourceBarrier(1, &barrier);
}

void ModuleID3D12::CreateAliasingBarrier(ComPtr<ID3D12GraphicsCommandList> commandList, 
    ComPtr<ID3D12Resource> resourceBefore, ComPtr<ID3D12Resource> resourceAfter)
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(resourceBefore.Get(), resourceAfter.Get());
    commandList->ResourceBarrier(1, &barrier);
}

void ModuleID3D12::CreateUAVBarrier(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12Resource> resource)
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(resource.Get());
    commandList->ResourceBarrier(1, &barrier);
}

void ModuleID3D12::UpdateBufferResource(ComPtr<ID3D12GraphicsCommandList> commandList, 
    ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource, size_t numElements, size_t elementSize,
    const void* bufferData, D3D12_RESOURCE_FLAGS flags)
{
    size_t bufferSize = numElements * elementSize;

    CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);

    // Create a committed resource for the GPU resource in a default heap.
    Chiron::Utils::ThrowIfFailed(_device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE, 
        &resourceDesc,
        D3D12_RESOURCE_STATE_COMMON, 
        nullptr,
        IID_PPV_ARGS(pDestinationResource)));

    if (bufferData)
    {
        heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        // Create a commited resource in upload heap
        Chiron::Utils::ThrowIfFailed(_device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(pIntermediateResource)));
        
        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = bufferData;
        subresourceData.RowPitch = bufferSize;
        subresourceData.SlicePitch = subresourceData.RowPitch;
        
        UpdateSubresources(commandList.Get(), *pDestinationResource, *pIntermediateResource, 0, 0, 1, &subresourceData);
    }
}

void ModuleID3D12::Flush()
{
    _commandQueueDirect->Flush();
    _commandQueueCompute->Flush();
    _commandQueueCopy->Flush();
}

uint64_t ModuleID3D12::Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue)
{
    uint64_t fenceValueForSignal = ++fenceValue;
    Chiron::Utils::ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValueForSignal));

    return fenceValueForSignal;
}

void ModuleID3D12::WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, 
    std::chrono::milliseconds duration)
{
    if (fence->GetCompletedValue() < fenceValue)
    {
        Chiron::Utils::ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
        ::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
    }
}

bool ModuleID3D12::CreateFactory()
{
    UINT dxgiFactoryFlags = 0;
#ifdef DEBUG
    // Create a Debug Controller to track errors
    ID3D12Debug* dc;
    Chiron::Utils::ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&dc)));
    Chiron::Utils::ThrowIfFailed(dc->QueryInterface(IID_PPV_ARGS(&_debugController)));
    _debugController->EnableDebugLayer();
    _debugController->SetEnableGPUBasedValidation(true);

    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

    dc->Release();
    dc = nullptr;
#endif

    HRESULT result = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&_factory));
    return SUCCEEDED(result);
}

bool ModuleID3D12::CreateAdapter()
{
    ComPtr<IDXGIAdapter1> adapter1;

    bool ok = false;
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != _factory->EnumAdapters1(adapterIndex, &adapter1); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter1->GetDesc1(&desc);

        // Don't select the Basic Render Driver adapter.
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        // Check if the adapter supports Direct3D 12, and use that for the rest
        // of the application
        if (SUCCEEDED(D3D12CreateDevice(_adapter.Get(), D3D_FEATURE_LEVEL_12_0,
            _uuidof(ID3D12Device), nullptr)))
        {
            adapter1.As(&_adapter);
            ok = true;
        }
    }

    if (ok)
    {
        ComPtr<IDXGIFactory5> factory5;

        if (SUCCEEDED(_factory.As(&factory5)))
        {
            BOOL tearing = FALSE;
            factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearing, sizeof(tearing));

            _tearingSupported = tearing == TRUE;
        }
    }

    return ok;
}

bool ModuleID3D12::CreateDevice()
{
    HRESULT result = D3D12CreateDevice(_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_device));
    _device->SetName(L"Device");
    bool ok = SUCCEEDED(result);

#ifdef DEBUG
    // Get debug device
    ok = ok && SUCCEEDED(_device->QueryInterface(IID_PPV_ARGS(&_debugDevice)));
#endif

    return ok;
}

bool ModuleID3D12::CreateCommandQueue()
{
    _commandQueueDirect = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
    _commandQueueCompute = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
    _commandQueueCopy = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);

    return true;
}

bool ModuleID3D12::CreateSwapChain()
{
    unsigned width;
    unsigned height;

    auto windowModule = App->GetModule<ModuleWindow>();
    windowModule->GetWindowSize(width, height);
    HWND hwnd = windowModule->GetWindowId();

    if (_swapChain != nullptr)
    {
        // Create Render Target Attachments from swapchain
        _swapChain->ResizeBuffers(backBufferCount, width, height,
            DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        return true;
    }
    else
    {
        // Create swapchain
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc = { 1, 0 };
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = _tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

        ComPtr<IDXGISwapChain1> newSwapchain1;
        Chiron::Utils::ThrowIfFailed(_factory->CreateSwapChainForHwnd(_commandQueueDirect->GetCommandQueue(), hwnd, 
            &swapChainDesc, nullptr, nullptr, &newSwapchain1));

        Chiron::Utils::ThrowIfFailed(newSwapchain1.As(&_swapChain));

        // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
        // will be handled manually.
        Chiron::Utils::ThrowIfFailed(_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
        
        return true;
    }

    return false;
}

void ModuleID3D12::CreateDepthStencil(unsigned width, unsigned height)
{
    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil = { 1.0f, 0 };
    CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, 
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    Chiron::Utils::ThrowIfFailed(
        _device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &clearValue, IID_PPV_ARGS(&_depthStencilBuffer)));

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    Chiron::Utils::ThrowIfFailed(_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_dsvHeap)));

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_dsvHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Texture2D.MipSlice = 0;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

    _device->CreateDepthStencilView(_depthStencilBuffer.Get(), &depthStencilDesc, dsvHandle);
    _depthStencilBuffer->SetName(L"Depth Stencil Buffer");
}

void ModuleID3D12::UpdateRenderTargetViews()
{
    _renderTargetViewDesciptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < backBufferCount; ++i)
    {
        ComPtr<ID3D12Resource> backBuffer;
        Chiron::Utils::ThrowIfFailed(_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

        _device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

        _renderBuffers[i] = backBuffer;
        _renderBuffers[i]->SetName(L"Render Buffer " + i);

        rtvHandle.Offset(_renderTargetViewDesciptorSize);
    }
}

void ModuleID3D12::InitFrameBuffer()
{
    // ------------- RTV ---------------------------

    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = backBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    Chiron::Utils::ThrowIfFailed(_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_renderTargetViewHeap)));

    _renderTargetViewDesciptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Create frame resources
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV for each frame.
    for (UINT n = 0; n < backBufferCount; n++)
    {
        Chiron::Utils::ThrowIfFailed(_swapChain->GetBuffer(n, IID_PPV_ARGS(&_renderBuffers[n])));
        _device->CreateRenderTargetView(_renderBuffers[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(_renderTargetViewDesciptorSize);
    }

    // ------------- DEPTH-STENCIL ---------------------------
    
    unsigned width;
    unsigned height;

    App->GetModule<ModuleWindow>()->GetWindowSize(width, height);

    CreateDepthStencil(width, height);
}

void ModuleID3D12::InitDescriptorAllocator()
{
    _descriptorAllocators.reserve(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);

    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
    {
        _descriptorAllocators.push_back(std::make_unique<DescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i)));
    }
}
