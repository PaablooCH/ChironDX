#pragma once
#include "Module.h"

#include "DataModels/FileSystem/UID/UID.h"
#include "Enums/AssetType.h"

#include "ThreadPool/ThreadPool.h"

class Asset;
class MaterialImporter;
class MeshImporter;
class ModelImporter;
class TextureImporter;

class ModuleResources : public Module
{
public:
    ModuleResources();
    ~ModuleResources() override;

    bool Init() override;
    bool Start() override;
    bool CleanUp() override;

    // Request resource and Import if is necessary
    template<class A = Asset>
    std::future<std::shared_ptr<A>> RequestAsset(const std::string path);

    // Search resource by UID
    template<class A = Asset>
    std::future<std::shared_ptr<A>> SearchAsset(UID uid);

private:
    void ImportAsset(const std::shared_ptr<Asset>& asset);
    void LoadAsset(const std::shared_ptr<Asset>& asset);

    std::shared_ptr<Asset> LoadBinary(UID uid);

    // ------------- CREATORS ----------------------

    std::shared_ptr<Asset> CreateNewAsset(const std::string& assetPath, AssetType type);
    std::shared_ptr<Asset> CreateAssetOfType(AssetType type, UID uid);

    // ------------- GETTERS ----------------------

    std::string GetLibraryPath(UID uid, AssetType type);
    std::string GetLibraryPathByType(AssetType type);
    AssetType GetAssetTypeByExtension(const std::string& path);

    void CreateLibraryFolder();

    UID GetFileUID(const std::string& filePath) const;
    std::shared_ptr<Asset> CheckAndLoadAsset(const std::string& path, UID uid, AssetType type);
    bool ExistsFile(UID uid);

private:
    std::map<UID, std::weak_ptr<Asset>> _assets;

    std::unique_ptr<TextureImporter> _textureImporter;
    std::unique_ptr<MaterialImporter> _materialImporter;
    std::unique_ptr<MeshImporter> _meshImporter;
    std::unique_ptr<ModelImporter> _modelImporter;

    std::unique_ptr<ThreadPool> _threadPool;
};

template<class A>
inline std::future<std::shared_ptr<A>> ModuleResources::RequestAsset(const std::string path)
{
    std::shared_ptr<std::promise<std::shared_ptr<A>>> promise = std::make_shared<std::promise<std::shared_ptr<A>>>();
    auto future = promise->get_future();
    _threadPool->AddTask(
        [this, path, promise]() 
        {
            try 
            {
                std::shared_ptr<Asset> shared;
                AssetType type = GetAssetTypeByExtension(path);
                if (type == AssetType::UNKNOWN)
                {
                    LOG_ERROR("Extension not supported.");
                    promise->set_value(nullptr);
                    return;
                }

                UID uid = GetFileUID(path);

                if (uid == 0) 
                {
                    LOG_INFO("No meta found for '{}'. Reimporting...", path);
                    shared = CreateNewAsset(path, type);
                    ImportAsset(shared);
                    promise->set_value(std::dynamic_pointer_cast<A>(shared));
                    return;
                }

                auto it = _assets.find(uid);
                if (it != _assets.end() && !(it->second).expired())
                {
                    shared = (it->second).lock();
                    promise->set_value(std::dynamic_pointer_cast<A>(shared));
                    return;
                }

                shared = CheckAndLoadAsset(path, uid, type);
                promise->set_value(std::dynamic_pointer_cast<A>(shared));
            }
            catch (std::future_error const& e)
            {
                LOG_ERROR("Error during asset request Future error: {}", e.what());
            }
            catch (std::exception const& e)
            {
                LOG_ERROR("Error during asset request Standard exception: {}", e.what());
                promise->set_exception(std::current_exception());
            }
            catch (...)
            {
                LOG_ERROR("Error during asset request Unknown exception");
            }
        });
    return future;
}

template<class A>
inline std::future<std::shared_ptr<A>> ModuleResources::SearchAsset(UID uid)
{
    std::shared_ptr<std::promise<std::shared_ptr<A>>> promise = std::make_shared<std::promise<std::shared_ptr<A>>>();
    auto future = promise->get_future();
    _threadPool->AddTask(
        [this, uid, promise]() 
        {
            try 
            {
                std::shared_ptr<Asset> shared;
                auto it = _assets.find(uid);
                if (it != _assets.end() && !(it->second).expired())
                {
                    shared = (it->second).lock();
                    promise->set_value(std::dynamic_pointer_cast<A>(shared));
                    return;
                }

                if (ExistsFile(uid))
                {
                    shared = LoadBinary(uid);
                    promise->set_value(std::dynamic_pointer_cast<A>(shared));
                    return;
                }
                LOG_WARNING("Couldn't find or load {} file.", uid);
                promise->set_value(nullptr);
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Error during asset request: {}", e.what());
                promise->set_exception(std::current_exception());
            }
        });
    return future;
}