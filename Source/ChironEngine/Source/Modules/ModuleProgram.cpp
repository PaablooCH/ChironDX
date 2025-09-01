#include "Pch.h"
#include "ModuleProgram.h"

#include "DataModels/Programs/DefaultProgram.h"
#include "DataModels/Programs/GridProgram.h"

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
    _programs[static_cast<int>(ProgramType::GRID)] = std::make_unique<GridProgram>("Grid");

    return true;
}

bool ModuleProgram::CleanUp()
{
    _programs.clear();
    return true;
}