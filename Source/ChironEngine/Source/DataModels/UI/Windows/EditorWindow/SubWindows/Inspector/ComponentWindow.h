#pragma once
#include "../SubWindow.h"

#include "DataModels/FileSystem/UID/UID.h"

class Component;

class ComponentWindow : public SubWindow
{
public:
    virtual ~ComponentWindow() override {};

    void Draw(const std::shared_ptr<CommandList>& commandList = nullptr) override;

protected:
    ComponentWindow(std::string name, Component* component, bool disableEnable = false, bool disableRemove = false);

    bool CollapsingHeader();

private:
    void DrawEnable();
    void DrawRemoveComponent();

protected:
    Component* _component;

private:
    UID _windowUID;

    bool _disableEnable;
    bool _disableRemove;
};
