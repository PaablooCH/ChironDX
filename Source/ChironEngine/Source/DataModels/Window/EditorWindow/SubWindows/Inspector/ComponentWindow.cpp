#include "Pch.h"
#include "ComponentWindow.h"

#include "DataModels/Components/Component.h"

#include "DataModels/GameObject/GameObject.h"

#include "DataModels/FileSystem/UID/UIDGenerator.h"

#include <sstream>

void ComponentWindow::Draw(const std::shared_ptr<CommandList>& commandList)
{
    if (CollapsingHeader())
    {
        std::ostringstream childString;
        childString << "##com" << _windowUID;
        if (ImGui::BeginChild(childString.str().c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY))
        {
            std::ostringstream windowTableName;
            windowTableName << "###ComponentWindowTable" << _windowUID;
            if (ImGui::BeginTable(windowTableName.str().c_str(), 2))
            {
                std::ostringstream firstCol;
                std::ostringstream secondCol;

                firstCol << "###FirstCol" << _windowUID;
                secondCol << "###SecondCol" << _windowUID;
                
                ImGui::TableSetupColumn(firstCol.str().c_str(), ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(secondCol.str().c_str(), ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextColumn();
                ImGui::Dummy(ImVec2(0.5f, 0.f));
                
                ImGui::TableNextColumn();
                DrawWindowContent(commandList);
                
                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }
}

ComponentWindow::ComponentWindow(std::string name, Component* component, bool disableEnable, bool disableRemove) :
    SubWindow(name), _component(component), _windowUID(Chiron::UIDGenerator::GenerateUID()), _disableEnable(disableEnable),
    _disableRemove(disableRemove)
{
    _flags |= ImGuiTreeNodeFlags_AllowOverlap;
}

bool ComponentWindow::CollapsingHeader()
{
    bool open = ImGui::CollapsingHeader(_name.c_str(), _flags);
    if (!_disableEnable)
    {
        DrawEnable();
    }
    if (!_disableRemove)
    {
        DrawRemoveComponent();
    }
    return open;
}

void ComponentWindow::DrawEnable()
{
    if (_component)
    {
        std::ostringstream oss;

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 55);
        bool enable = _component->IsEnabled();
        oss << (enable ? ICON_FA_EYE : ICON_FA_EYE_SLASH) << "##Enabled" << _windowUID;
        if (ImGui::Button(oss.str().c_str()))
        {
            _component->SetEnabled(!enable);
        }
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("Enable/Disable");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}

void ComponentWindow::DrawRemoveComponent()
{
    if (_component)
    {
        std::ostringstream oss;
        oss << ICON_FA_TRASH "###" << _windowUID;

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 25);
        if (ImGui::Button(oss.str().c_str()))
        {
            _component->GetOwner()->RemoveComponent(_component);
            _component = nullptr;
        }
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("Remove Component");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        ImGui::Dummy(ImVec2(30, 0));
    }
}

void ComponentWindow::RemoveAction()
{
}
