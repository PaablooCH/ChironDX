#pragma once

#include "Alias/ComponentsView.h"

class ComponentWindow;

class ComponentWindowFactory
{
public:
   static std::vector<std::unique_ptr<ComponentWindow>> CreateComponentsWindow(const ComponentsView& component);

private:
    ComponentWindowFactory() = delete;
    ~ComponentWindowFactory() = delete;
};
