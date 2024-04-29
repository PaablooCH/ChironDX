#pragma once

namespace Chiron
{
    class Utils
    {
    public:

        // ------------- DIRECTX ----------------------
        // Helper utility converts D3D API failures into exceptions.
        static void ThrowIfFailed(HRESULT hr, const std::string& message = "") noexcept(false);

        // ------------- CONTAINERS ----------------------

        template<typename T>
        static inline void TryFrontPop(std::queue<T>& queue, T& element, bool& success);
        
        // ------------- HANDLES ----------------------

        static HANDLE CreateEventHandle();
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
}