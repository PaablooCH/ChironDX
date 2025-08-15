#pragma once
#include "ComponentWindow.h"

class MeshRendererComponent;
class ComponentWindowFactory;

class TextureAsset;

class RenderComponentWindow : public ComponentWindow
{
public:
    ~RenderComponentWindow() override;

private:
    friend class ComponentWindowFactory;

    RenderComponentWindow(MeshRendererComponent* component);

    void DrawWindowContent(const std::shared_ptr<CommandList>& commandList = nullptr) override;

    void DrawRenderWindow(const std::shared_ptr<CommandList>& commandList);

    std::shared_ptr<TextureAsset> a;
};

