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
	return false;
}

UpdateStatus ModuleInput::PreUpdate()
{
	return UpdateStatus();
}

UpdateStatus ModuleInput::Update()
{
	return UpdateStatus();
}

bool ModuleInput::CleanUp()
{
	return false;
}
