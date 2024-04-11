#pragma once

class ModuleInput;

// Order matters: they will Init/start/update/cleanUp in this order
enum class ModuleType
{
	INPUT,
	LAST,
};

template<typename T>
struct ModuleToEnum
{
};

template<>
struct ModuleToEnum<ModuleInput>
{
	const static ModuleType value = ModuleType::INPUT;
};
