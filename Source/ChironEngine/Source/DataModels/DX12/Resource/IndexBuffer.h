#pragma once
#include "Resource.h"

class IndexBuffer : public Resource
{
public:
	IndexBuffer() = delete;
	IndexBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t numIndices, const DXGI_FORMAT& indexFormat,
		const std::wstring& name = L"", const D3D12_CLEAR_VALUE* clearValue = nullptr);
	IndexBuffer(const IndexBuffer& copy);

	~IndexBuffer() override;

	// ------------- GETTERS ----------------------

	inline const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const;

private:
	size_t _numIndices;
	DXGI_FORMAT _format;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;
};

inline const D3D12_INDEX_BUFFER_VIEW& IndexBuffer::GetIndexBufferView() const
{
	return _indexBufferView;
}