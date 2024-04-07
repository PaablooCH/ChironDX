#include "Pch.h"
#include <cstdarg>
#include "Event.h"

namespace Chiron
{
    Event::Event(const std::string& name, const EventType& type, EventCategory categories ...)
    {
        _name = name;
        _type = type;
        _categories = 0;

        // Fill _categories filter
        va_list args;
        va_start(args, categories);
        while (const EventCategory& arg = va_arg(args, EventCategory)) {
            _categories |= arg;
        }
        va_end(args);
    }
}