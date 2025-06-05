#pragma once
#include "ComponentWindow.h"

class MeshRendererComponent;
class ComponentWindowFactory;

class RenderComponentWindow : public ComponentWindow
{
public:
    ~RenderComponentWindow() override;

private:
    friend class ComponentWindowFactory;

    RenderComponentWindow(MeshRendererComponent* component);

    void DrawWindowContent(const std::shared_ptr<CommandList>& commandList = nullptr) override;
    
    void RemoveAction() override;

    void DrawRenderWindow(const std::shared_ptr<CommandList>& commandList);
};

