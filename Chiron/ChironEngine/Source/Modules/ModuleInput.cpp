#include "Pch.h"
#include "ModuleInput.h"

ModuleInput::ModuleInput() : Module()
{
}

ModuleInput::~ModuleInput()
{
}

bool ModuleInput::Init()
{
	return true;
}

UpdateStatus ModuleInput::PreUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleInput::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	return true;
}
