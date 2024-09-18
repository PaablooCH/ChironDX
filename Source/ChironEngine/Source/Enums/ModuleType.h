#pragma once

class ModuleCamera;
class ModuleFileSystem;
class ModuleID3D12;
class ModuleInput;
class ModuleProgram;
class ModuleRender;
class ModuleWindow;

// Order matters: they will Init/start/update/cleanUp in this order
enum class ModuleType
{
    WINDOW,
    ID3D12,
    FILE_SYSTEM,
    PROGRAM,
    INPUT,
    CAMERA,
    RENDER,
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
struct ModuleToEnum<ModuleID3D12>
{
    const static ModuleType value = ModuleType::ID3D12;
};

template<>
struct ModuleToEnum<ModuleFileSystem>
{
    const static ModuleType value = ModuleType::FILE_SYSTEM;
};

template<>
struct ModuleToEnum<ModuleProgram>
{
    const static ModuleType value = ModuleType::PROGRAM;
};

template<>
struct ModuleToEnum<ModuleInput>
{
    const static ModuleType value = ModuleType::INPUT;
};

template<>
struct ModuleToEnum<ModuleCamera>
{
    const static ModuleType value = ModuleType::CAMERA;
};

template<>
struct ModuleToEnum<ModuleRender>
{
    const static ModuleType value = ModuleType::RENDER;
};