#pragma once
#include "Module.h"

class TextureImporter;
class Resource;

class ModuleFileSystem : public Module
{
public:
    ModuleFileSystem();
    ~ModuleFileSystem() override;

    bool Init() override;
    bool CleanUp() override;

    void Import(const char* filePath, std::shared_ptr<Resource> resource);

    static std::string GetFileExtension(const char* path);

private:
    std::unique_ptr<TextureImporter> _textureImporter;
};

