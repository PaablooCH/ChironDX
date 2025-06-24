#pragma once

#include "Asset.h"

class MaterialAsset;
class MeshAsset;

struct Node
{
    std::string name;
    Matrix transform;
    int parent;
    std::vector<std::pair<std::shared_ptr<MeshAsset>, std::shared_ptr<MaterialAsset>>> meshMaterial;
};

class ModelAsset : public Asset
{
public:
    ModelAsset(UID uid);
    ~ModelAsset() override;

    inline void AddNode(Node* node);
    inline void ClearNodes();

    // ------------- GETTERS ----------------------

    inline const std::vector<std::unique_ptr<Node>>& GetNodes() const;

    // ------------- SETTERS ----------------------

    inline void SetNodes(std::vector<std::unique_ptr<Node>>& nodes);

private:
    std::vector<std::unique_ptr<Node>> _nodes;
};

inline void ModelAsset::AddNode(Node* node)
{
    _nodes.push_back(std::unique_ptr<Node>(node));
}

inline void ModelAsset::ClearNodes()
{
    _nodes.clear();
}

inline const std::vector<std::unique_ptr<Node>>& ModelAsset::GetNodes() const
{
    return _nodes;
}

inline void ModelAsset::SetNodes(std::vector<std::unique_ptr<Node>>& nodes)
{
    _nodes = std::move(nodes);
}
