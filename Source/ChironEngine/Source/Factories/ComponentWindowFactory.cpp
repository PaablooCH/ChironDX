#include "Pch.h"
#include "ComponentWindowFactory.h"

#include "DataModels/Components/MeshRendererComponent.h"
#include "DataModels/Components/TransformComponent.h"

#include "DataModels/GameObject/GameObject.h"

#include "DataModels/UI/Windows/EditorWindow/SubWindows/Inspector/MeshComponentWindow.h"
#include "DataModels/UI/Windows/EditorWindow/SubWindows/Inspector/RenderComponentWindow.h"
#include "DataModels/UI/Windows/EditorWindow/SubWindows/Inspector/TransformComponentWindow.h"

std::vector<std::unique_ptr<ComponentWindow>> ComponentWindowFactory::CreateComponentsWindow(const ComponentsView& component)
{
    std::vector<std::unique_ptr<ComponentWindow>> windows;
    windows.reserve(component.size());
    for (Component* comp : component)
    {
        switch (comp->GetType())
        {
        case ComponentType::TRANSFORM:
            if (!comp->GetOwner()->IsRoot())
            {
                auto window = new TransformComponentWindow(static_cast<TransformComponent*>(comp));
                windows.push_back(std::unique_ptr<ComponentWindow>(window));
            }
            break;
        case ComponentType::MESH_RENDERER:
            auto meshRenderer = static_cast<MeshRendererComponent*>(comp);
            auto meshWindow = new MeshComponentWindow(meshRenderer);
            windows.push_back(std::unique_ptr<ComponentWindow>(meshWindow));

            auto renderWindow = new RenderComponentWindow(static_cast<MeshRendererComponent*>(comp));
            windows.push_back(std::unique_ptr<ComponentWindow>(renderWindow));
            break;
        }
    }
    
    return windows;
}