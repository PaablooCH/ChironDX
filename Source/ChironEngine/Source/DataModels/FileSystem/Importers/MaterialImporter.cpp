#include "Pch.h"
#include "MaterialImporter.h"

#include "Application.h"

#include "Modules/ModuleAssets.h"
#include "Modules/ModuleFileSystem.h"
#include "Modules/ModuleResources.h"

#include "DataModels/Assets/MaterialAsset.h"
#include "DataModels/Assets/TextureAsset.h"

#include "Defines/FileSystemDefine.h"

MaterialImporter::MaterialImporter()
{
}

MaterialImporter::~MaterialImporter()
{
}

void MaterialImporter::Import(const char* filePath, const std::shared_ptr<MaterialAsset>& material)
{
    material->SetName(ModuleFileSystem::GetFile(filePath));

    rapidjson::Document doc;
    Json json = Json(doc);
    ModuleFileSystem::LoadJson(filePath, json);

    auto resources = App->GetModule<ModuleResources>();

    std::future<std::shared_ptr<TextureAsset>> futureBase;
    std::future<std::shared_ptr<TextureAsset>> futureNormalMap;
    std::future<std::shared_ptr<TextureAsset>> futureOcclusion;
    std::future<std::shared_ptr<TextureAsset>> futureProperty;
    std::future<std::shared_ptr<TextureAsset>> futureEmissive;

    bool hasBase = false;
    bool hasNormal = false;
    bool hasOcclusion = false;
    bool hasProperty = false;
    bool hasEmissive = false;

    UID textureUID = json["baseTextureUID"];
    if (textureUID != 0)
    {
        futureBase = resources->SearchAsset<TextureAsset>(textureUID);
        hasBase = true;
    }

    textureUID = json["normalMapUID"];
    if (textureUID != 0)
    {
        futureNormalMap = resources->SearchAsset<TextureAsset>(textureUID);
        hasNormal = true;
    }

    textureUID = json["ambientOcclusionUID"];
    if (textureUID != 0)
    {
        futureOcclusion = resources->SearchAsset<TextureAsset>(textureUID);
        hasOcclusion = true;
    }

    textureUID = json["propertyTextureUID"];
    if (textureUID != 0)
    {
        futureProperty = resources->SearchAsset<TextureAsset>(textureUID);
        hasProperty = true;
    }

    textureUID = json["emissiveTextureUID"];
    if (textureUID != 0)
    {
        futureEmissive = resources->SearchAsset<TextureAsset>(textureUID);
        hasEmissive = true;
    }

    if (hasBase)
    {
        material->SetBaseTexture(futureBase.get());
    }
    if (hasNormal)
    {
        material->SetNormalMap(futureNormalMap.get());
    }
    if (hasOcclusion)
    {
        material->SetAmbientOcclusion(futureOcclusion.get());
    }
    if (hasProperty)
    {
        material->SetPropertyTexture(futureProperty.get());
    }
    if (hasEmissive)
    {
        material->SetEmissiveTexture(futureEmissive.get());
    }

    Save(material);
}

void MaterialImporter::Load(const char* libraryPath, const std::shared_ptr<MaterialAsset>& material)
{
    
    /* I'm not 100% sure if this is needed 
    if (!ModuleFileSystem::ExistsFile(libraryPath))
    {
        std::string assetPath = App->GetModule<ModuleAssets>()->GetFilePath(material->GetUID());
        LoadFromMeta(assetPath.c_str(), material);
        return;
    }
    */

    char* fileBuffer;
    ModuleFileSystem::LoadFile(libraryPath, fileBuffer);
    char* fileBufferOriginal = fileBuffer;

    // ------------- BINARY ----------------------

    auto resourceModule = App->GetModule<ModuleResources>();

    std::future<std::shared_ptr<TextureAsset>> futureBase;
    std::future<std::shared_ptr<TextureAsset>> futureNormalMap;
    std::future<std::shared_ptr<TextureAsset>> futureOcclusion;
    std::future<std::shared_ptr<TextureAsset>> futureProperty;
    std::future<std::shared_ptr<TextureAsset>> futureEmissive;

    bool hasBase = false;
    bool hasNormal = false;
    bool hasOcclusion = false;
    bool hasProperty = false;
    bool hasEmissive = false;

    unsigned int header[1];
    unsigned int bytes = sizeof(header);
    memcpy(header, fileBuffer, bytes);
    fileBuffer += bytes;

    material->SetName(std::string(fileBuffer, header[0]));
    fileBuffer += header[0];

    bytes = sizeof(UID);
    UID textureUID;
    memcpy(&textureUID, fileBuffer, bytes);
    if (textureUID != 0)
    {
        futureBase = resourceModule->SearchAsset<TextureAsset>(textureUID);
        hasBase = true;
    }
    fileBuffer += bytes;

    memcpy(&textureUID, fileBuffer, bytes);
    if (textureUID != 0)
    {
        futureNormalMap = resourceModule->SearchAsset<TextureAsset>(textureUID);
        hasNormal = true;
    }
    fileBuffer += bytes;

    memcpy(&textureUID, fileBuffer, bytes);
    if (textureUID != 0)
    {
        futureOcclusion = resourceModule->SearchAsset<TextureAsset>(textureUID);
        hasOcclusion = true;
    }
    fileBuffer += bytes;

    memcpy(&textureUID, fileBuffer, bytes);
    if (textureUID != 0)
    {
        futureProperty = resourceModule->SearchAsset<TextureAsset>(textureUID);
        hasProperty = true;
    }
    fileBuffer += bytes;

    memcpy(&textureUID, fileBuffer, bytes);
    if (textureUID != 0)
    {
        futureEmissive = resourceModule->SearchAsset<TextureAsset>(textureUID);
        hasEmissive = true;
    }
    fileBuffer += bytes;

    bytes = sizeof(Color);
    Color color;
    memcpy(&color, &material->GetBaseColor(), bytes);
    material->SetBaseColor(color);
    fileBuffer += bytes;

    memcpy(&color, fileBuffer, bytes);
    material->SetSpecularColor(color);
    fileBuffer += bytes;

    bytes = sizeof(UINT);
    UINT options;
    memcpy(&options, fileBuffer, bytes);
    material->SetOptions(options);

    if (hasBase)
    {
        material->SetBaseTexture(futureBase.get());
    }
    if (hasNormal)
    {
        material->SetNormalMap(futureNormalMap.get());
    }
    if (hasOcclusion)
    {
        material->SetAmbientOcclusion(futureOcclusion.get());
    }
    if (hasProperty)
    {
        material->SetPropertyTexture(futureProperty.get());
    }
    if (hasEmissive)
    {
        material->SetEmissiveTexture(futureEmissive.get());
    }

    delete[] fileBufferOriginal;
}

