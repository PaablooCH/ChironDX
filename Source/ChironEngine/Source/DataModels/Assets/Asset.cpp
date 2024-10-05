#include "Pch.h"
#include "Asset.h"

Asset::Asset(AssetType type) : _type(type)
{
}

Asset::~Asset()
{
    CHIRON_TODO("Guardar los binarios cuando se destruyan?");
}