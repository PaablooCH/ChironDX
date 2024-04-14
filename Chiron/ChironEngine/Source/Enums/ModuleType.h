#pragma once

class ModuleInput;
class ModuleWindow;

// Order matters: they will Init/start/update/cleanUp in this order
enum class ModuleType
{
	WINDOW,
	INPUT,
	LAST,
};

template<typename T>
struct ModuleToEnum
{
};

template<>
struct ModuleToEnum<ModuleWindow>
{
	const static ModuleType value = ModuleType::WINDOW;
};

template<>
struct ModuleToEnum<ModuleInput>
{
	const static ModuleType value = ModuleType::INPUT;
};