#pragma once

namespace Chiron
{
	enum class EventType
	{
		NONE = 0,
		KEY_HOLDED,
		KEY_PRESSED,
		KEY_RELEASED,
		MOUSE_MOVE,
		MOUSE_PRESSED,
		MOUSE_RELEASED,
		MOUSE_SCROLL,
		WINDOW_BORDERLESS,
		WINDOW_CLOSED,
		WINDOW_FULLSCREEN,
		WINDOW_MOVED,
		WINDOW_RESIZED
	};

	enum EventCategory
	{
		NONE = 0x00000000,
		APPLICATION = 0x00000001,
		INPUT = 0x00000002,
		KEYBOARD = 0x00000004,
		MOUSE = 0x00000006,
		MOUSE_BUTTON = 0x00000008,
		WINDOW = 0x00000010,
		OTHER = 0x80000000
	};

	class CHIRON_API Event
	{
	public:
		Event(const std::string& name, const EventType& type, EventCategory categories ...);
		~Event() = default;

		inline const std::string GetName() const;
		inline const EventType GetEventType() const;
		inline const int GetEventCategories() const;

		inline bool IsACategory(EventCategory category) const;

		virtual inline const std::string ToString() const;

	protected:
		std::string _name;
		EventType _type;
		int _categories;
	};

	inline const std::string Event::GetName() const
	{
		return _name;
	}

	inline const EventType Event::GetEventType() const
	{
		return _type;
	}

	inline const int Event::GetEventCategories() const
	{
		return _categories;
	}

	inline bool Event::IsACategory(EventCategory category) const
	{
		return  category & GetEventCategories();
	}

	inline const std::string Event::ToString() const
	{
		return GetName();
	}
}