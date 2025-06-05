#pragma once
#include "ComponentWindow.h"

class MeshRendererComponent;
class ComponentWindowFactory;

class MeshComponentWindow : public ComponentWindow
{
public:
    ~MeshComponentWindow() override;

private:
    friend class ComponentWindowFactory;
    
    MeshComponentWindow(MeshRendererComponent* component);

    void DrawWindowContent(const std::shared_ptr<CommandList>& commandList = nullptr) override;
    void RemoveAction() override;

    void DrawMeshWindow();
};

