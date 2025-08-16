#pragma once

#include "DataModels/FileSystem/UID/UID.h"
#include "Enums/FileType.h"

class Folder;

class AssetPicker
{
public:
    AssetPicker();
    ~AssetPicker() = default;

    bool Draw(FileType type, UID& actualUID);

private:
    void EndImGui();

private:
    FileType _selectedFileType;

    Folder* _rootFolder;
};

