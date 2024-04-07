#pragma once
#include "WindowEvent.h"

namespace Chiron
{
    class WindowResizedEvent : public WindowEvent
    {
    public:
        WindowResizedEvent(const std::string& name, unsigned int width, unsigned int height);

        inline unsigned int GetWidht() const;
        inline unsigned int GetHeight() const;

        inline const std::string ToString() const override;
    private:
        unsigned int _width;
        unsigned int _height;
    };

    inline unsigned int WindowResizedEvent::GetWidht() const
    {
        return _width;
    }

    inline unsigned int WindowResizedEvent::GetHeight() const
    {
        return _height;
    }

    inline const std::string WindowResizedEvent::ToString() const
    {
        return "Width: " + std::to_string(_width) + ", Height: " + std::to_string(_height) + ".";
    }
}