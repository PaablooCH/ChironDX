#pragma once

template<typename U>
class Importer
{
public:
    /// <summary>
    /// Imports an asset from the specified file path.
    /// </summary>
    /// <param name="filePath">The path to the file to import.</param>
    /// <param name="asset">A shared pointer to the asset object to be populated with imported data.</param>
    virtual void Import(const char* filePath, const std::shared_ptr<U>& asset) = 0;

    /// <summary>
    /// Loads an asset from the specified library path.
    /// </summary>
    /// <param name="libraryPath">A pointer to a null-terminated string specifying the path to the library.</param>
    /// <param name="asset">A shared pointer to the asset to be loaded.</param>
    virtual void Load(const char* libraryPath, const std::shared_ptr<U>& asset) = 0;

    /// <summary>
    /// Loads asset data from a metadata file.
    /// </summary>
    /// <param name="filePath">The path to the metadata file to load from.</param>
    /// <param name="asset">A shared pointer to the asset object to be loaded or updated.</param>
    virtual void LoadFromMeta(const char* filePath, const std::shared_ptr<U>& asset) = 0;
protected:
    Importer() = default;
    virtual ~Importer() {}

    /// <summary>
    /// Saves the specified asset.
    /// </summary>
    /// <param name="asset">A shared pointer to the asset to be saved.</param>
    virtual void Save(const std::shared_ptr<U>& asset) = 0;
};
