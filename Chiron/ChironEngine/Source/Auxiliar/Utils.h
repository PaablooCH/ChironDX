#pragma once

namespace Chiron
{
    class Utils
    {
    public:

        // Helper utility converts D3D API failures into exceptions.
        static inline void ThrowIfFailed(HRESULT hr, const std::string& message = "") noexcept(false)
        {
            if (FAILED(hr))
            {
                throw std::runtime_error("Error: " + message + " (HRESULT: 0x" + std::to_string(hr) + ")");
            }
        }
    };
}