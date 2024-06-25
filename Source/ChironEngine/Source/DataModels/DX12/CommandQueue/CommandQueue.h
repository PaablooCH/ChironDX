#pragma once

class CommandList;

class CommandQueue
{
public:
	CommandQueue() = delete;
	CommandQueue(D3D12_COMMAND_LIST_TYPE type);
	~CommandQueue();

	// ------------- ACTIONS ----------------------

	uint64_t ExecuteCommandList(std::shared_ptr<CommandList> commandList);

	// ------------- SYNCHRONIZATION ----------------------

	void WaitForFenceValue(uint64_t fenceValue);
	void Flush();

	// ------------- GETTERS ----------------------

	std::shared_ptr<CommandList> GetCommandList();
	inline ID3D12CommandQueue* GetCommandQueue();
private:
	// ------------- CREATORS ----------------------

	std::shared_ptr<CommandList> CreateCommandList() const;

	// ------------- SYNCHRONIZATION ----------------------

	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
private:
	struct CommandListWaiting
	{
		std::shared_ptr<CommandList> commandList;
		uint64_t fenceValue;
	};

	ComPtr<ID3D12CommandQueue> _commandQueue;

	std::queue<CommandListWaiting> _commandListWaiting;

	ComPtr<ID3D12Fence> _fence;
	HANDLE _fenceEvent;
	uint64_t _fenceValue;

	D3D12_COMMAND_LIST_TYPE _type;
};

inline ID3D12CommandQueue* CommandQueue::GetCommandQueue()
{
	return _commandQueue.Get();
}