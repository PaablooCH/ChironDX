#include "Pch.h"
#include "ModuleProgram.h"

#include "Application.h"

#include "ModuleID3D12.h"

#include "DataModels/Programs/ProgramDefault.h"

ModuleProgram::ModuleProgram()
{
}

ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Init()
{
    _programs.resize(static_cast<int>(ProgramType::SIZE));
    _programs[static_cast<int>(ProgramType::DEFAULT)] = std::make_unique<ProgramDefault>("Default");
	
    return true;
}

bool ModuleProgram::CleanUp()
{
    _programs.clear();
    return true;
}