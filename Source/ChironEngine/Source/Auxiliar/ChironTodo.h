#pragma once

#define _STR(x) #x
#define STR(x) _STR(x)

#define CHIRON_TODO(todoMessage) __pragma(message(__FILE__ "("STR(__LINE__)") : TODO: " todoMessage))
