#include "Pch.h"
#include "ModuleRender.h"

ModuleRender::ModuleRender()
{
}

ModuleRender::~ModuleRender()
{
}

bool ModuleRender::Init()
{
    return true;
}

UpdateStatus ModuleRender::PreUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::Update()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
    return true;
}
