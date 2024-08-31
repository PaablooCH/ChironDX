#include "Pch.h"
#include "ResourceStateTracker.h"

#include "DataModels/DX12/CommandList/CommandList.h"
#include "DataModels/DX12/Resource/Resource.h"

// Static definitions.
std::mutex ResourceStateTracker::_globalMutex;
bool ResourceStateTracker::_isLocked = false;
std::unordered_map<ID3D12Resource*, ResourceStateTracker::ResourceState> ResourceStateTracker::_globalResourceState;

ResourceStateTracker::ResourceStateTracker()
{
}

ResourceStateTracker::~ResourceStateTracker()
{
}

void ResourceStateTracker::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource)
{
    if (resource)
    {
        ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COMMON, stateAfter, subResource));
    }
}

void ResourceStateTracker::UAVBarrier(ID3D12Resource* resource)
{
    ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(resource));
}

void ResourceStateTracker::AliasingBarrier(ID3D12Resource* resourceBefore, ID3D12Resource* resourceAfter)
{
    ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(resourceBefore, resourceAfter));
}

uint32_t ResourceStateTracker::FlushPendingResourceBarriers(const CommandList* commandList)
{
    assert(_isLocked);

    std::vector<D3D12_RESOURCE_BARRIER> barriersToPush;
    barriersToPush.reserve(_pendingResourceBarriers.size());
    
    for (auto& pendingBarrier : _pendingResourceBarriers)
    {
        // All pendings are transition barriers
        D3D12_RESOURCE_TRANSITION_BARRIER pendingTransitionBarrier = pendingBarrier.Transition;
        const auto& iter = _globalResourceState.find(pendingTransitionBarrier.pResource);
        if (iter != _globalResourceState.end())
        {
            ResourceState& resourceState = iter->second;
            if (pendingTransitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
                !resourceState.SubresourceState.empty())
            {
                for (auto& subresourceState : resourceState.SubresourceState)
                {
                    if (subresourceState.second != pendingTransitionBarrier.StateAfter)
                    {
                        D3D12_RESOURCE_BARRIER newBarrier = pendingBarrier;
                        newBarrier.Transition.Subresource = subresourceState.first;
                        newBarrier.Transition.StateBefore = subresourceState.second;
                        barriersToPush.push_back(newBarrier);
                    }
                }
            }
            else
            {
                auto globalState = (iter->second).GetSubresourceState(pendingTransitionBarrier.Subresource);
                if (globalState != pendingTransitionBarrier.StateAfter)
                {
                    pendingBarrier.Transition.StateBefore = globalState;
                    barriersToPush.push_back(pendingBarrier);
                }
            }
        }
    }

    UINT numBarriers = static_cast<UINT>(barriersToPush.size());
    if (numBarriers > 0)
    {
        auto d3d12CommandList = commandList->GetGraphicsCommandList();
        d3d12CommandList->ResourceBarrier(numBarriers, barriersToPush.data());
    }
    _pendingResourceBarriers.clear();

    return numBarriers;
}

void ResourceStateTracker::FlushResourceBarriers(CommandList* commandList)
{
    UINT numBarriers = static_cast<UINT>(_resourceBarriers.size());
    if (numBarriers > 0)
    {
        std::vector<D3D12_RESOURCE_BARRIER> availableBarriers;
        availableBarriers.reserve(numBarriers);

        for (D3D12_RESOURCE_BARRIER barrier : _resourceBarriers)
        {
            bool exists = true;
            switch (barrier.Type)
            {
            case D3D12_RESOURCE_BARRIER_TYPE_TRANSITION:
                exists = ExistsResource(barrier.Transition.pResource);
                break;
            case D3D12_RESOURCE_BARRIER_TYPE_ALIASING:
                exists = ExistsResource(barrier.Aliasing.pResourceBefore) && 
                    ExistsResource(barrier.Aliasing.pResourceAfter);
                break;
            case D3D12_RESOURCE_BARRIER_TYPE_UAV:
                exists = ExistsResource(barrier.UAV.pResource);
                break;
            }

            if (exists)
            {
                availableBarriers.push_back(std::move(barrier));
            }
        }
        numBarriers = static_cast<UINT>(availableBarriers.size());
        auto d3d12CommandList = commandList->GetGraphicsCommandList();
        d3d12CommandList->ResourceBarrier(numBarriers, availableBarriers.data());

        _resourceBarriers.clear();
    }
}

void ResourceStateTracker::CommitFinalResourceStates()
{
    assert(_isLocked);

    for (const auto& finalResourceState : _finalResourceState)
    {
        // If the resource doesn't exist in _globalResourceState means it was destroyed, we don't want to insert it again
        if (ExistsResource(finalResourceState.first))
        {
            _globalResourceState[finalResourceState.first] = finalResourceState.second;
        }
    }

    _finalResourceState.clear();
}

void ResourceStateTracker::Reset()
{
    _pendingResourceBarriers.clear();
    _resourceBarriers.clear();
    _finalResourceState.clear();
}

void ResourceStateTracker::Lock()
{
    if (_isLocked == false)
    {
        _isLocked = true;
        _globalMutex.lock();
    }
}

void ResourceStateTracker::Unlock()
{
    if (_isLocked == true)
    {
        _isLocked = false;
        _globalMutex.unlock();
    }
}

void ResourceStateTracker::AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state)
{
    std::lock_guard<std::mutex> lock(_globalMutex);
    // if something in resources state fails, check this condition
    if (!ExistsResource(resource))
    {
        _globalResourceState[resource].SetSubresourceState(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
    }
}

void ResourceStateTracker::RemoveGlobalResourceState(ID3D12Resource* resource)
{
    std::lock_guard<std::mutex> lock(_globalMutex);
    _globalResourceState.erase(resource);
}

void ResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier)
{
    if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
    {
        const D3D12_RESOURCE_TRANSITION_BARRIER& transitionBarrier = barrier.Transition;

        const auto iter = _finalResourceState.find(transitionBarrier.pResource);
        if (iter != _finalResourceState.end())
        {
            auto& resourceState = iter->second;
            // If the known final state of the resource is different...
            if (transitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
                !resourceState.SubresourceState.empty())
            {
                // First transition all of the subresources if they are different than the StateAfter.
                for (auto& subresourceState : resourceState.SubresourceState)
                {
                    if (transitionBarrier.StateAfter != subresourceState.second)
                    {
                        D3D12_RESOURCE_BARRIER newBarrier = barrier;
                        newBarrier.Transition.Subresource = subresourceState.first;
                        newBarrier.Transition.StateBefore = subresourceState.second;
                        _resourceBarriers.push_back(newBarrier);
                    }
                }
            }
            else
            {
                auto finalState = resourceState.GetSubresourceState(transitionBarrier.Subresource);
                if (transitionBarrier.StateAfter != finalState)
                {
                    // Push a new transition barrier with the correct before state.
                    D3D12_RESOURCE_BARRIER newBarrier = barrier;
                    newBarrier.Transition.StateBefore = finalState;
                    _resourceBarriers.push_back(newBarrier);
                }
            }
        }
        else // In this case, the resource is being used on the command list for the first time. 
        {
            // Add a pending barrier. The pending barriers will be resolved
            // before the command list is executed on the command queue.
            _pendingResourceBarriers.push_back(barrier);
        }
        // Push the final known state (possibly replacing the previously known state for the subresource).
        _finalResourceState[transitionBarrier.pResource].SetSubresourceState(transitionBarrier.Subresource,
            transitionBarrier.StateAfter);
    }
    else
    {
        // Just push non-transition barriers to the resource barriers array.
        _resourceBarriers.push_back(barrier);
    }
}