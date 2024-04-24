#pragma once
#include "Module.h"

class Program;

enum class ProgramType
{
    DEFAULT,
    SIZE
};

class ModuleProgram : public Module
{
public:
    ModuleProgram();
    ~ModuleProgram() override;

    bool Init() override;
    bool CleanUp() override;

    // ------------- GETTERS ----------------------

    inline Program* GetProgram(ProgramType type);

private:
    // ------------- CREATORS ----------------------

    std::unique_ptr<Program> CreateDefaultProgram();
    std::unique_ptr<Program> CreateProgram(const LPCWSTR& shaderFile, const D3D12_INPUT_ELEMENT_DESC inputElementDescs[],
        const std::string& name);

private:
    std::vector<std::unique_ptr<Program>> _programs;
};

inline Program* ModuleProgram::GetProgram(ProgramType type)
{
    if (_programs.empty() || type == ProgramType::SIZE || _programs.size() > static_cast<int>(ProgramType::SIZE))
    {
        return nullptr;
    }
    else
    {
        return _programs[static_cast<int>(type)].get();
    }
}