void MaterialImporter::LoadFromMeta(const char* filePath, const std::shared_ptr<MaterialAsset>& material)
{
    // ------------- META ----------------------

    std::string metaPath = std::string(filePath) + META_EXT;
    rapidjson::Document doc;
    Json meta = Json(doc);
    ModuleFileSystem::LoadJson(metaPath.c_str(), meta);

    Color baseColor = Color(meta["baseColor"]["r"], meta["baseColor"]["g"], meta["baseColor"]["b"], meta["baseColor"]["a"]);
    material->SetBaseColor(baseColor);
    Color specularColor = Color(meta["specularColor"]["r"], meta["specularColor"]["g"], meta["specularColor"]["b"], meta["specularColor"]["a"]);
    material->SetSpecularColor(specularColor);
    UINT options = meta["Options"];
    material->SetOptions(options);

    // ------------- REIMPORT FILE ----------------------

    Import(filePath, material);
}

void MaterialImporter::Save(const std::shared_ptr<MaterialAsset>& material)
{
    // ------------- META ----------------------

    std::string metaPath = App->GetModule<ModuleAssets>()->GetFilePath(material->GetUID()) + META_EXT;

    rapidjson::Document doc;
    Json json = Json(doc);
    ModuleFileSystem::LoadJson(metaPath.c_str(), json);

    json["baseColor"]["r"] = material->GetBaseColor().R();
    json["baseColor"]["g"] = material->GetBaseColor().G();
    json["baseColor"]["b"] = material->GetBaseColor().B();
    json["baseColor"]["a"] = material->GetBaseColor().A();

    json["specularColor"]["r"] = material->GetSpecularColor().R();
    json["specularColor"]["g"] = material->GetSpecularColor().G();
    json["specularColor"]["b"] = material->GetSpecularColor().B();
    json["specularColor"]["a"] = material->GetSpecularColor().A();

    json["options"] = material->GetOptions();

    auto filebuffer = json.ToBuffer();
    ModuleFileSystem::SaveFile(metaPath.c_str(), filebuffer.GetString(), filebuffer.GetSize());

    // ------------- BINARY ----------------------

                //textures' uid     //base/specular color  //options
    UINT size = (sizeof(UID) * 5) + sizeof(Color) * 2 + sizeof(UINT);

    unsigned int header[1] = { static_cast<unsigned int>(material->GetName().size()) };

    size += sizeof(header);
    size += sizeof(char) * static_cast<unsigned int>(material->GetName().size());

    char* fileBuffer = new char[size] {};
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    bytes = sizeof(char) * static_cast<unsigned int>(material->GetName().size());
    memcpy(cursor, &material->GetName()[0], bytes);
    cursor += bytes;

    bytes = sizeof(UID);
    UID textureUID = material->GetBaseTexture() ? material->GetBaseTexture()->GetUID() : 0;
    memcpy(cursor, &textureUID, bytes);
    cursor += bytes;

    textureUID = material->GetNormalMap() ? material->GetNormalMap()->GetUID() : 0;
    memcpy(cursor, &textureUID, bytes);
    cursor += bytes;

    textureUID = material->GetAmbientOcclusion() ? material->GetAmbientOcclusion()->GetUID() : 0;
    memcpy(cursor, &textureUID, bytes);
    cursor += bytes;

    textureUID = material->GetPropertyTexture() ? material->GetPropertyTexture()->GetUID() : 0;
    memcpy(cursor, &textureUID, bytes);
    cursor += bytes;

    textureUID = material->GetEmissiveTexture() ? material->GetEmissiveTexture()->GetUID() : 0;
    memcpy(cursor, &textureUID, bytes);
    cursor += bytes;

    bytes = sizeof(Color);
    memcpy(cursor, &material->GetBaseColor(), bytes);
    cursor += bytes;

    memcpy(cursor, &material->GetSpecularColor(), bytes);
    cursor += bytes;

    bytes = sizeof(UINT);
    UINT options = material->GetOptions();
    memcpy(cursor, &options, bytes);
    
    std::string libPath = MATERIALS_LIB_PATH + std::to_string(material->GetUID()) + BINARY_EXT;
    ModuleFileSystem::SaveFile(libPath.c_str(), fileBuffer, size);

    delete[] fileBuffer;
}