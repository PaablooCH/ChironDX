#include "Pch.h"
#include "CommandQueue.h"

#include "Application.h"

#include "Modules/ModuleID3D12.h"

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type, ComPtr<ID3D12Device> device) : _type(type), _fenceValue(0)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = _type;
	Chiron::Utils::ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue)));

	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = Chiron::Utils::CreateEventHandle();
}

CommandQueue::~CommandQueue()
{
}

uint64_t CommandQueue::ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	commandList->Close();

	ID3D12CommandAllocator* commandAllocator;
	UINT allocatorSize = sizeof(commandAllocator);
	Chiron::Utils::ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &allocatorSize, 
		&commandAllocator));

	ID3D12CommandList* const commandLists[] = {
		commandList.Get()
	};
	_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	uint64_t fenceValue = Signal();

	_commandAllocators.emplace(AllocatorWaiting{ commandAllocator, fenceValue });
	_commandLists.push(commandList);

	commandAllocator->Release(); // free CommandLine private data memory

	return fenceValue;
}

void CommandQueue::Flush()
{
	WaitForFenceValue(Signal());
}

ComPtr<ID3D12GraphicsCommandList> CommandQueue::GetCommandList()
{
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;

	bool success;

	if (!_commandAllocators.empty() && IsFenceComplete(_commandAllocators.front().fenceValue))
	{
		AllocatorWaiting alloWaiting;

		Chiron::Utils::TryFrontPop(_commandAllocators, alloWaiting, success);
		commandAllocator = alloWaiting.commmandAllocator;
		Chiron::Utils::ThrowIfFailed(commandAllocator->Reset());
	}
	else
	{
		commandAllocator = CreateCommandAllocator();
	}
	
	Chiron::Utils::TryFrontPop(_commandLists, commandList, success);
	if (success)
	{
		Chiron::Utils::ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
	}
	else
	{
		commandList = CreateCommandList(commandAllocator);
	}

	commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get());

	return commandList;
}

ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator() const
{
	ComPtr<ID3D12CommandAllocator> commandAllocator;

	auto device = App->GetModule<ModuleID3D12>()->GetDevice();
	device->CreateCommandAllocator(_type, IID_PPV_ARGS(&commandAllocator));

	return commandAllocator;
}

ComPtr<ID3D12GraphicsCommandList> CommandQueue::CreateCommandList(ComPtr<ID3D12CommandAllocator> commandAllocator) const
{
	ComPtr<ID3D12GraphicsCommandList> commandList;

	auto device = App->GetModule<ModuleID3D12>()->GetDevice();
	Chiron::Utils::ThrowIfFailed(device->CreateCommandList(0, _type, commandAllocator.Get(), nullptr, 
		IID_PPV_ARGS(&commandList)));

	return commandList;
}

uint64_t CommandQueue::Signal()
{
	uint64_t fenceValue = ++_fenceValue;
	Chiron::Utils::ThrowIfFailed(_commandQueue->Signal(_fence.Get(), fenceValue));
	return fenceValue;
}

void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
	if (!IsFenceComplete(fenceValue))
	{
		Chiron::Utils::ThrowIfFailed(_fence->SetEventOnCompletion(fenceValue, _fenceEvent));
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	return _fence->GetCompletedValue() >= fenceValue;
}
