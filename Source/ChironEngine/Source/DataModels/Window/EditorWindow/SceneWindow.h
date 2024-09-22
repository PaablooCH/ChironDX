#pragma once
#include "EditorWindow.h"

class SceneWindow : public EditorWindow
{
public:
    SceneWindow();
    ~SceneWindow() override;

private:
    void DrawWindowContent(const std::shared_ptr<CommandList>& commandList) override;
};

