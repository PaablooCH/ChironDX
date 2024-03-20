#pragma once
#include "ChironClasses/ChironTodo.h"

namespace chiron
{
	namespace detail
	{
		size_t FindPositionKey(const std::string& format);
		bool Format(std::string& format, int arg);
		bool Format(std::string& format, unsigned int arg);
		bool Format(std::string& format, float arg);
		bool Format(std::string& format, const char* arg);
		bool Format(std::string& format, const std::string& arg);
		bool Format(std::string& format, bool arg);
		bool Format(std::string& format, unsigned long long arg);
	}

	template<typename... Args>
	std::string Format(const std::string& format, Args&&... args)
	{
		std::string formattedString = format;
		CHIRON_TODO("change asserts to SDL_Assert")
		
		(
			[&]()
			{
				if (!detail::Format(formattedString, args))
				{
					assert(false && "Too many arguments in log call!");
				}
			}(),
			...); // variadic parameter unpacking

		assert(detail::FindPositionKey(formattedString) == std::string::npos && "Too few arguments in log call!");

		return formattedString;
	}
}

