#pragma once
#include "Module.h"

class TextureImporter;
class ModelImporter;
class Asset;

class ModuleFileSystem : public Module
{
public:
    ModuleFileSystem();
    ~ModuleFileSystem() override;

    bool Init() override;
    bool CleanUp() override;

    void Import(const char* filePath, const std::shared_ptr<Asset>& asset);

    static const std::string GetFileExtension(const char* path);
    static const std::string GetFileName(const std::string& path);

private:
    std::unique_ptr<TextureImporter> _textureImporter;
    std::unique_ptr<ModelImporter> _modelImporter;
};
