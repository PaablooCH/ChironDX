#include "Pch.h"
#include "Utils.h"

void Chiron::Utils::ThrowIfFailed(HRESULT hr, const std::string& message) noexcept(false)
{
    if (FAILED(hr))
    {
        throw std::runtime_error("Error: " + message + " (HRESULT: 0x" + std::to_string(hr) + ")");
    }
}

HANDLE Chiron::Utils::CreateEventHandle()
{
    HANDLE event;

    event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(event && "Failed to create event.");

    return event;
}
