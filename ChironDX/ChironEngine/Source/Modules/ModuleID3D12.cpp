#include "Pch.h"
#include "ModuleID3D12.h"

#include "Application.h"

#include "ModuleWindow.h"

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
    ok = ok && CreateCommandAllocator();
    ok = ok && CreateCommandQueue();
    ok = ok && CreateFence();
    ok = ok && CreateSwapChain();

    InitFrameBuffer();
    InitResources();

    if (ok)
    {
        _currentBuffer = _swapChain->GetCurrentBackBufferIndex();
    }

    return ok;
}

UpdateStatus ModuleID3D12::PreUpdate()
{
    _commandAllocator[_currentBuffer]->Reset();
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

    _frameValues[_currentBuffer] = Signal(_commandQueue, _fence, _fenceValue);

    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleID3D12::CleanUp()
{
    Flush(_commandQueue, _fence, _fenceValue, _fenceEvent);

    return ::CloseHandle(_fenceEvent);
}

void ModuleID3D12::SwapCurrentBuffer()
{
    _currentBuffer = _swapChain->GetCurrentBackBufferIndex();

    WaitForFenceValue(_fence, _frameValues[_currentBuffer], _fenceEvent);
}

void ModuleID3D12::ToggleVSync()
{
    _vSync = !_vSync;
}

void ModuleID3D12::ResizeSwapChain(unsigned newWidth, unsigned newHeight)
{
    Flush(_commandQueue, _fence, _frameValues[_currentBuffer], _fenceEvent);
    
    for (int i = 0; i < backbufferCount; ++i)
    {
        // Any references to the back buffers must be released
        // before the swap chain can be resized.
        _renderBuffers[i].Reset();
        _frameValues[i] = _frameValues[_currentBuffer];
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    Chiron::Utils::ThrowIfFailed(_swapChain->GetDesc(&swapChainDesc)); // Get the current descr to apply it to the newer.
    Chiron::Utils::ThrowIfFailed(_swapChain->ResizeBuffers(backbufferCount, newWidth, newHeight,
        swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

    _currentBuffer = _swapChain->GetCurrentBackBufferIndex();

    UpdateRenderTargetViews();
}

ComPtr<ID3D12GraphicsCommandList> ModuleID3D12::CreateCommandList(ID3D12CommandAllocator* commandAllocator, 
    D3D12_COMMAND_LIST_TYPE type, const LPCWSTR& name)
{
    ComPtr<ID3D12GraphicsCommandList> commandList;

    Chiron::Utils::ThrowIfFailed(_device->CreateCommandList(
        0,
        type,
        commandAllocator,
        nullptr,
        IID_PPV_ARGS(&commandList)));
    Chiron::Utils::ThrowIfFailed(commandList->Close());
    Chiron::Utils::ThrowIfFailed(commandList->SetName(name));
    return commandList;
}

bool ModuleID3D12::CreateBarrier(ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_RESOURCE_BARRIER_TYPE type)
{
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = type;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    //barrier.Transition.pResource = texResource;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    commandList->ResourceBarrier(1, &barrier);
    return true;
}

HANDLE ModuleID3D12::CreateEventHandle()
{
    HANDLE fenceEvent;

    fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent && "Failed to create fence event.");

    return fenceEvent;
}

void ModuleID3D12::Flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, 
    HANDLE fenceEvent)
{
    uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);
    WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
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
    bool ok = SUCCEEDED(result);

#ifdef DEBUG
    // Get debug device
    ok = ok && SUCCEEDED(_device->QueryInterface(IID_PPV_ARGS(&_debugDevice)));
#endif

    return ok;
}

bool ModuleID3D12::CreateCommandAllocator()
{
    bool ok = true;
    for (int i = 0; i < backbufferCount; i++)
    {
        HRESULT result = _device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, 
            IID_PPV_ARGS(&_commandAllocator[i]));
        ok = ok && SUCCEEDED(result);
    }
        
    return ok;
}

bool ModuleID3D12::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    HRESULT result = _device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue));
    return SUCCEEDED(result);
}

bool ModuleID3D12::CreateFence()
{
    // Create fence
    HRESULT result = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    _fenceEvent = CreateEventHandle();
    return SUCCEEDED(result);
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
        _swapChain->ResizeBuffers(backbufferCount, width, height,
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
        swapChainDesc.BufferCount = backbufferCount;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = _tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

        ComPtr<IDXGISwapChain1> newSwapchain1;
        Chiron::Utils::ThrowIfFailed(_factory->CreateSwapChainForHwnd(_commandQueue.Get(), hwnd, &swapChainDesc, nullptr,
            nullptr, &newSwapchain1));

        Chiron::Utils::ThrowIfFailed(newSwapchain1.As(&_swapChain));

        // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
        // will be handled manually.
        Chiron::Utils::ThrowIfFailed(_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
        
        return true;
    }

    return false;
}

void ModuleID3D12::UpdateRenderTargetViews()
{
    _renderTargetViewDesciptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < backbufferCount; ++i)
    {
        ComPtr<ID3D12Resource> backBuffer;
        Chiron::Utils::ThrowIfFailed(_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

        _device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

        _renderBuffers[i] = backBuffer;

        rtvHandle.Offset(_renderTargetViewDesciptorSize);
    }
}

void ModuleID3D12::InitFrameBuffer()
{
    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = backbufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    Chiron::Utils::ThrowIfFailed(_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_renderTargetViewHeap)));

    _renderTargetViewDesciptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Create frame resources
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV for each frame.
    for (UINT n = 0; n < backbufferCount; n++)
    {
        Chiron::Utils::ThrowIfFailed(_swapChain->GetBuffer(n, IID_PPV_ARGS(&_renderBuffers[n])));
        _device->CreateRenderTargetView(_renderBuffers[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(_renderTargetViewDesciptorSize);
    }
}

bool ModuleID3D12::InitResources()
{
    // ------------ Root Signature ------------

    // Determine if we can get Root Signature Version 1.1:
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        LOG_WARNING("Root Signature Version 1.1 not supported");
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    // Individual GPU Resources
    D3D12_DESCRIPTOR_RANGE1 ranges[1]{};
    ranges[0].BaseShaderRegister = 0;
    ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    ranges[0].NumDescriptors = 1;
    ranges[0].RegisterSpace = 0;
    ranges[0].OffsetInDescriptorsFromTableStart = 0;
    ranges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

    // Groups of GPU Resources
    D3D12_ROOT_PARAMETER1 rootParameters[1]{};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[0].DescriptorTable.pDescriptorRanges = ranges;

    // Overall Layout
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Version = featureData.HighestVersion;
    rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.Desc_1_1.NumParameters = 1;
    rootSignatureDesc.Desc_1_1.pParameters = rootParameters;
    rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
    rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;

    ID3DBlob* signature = nullptr;
    ID3DBlob* error = nullptr;
    try
    {
        // Create the root signature
        Chiron::Utils::ThrowIfFailed(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error));
        Chiron::Utils::ThrowIfFailed(_device->CreateRootSignature(0, signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&_rootSignature)));
        _rootSignature->SetName(L"Hello Triangle Root Signature");
    }
    catch (std::exception e)
    {
        const char* errStr = (const char*)error->GetBufferPointer();
        LOG_ERROR(errStr);
        error->Release();
        error = nullptr;
    }

    if (signature != nullptr)
    {
        signature->Release();
        signature = nullptr;
    }

    return true;
}
