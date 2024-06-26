#pragma once

#include "debug_draw.hpp"

namespace Chiron
{
    class Utils
    {
    public:

        Utils() = delete;

        // ------------- DIRECTX ----------------------

        // Helper utility converts D3D API failures into exceptions.
        static void ThrowIfFailed(HRESULT hr, const std::string& message = "") noexcept(false);

        // ------------- CONTAINERS ----------------------

        template<typename T>
        static inline void TryFrontPop(std::queue<T>& queue, T& element, bool& success);
        
        // ------------- HANDLES ----------------------

        static HANDLE CreateEventHandle();

        // ------------- DEBUG DRAW ----------------------

        static inline const ddVec3& ddConvert(const DirectX::SimpleMath::Vector3& v);
        static inline const ddMat4x4& ddConvert(const DirectX::SimpleMath::Matrix& m);
    };

    template<typename T>
    inline void Utils::TryFrontPop(std::queue<T>& queue, T& element, bool& success)
    {
        if (!queue.empty())
        {
            element = std::move(queue.front());
            queue.pop();
            success = true;
        }
        else
        {
            success = false;
        }
    }

    inline const ddVec3& Utils::ddConvert(const DirectX::SimpleMath::Vector3& v)
    {
        return reinterpret_cast<const ddVec3&>(v);
    }
    inline const ddMat4x4& Utils::ddConvert(const DirectX::SimpleMath::Matrix& m)
    {
        return reinterpret_cast<const ddMat4x4&>(m);
    }
}