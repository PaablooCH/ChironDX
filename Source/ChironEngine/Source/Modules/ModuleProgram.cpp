#include "Pch.h"
#include "ModuleProgram.h"

#include "DataModels/Programs/DefaultProgram.h"

ModuleProgram::ModuleProgram()
{
}

ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Init()
{
    _programs.resize(static_cast<int>(ProgramType::SIZE));
    _programs[static_cast<int>(ProgramType::DEFAULT)] = std::make_unique<DefaultProgram>("Default");
    //_programs[static_cast<int>(ProgramType::GENERATE_MIPS)] = std::make_unique<GenerateMipsProgram>("GenerateMips");

    return true;
}

bool ModuleProgram::CleanUp()
{
    _programs.clear();
    return true;
}