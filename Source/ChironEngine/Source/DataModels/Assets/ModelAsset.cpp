#include "Pch.h"
#include "ModelAsset.h"

ModelAsset::ModelAsset(UID uid) : Asset(uid, AssetType::Model)
{
}

ModelAsset::ModelAsset(ModelAsset& copy) : Asset(copy)
{
    _nodes.reserve(copy._nodes.size());
    for (const auto& node : copy._nodes)
    {
        _nodes.push_back(std::make_unique<Node>(*node));
    }
}

ModelAsset::~ModelAsset()
{
}