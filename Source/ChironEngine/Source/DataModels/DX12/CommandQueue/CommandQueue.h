#pragma once

class CommandQueue
{
public:
	CommandQueue() = delete;
	CommandQueue(D3D12_COMMAND_LIST_TYPE type, ComPtr<ID3D12Device> device);
	~CommandQueue();

	// ------------- ACTIONS ----------------------

	uint64_t ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList> commandList);

	// ------------- SYNCHRONIZATION ----------------------

	void WaitForFenceValue(uint64_t fenceValue);
	void Flush();

	// ------------- GETTERS ----------------------

	ComPtr<ID3D12GraphicsCommandList> GetCommandList();
	inline ID3D12CommandQueue* GetCommandQueue();
private:
	// ------------- CREATORS ----------------------

	ComPtr<ID3D12CommandAllocator> CreateCommandAllocator() const;
	ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12CommandAllocator> commandAllocator) const;

	// ------------- SYNCHRONIZATION ----------------------

	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
private:
	struct AllocatorWaiting
	{
		ComPtr<ID3D12CommandAllocator> commmandAllocator;
		uint64_t fenceValue;
	};

	ComPtr<ID3D12CommandQueue> _commandQueue;

	std::queue<AllocatorWaiting> _commandAllocators;
	std::queue<ComPtr<ID3D12GraphicsCommandList>> _commandLists;

	ComPtr<ID3D12Fence> _fence;
	HANDLE _fenceEvent;
	uint64_t _fenceValue;

	D3D12_COMMAND_LIST_TYPE _type;
};

inline ID3D12CommandQueue* CommandQueue::GetCommandQueue()
{
	return _commandQueue.Get();
}