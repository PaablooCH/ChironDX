#pragma once
#include "ComponentWindow.h"

#include "DataModels/UI/Components/AssetPicker.h"
#include "Enums/TextureType.h"

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
    void DrawTexture(const std::shared_ptr<CommandList>& commandList, TextureType textureType, const char* label);

private:
    AssetPicker _assetPicker;
};

