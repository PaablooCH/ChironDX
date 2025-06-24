#pragma once
#include "DataModels/Window/Window.h"

#include <ImGui/imgui.h>

class SubWindow : public Window
{
public:
    virtual ~SubWindow() override;

    void Draw(const std::shared_ptr<CommandList>& commandList = nullptr) override;

protected:
    SubWindow(std::string name);

    virtual void DrawWindowContent(const std::shared_ptr<CommandList>& commandList = nullptr) = 0;

protected:
    ImGuiTreeNodeFlags _flags;

    inline static const ImVec4 _secondaryColor = ImVec4(59.f / 255.f, 186.f / 255.f, 115.f / 255.f, 1.f);
};
