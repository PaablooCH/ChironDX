#include "Pch.h"
#include "ModelImporter.h"

#include "Application.h"

#include "Modules/ModuleAssets.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleFileSystem.h"
#include "Modules/ModuleResources.h"

#include "DataModels/Assets/MaterialAsset.h"
#include "DataModels/Assets/MeshAsset.h"
#include "DataModels/Assets/ModelAsset.h"

#include "DataModels/FileSystem/FileSystemEntry/Folder/Folder.h"
#include "DataModels/FileSystem/Json/Json.h"

#include "DataModels/UI/Windows/EditorWindow/FileBrowserWindow.h"

#include "Defines/FileSystemDefine.h"

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#ifdef PROFILE
    #include "Optick/optick.h"
#endif // OPTICK

ModelImporter::ModelImporter()
{
}

ModelImporter::~ModelImporter()
{
}

void ModelImporter::Import(const char* filePath, const std::shared_ptr<ModelAsset>& model)
{
    LOG_INFO("Import Model from {}", filePath);

    const aiScene* scene =
        aiImportFile(filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
    if (scene)
    {
#ifdef PROFILE
        OPTICK_THREAD("ImportThread");
        OPTICK_CATEGORY("ImportModel", Optick::Category::Debug);
#endif // DEBUG
        model->SetName(ModuleFileSystem::GetFileName(filePath));
        ImportNode(scene, filePath, model, scene->mRootNode, -1, Matrix::Identity);

        Save(model);

        aiReleaseImport(scene);
    }
    else
    {
        LOG_ERROR("Error loading {}: {}", filePath, aiGetErrorString());
    }
}

void ModelImporter::Load(const char* libraryPath, const std::shared_ptr<ModelAsset>& model)
{
#ifdef PROFILE
    OPTICK_THREAD("LoadThread");
    OPTICK_CATEGORY("Load Model", Optick::Category::Debug);
#endif // OPTICK
    std::vector<std::unique_ptr<Node>> nodes;

    /* I'm not 100% sure if this is needed
    if (!ModuleFileSystem::ExistsFile(libraryPath))
    {
        LoadFromMeta(App->GetModule<ModuleAssets>()->GetFilePath(model->GetUID()).c_str(), model);
        return;
    }
    */

    char* fileBuffer;
    ModuleFileSystem::LoadFile(libraryPath, fileBuffer);
    char* oringinalBuffer = fileBuffer;

    // ------------- BINARY ----------------------

    unsigned int header[2];
    unsigned int bytes = sizeof(header);
    memcpy(header, fileBuffer, bytes);
    fileBuffer += bytes;

    model->SetName(std::string(fileBuffer, header[0]));
    fileBuffer += header[0];

    nodes.reserve(header[1]);

    for (unsigned int i = 0; i < header[1]; ++i)
    {
        std::unique_ptr<Node> node = std::make_unique<Node>();

        unsigned int nodeHeader[2];
        bytes = sizeof(nodeHeader);
        memcpy(nodeHeader, fileBuffer, bytes);
        fileBuffer += bytes;

        node->name = std::string(fileBuffer, nodeHeader[0]);
        fileBuffer += nodeHeader[0];

        memcpy(&node->transform, fileBuffer, sizeof(Matrix));
        fileBuffer += sizeof(Matrix);

        memcpy(&node->parent, fileBuffer, sizeof(int));
        fileBuffer += sizeof(int);

        node->meshMaterial.reserve(nodeHeader[1]);

        // NOT ENGINE
        std::vector<UID> meshesUIDs(nodeHeader[1]);
        memcpy(meshesUIDs.data(), fileBuffer, sizeof(UID) * nodeHeader[1]);
        fileBuffer += sizeof(UID) * nodeHeader[1];

        std::vector<UID> materialsUIDs(nodeHeader[1]);
        memcpy(materialsUIDs.data(), fileBuffer, sizeof(UID) * nodeHeader[1]);
        fileBuffer += sizeof(UID) * nodeHeader[1];

        auto moduleResource = App->GetModule<ModuleResources>();
        for (int i = 0; i < meshesUIDs.size(); ++i)
        {
            auto futureMesh = moduleResource->SearchAsset<MeshAsset>(meshesUIDs[i]);
            auto futureMat = moduleResource->SearchAsset<MaterialAsset>(materialsUIDs[i]);

            node->meshMaterial.emplace_back(futureMesh.get(), futureMat.get());
        }

        nodes.push_back(std::move(node));
    }
    model->SetNodes(nodes);

    delete[] oringinalBuffer;
}

void ModelImporter::LoadFromMeta(const char* filePath, const std::shared_ptr<ModelAsset>& model)
{
    std::string metaPath = std::string(filePath) + META_EXT;

    // ------------- LOAD META ----------------------

    rapidjson::Document doc;
    Json meta = Json(doc);
    ModuleFileSystem::LoadJson(metaPath.c_str(), meta);

    int nodesCount = meta["nodeSize"];
    
    if (nodesCount == 0)
    {
        Import(filePath, model);
        return;
    }

    model->SetName(ModuleFileSystem::GetFileName(filePath));
    std::vector<std::unique_ptr<Node>> nodes;
    nodes.reserve(nodesCount);
    auto metaNodes = meta["nodes"];

    for (int i = 0; i < nodesCount; i++)
    {
        std::unique_ptr<Node> node = std::make_unique<Node>();

        node->name = metaNodes[i]["name"];

        Matrix transform;
        transform._11 = metaNodes[i]["transform"]["11"];
        transform._12 = metaNodes[i]["transform"]["12"];
        transform._13 = metaNodes[i]["transform"]["13"];
        transform._14 = metaNodes[i]["transform"]["14"];
        transform._21 = metaNodes[i]["transform"]["21"];
        transform._22 = metaNodes[i]["transform"]["22"];
        transform._23 = metaNodes[i]["transform"]["23"];
        transform._24 = metaNodes[i]["transform"]["24"];
        transform._31 = metaNodes[i]["transform"]["31"];
        transform._32 = metaNodes[i]["transform"]["32"];
        transform._33 = metaNodes[i]["transform"]["33"];
        transform._34 = metaNodes[i]["transform"]["34"];
        transform._41 = metaNodes[i]["transform"]["41"];
        transform._42 = metaNodes[i]["transform"]["42"];
        transform._43 = metaNodes[i]["transform"]["43"];
        transform._44 = metaNodes[i]["transform"]["44"];

        node->transform = transform;

        node->parent = metaNodes[i]["parent"];

        int meshMaterialSize = metaNodes[i]["meshMaterialSize"];
        node->meshMaterial.reserve(meshMaterialSize);

        auto moduleResource = App->GetModule<ModuleResources>();
        for (int j = 0; j < meshMaterialSize; ++j)
        {
            auto futureMesh = moduleResource->SearchAsset<MeshAsset>(metaNodes[i]["meshesUIDs"][j]);
            auto futureMat = moduleResource->SearchAsset<MaterialAsset>(metaNodes[i]["materialsUIDs"][j]);

            auto mesh = futureMesh.get();
            auto mat = futureMat.get();
            if (mesh == nullptr || mat == nullptr)
            {
                model->ClearNodes();
                Import(filePath, model);
                return;
            }
            node->meshMaterial.emplace_back(mesh, mat);
        }
        nodes.push_back(std::move(node));
    }
    model->SetNodes(nodes);
}

void ModelImporter::Save(const std::shared_ptr<ModelAsset>& model)
{
    // ------------- META ----------------------

    std::string metaPath = App->GetModule<ModuleAssets>()->GetFilePath(model->GetUID()) + META_EXT;
    rapidjson::Document doc;
    Json meta = Json(doc);
    ModuleFileSystem::LoadJson(metaPath.c_str(), meta);
    meta["nodeSize"] = model->GetNodes().size();
    auto nodes = meta["nodes"];

    // ------------- BINARY ----------------------

                        //transform         //parent    //name and vector lenght header
    unsigned int size = (sizeof(Matrix) + sizeof(int) + (sizeof(unsigned int) * 2)) * static_cast<unsigned int>(model->GetNodes().size());

    for (auto& node : model->GetNodes())
    {
        size += sizeof(UID) * 2 * static_cast<unsigned int>(node->meshMaterial.size());
        size += sizeof(char) * static_cast<unsigned int>(node->name.size());
    }

    unsigned int header[2] = { static_cast<unsigned int>(model->GetName().size()), static_cast<unsigned int>(model->GetNodes().size()) };
    size += sizeof(header);
    size += sizeof(char) * static_cast<unsigned int>(model->GetName().size());

    char* fileBuffer = new char[size] {};
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    bytes = sizeof(char) * static_cast<unsigned int>(model->GetName().size());
    memcpy(cursor, &model->GetName()[0], bytes);
    cursor += bytes;

    for (int i = 0; i < model->GetNodes().size(); i++)
    {
        auto& node = model->GetNodes()[i];

        // ------------- META ----------------------

        nodes[i]["name"] = node->name;
        nodes[i]["parent"] = node->parent;

        nodes[i]["transform"]["11"] = node->transform._11;
        nodes[i]["transform"]["12"] = node->transform._12;
        nodes[i]["transform"]["13"] = node->transform._13;
        nodes[i]["transform"]["14"] = node->transform._14;
        nodes[i]["transform"]["21"] = node->transform._21;
        nodes[i]["transform"]["22"] = node->transform._22;
        nodes[i]["transform"]["23"] = node->transform._23;
        nodes[i]["transform"]["24"] = node->transform._24;
        nodes[i]["transform"]["31"] = node->transform._31;
        nodes[i]["transform"]["32"] = node->transform._32;
        nodes[i]["transform"]["33"] = node->transform._33;
        nodes[i]["transform"]["34"] = node->transform._34;
        nodes[i]["transform"]["41"] = node->transform._41;
        nodes[i]["transform"]["42"] = node->transform._42;
        nodes[i]["transform"]["43"] = node->transform._43;
        nodes[i]["transform"]["44"] = node->transform._44;
        
        nodes[i]["meshMaterialSize"] = node->meshMaterial.size();
        auto meshes = nodes[i]["meshesUIDs"];
        auto mat = nodes[i]["materialsUIDs"];

        // ------------- BINARY ----------------------

        unsigned int nodeHeader[2] = { static_cast<unsigned int>(node->name.size()),
                                       static_cast<unsigned int>(node->meshMaterial.size()) };

        bytes = sizeof(nodeHeader);
        memcpy(cursor, nodeHeader, bytes);
        cursor += bytes;

        bytes = sizeof(char) * static_cast<unsigned int>(node->name.size());
        memcpy(cursor, &(node->name[0]), bytes);
        cursor += bytes;

        bytes = sizeof(Matrix);
        memcpy(cursor, &(node->transform), bytes);
        cursor += bytes;

        bytes = sizeof(int);
        memcpy(cursor, &(node->parent), bytes);
        cursor += bytes;

        unsigned int countMeshes = 0;
        unsigned int countMat = 0;
        for (int j = 0; j < node->meshMaterial.size(); ++j)
        {
            UID meshUID = node->meshMaterial[j].first->GetUID();
            
            // ------------- META ----------------------

            meshes[countMeshes] = meshUID;
            ++countMeshes;

            // ------------- BINARY ----------------------

            memcpy(cursor, &meshUID, sizeof(UID));
            cursor += sizeof(UID);
        }

        for (int j = 0; j < node->meshMaterial.size(); ++j)
        {
            UID materialUID = node->meshMaterial[j].second->GetUID();

            // ------------- META ----------------------

            mat[countMat] = materialUID;
            ++countMat;

            // ------------- BINARY ----------------------

            memcpy(cursor, &materialUID, sizeof(UID));
            cursor += sizeof(UID);
        }
    }

    // ------------- META ----------------------

    rapidjson::StringBuffer buffer = meta.ToBuffer();
    ModuleFileSystem::SaveFile(metaPath.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());

    // ------------- BINARY ----------------------

    std::string libPath = MODELS_LIB_PATH + std::to_string(model->GetUID()) + BINARY_EXT;
    ModuleFileSystem::SaveFile(libPath.c_str(), fileBuffer, size);

    delete[] fileBuffer;
}

void ModelImporter::ImportNode(const aiScene* scene, const char* filePath, const std::shared_ptr<ModelAsset>& model, const aiNode* node,
    int parentIdx, const Matrix& accTransform)
{
    std::string name = node->mName.C_Str();
    Matrix transform = (*(Matrix*)&node->mTransformation);

    if (name.find("$AssimpFbx$") != std::string::npos || name.find("RootNode") != std::string::npos)
    {
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            const Matrix& newAcctransform = accTransform * transform;

            ImportNode(scene, filePath, model, node->mChildren[i], parentIdx, newAcctransform);
        }
    }
    else
    {
        Node* modelNode = new Node();
        modelNode->name = name;
        modelNode->parent = parentIdx;
        modelNode->transform = transform * accTransform;

        LOG_INFO("Node name: {}", name);
        if (node->mParent)
        {
            LOG_INFO("Parent node name: {}", node->mParent->mName.C_Str());
        }

        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            LOG_INFO("Importing mesh {}", name);
            LOG_INFO("Importing material {}", material->GetName().C_Str());

            auto futureMeshAsset = ImportMesh(mesh, name, i);
            auto futureMaterialAsset = ImportMaterial(material, filePath, i);

            std::pair<std::shared_ptr<MeshAsset>, std::shared_ptr<MaterialAsset>> meshMat =
                std::make_pair(futureMeshAsset.get(), futureMaterialAsset.get());
            modelNode->meshMaterial.push_back(meshMat);
        }

        model->AddNode(modelNode);
        int newParentId = static_cast<int>(model->GetNodes().size()) - 1;

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            ImportNode(scene, filePath, model, node->mChildren[i], newParentId, Matrix::Identity);
        }
    }
}

std::future<std::shared_ptr<MeshAsset>> ModelImporter::ImportMesh(const aiMesh* mesh, const std::string& fileName, int iteration)
{
    auto fileBrowser = static_cast<FileBrowserWindow*>(App->GetModule<ModuleEditor>()->GetWindow(WindowsType::FILE_BROWSER));
    
    auto currentFolder = fileBrowser->GetSelectedFolder();
    std::string meshPath = currentFolder->GetPath() + '/' + fileName + "_" + std::to_string(iteration) + MESH_EXT;

    if (ModuleFileSystem::ExistsFile(meshPath.c_str()))
    {
        return App->GetModule<ModuleResources>()->RequestAsset<MeshAsset>(meshPath);
    }

    // -------------- VERTEX ---------------------

    std::vector<Vertex> triangleVertices;
    triangleVertices.reserve(mesh->mNumVertices);
    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex newVertex;

        newVertex.vertices = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->HasTextureCoords(0))
        {
            newVertex.texCoords = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }

        if (mesh->HasNormals())
        {
            newVertex.normals = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        if (mesh->HasTangentsAndBitangents())
        {
            newVertex.tangents = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            newVertex.biTangents = Vector3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
        triangleVertices.push_back(newVertex);
    }
    const UINT vertexBufferSize = static_cast<UINT>(triangleVertices.size() * sizeof(Vertex));

    // -------------- INDEX ---------------------

    UINT numIndices = mesh->mNumFaces * 3;

    std::vector<UINT> indexBufferData;
    indexBufferData.reserve(numIndices);
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        indexBufferData.push_back(mesh->mFaces[i].mIndices[0]);
        indexBufferData.push_back(mesh->mFaces[i].mIndices[1]);
        indexBufferData.push_back(mesh->mFaces[i].mIndices[2]);
    }
    const UINT indexBufferSize = static_cast<UINT>(indexBufferData.size() * sizeof(UINT));

    // ------------- SAVE MESH FILE ----------------------

                        //Vertex                                //Indices
    unsigned int size = (sizeof(Vertex) * mesh->mNumVertices) + (sizeof(UINT) * (mesh->mNumFaces * 3));

    unsigned int header[2] = { static_cast<unsigned int>(mesh->mNumVertices), mesh->mNumFaces * 3 };
    size += sizeof(header);

    char* fileBuffer = new char[size] {};
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    bytes = sizeof(Vertex) * mesh->mNumVertices;
    memcpy(cursor, triangleVertices.data(), bytes);
    cursor += bytes;

    bytes = sizeof(UINT) * (mesh->mNumFaces * 3);
    memcpy(cursor, indexBufferData.data(), bytes);
    cursor += bytes;

    App->GetModule<ModuleAssets>()->SaveEngineFile(meshPath, fileBuffer, size, currentFolder);

    delete[] fileBuffer;

    return App->GetModule<ModuleResources>()->RequestAsset<MeshAsset>(meshPath);
}

std::future<std::shared_ptr<MaterialAsset>> ModelImporter::ImportMaterial(const aiMaterial* material, const std::string& filePath, int iteration)
{
    auto resources = App->GetModule<ModuleResources>();

    auto fileBrowser = static_cast<FileBrowserWindow*>(App->GetModule<ModuleEditor>()->GetWindow(WindowsType::FILE_BROWSER));
    auto currentFolder = fileBrowser->GetSelectedFolder();

    std::string matPath = currentFolder->GetPath() + '/' + ModuleFileSystem::GetFileName(filePath) + "_" +
        std::to_string(iteration) + MAT_EXT;

    if (ModuleFileSystem::ExistsFile(matPath.c_str()))
    {
        return App->GetModule<ModuleResources>()->RequestAsset<MaterialAsset>(matPath);
    }

    aiString file;

    rapidjson::Document doc;
    Json json = Json(doc);

    json["baseColor"]["r"] = 1.f;
    json["baseColor"]["g"] = 1.f;
    json["baseColor"]["b"] = 1.f;
    json["baseColor"]["a"] = 1.f;

    json["specularColor"]["r"] = 0.5f;
    json["specularColor"]["g"] = 0.30000001192092896f;
    json["specularColor"]["b"] = 0.5f;
    json["specularColor"]["a"] = 1.f;

    json["baseTextureUID"] = 0;
    json["normalMapUID"] = 0;
    json["ambientOcclusionUID"] = 0;
    json["propertyTextureUID"] = 0;
    json["emissiveTextureUID"] = 0;

    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
    {
        UID baseTextureUID = 0;
        CheckPathMaterial(filePath.c_str(), file, baseTextureUID);
        json["baseTextureUID"] = baseTextureUID;
    }

    if (material->GetTexture(aiTextureType_NORMALS, 0, &file) == AI_SUCCESS)
    {
        UID normalMapUID = 0;
        CheckPathMaterial(filePath.c_str(), file, normalMapUID);
        json["normalMapUID"] = normalMapUID;
    }

    if (material->GetTexture(aiTextureType_LIGHTMAP, 0, &file) == AI_SUCCESS)
    {
        UID ambientOcclusionUID = 0;
        CheckPathMaterial(filePath.c_str(), file, ambientOcclusionUID);
        json["ambientOcclusionUID"] = ambientOcclusionUID;
    }

    if (material->GetTexture(aiTextureType_METALNESS, 0, &file) == AI_SUCCESS)
    {
        UID propertyTextureUID = 0;
        CheckPathMaterial(filePath.c_str(), file, propertyTextureUID);
        json["propertyTextureUID"] = propertyTextureUID;
    }

    if (material->GetTexture(aiTextureType_EMISSIVE, 0, &file) == AI_SUCCESS)
    {
        UID emissiveTextureUID = 0;
        CheckPathMaterial(filePath.c_str(), file, emissiveTextureUID);
        json["emissiveTextureUID"] = emissiveTextureUID;
    }

    // ------------- SAVE MATERIAL FILE ----------------------

    auto fileBuffer = json.ToBuffer();

    App->GetModule<ModuleAssets>()->SaveEngineFile(matPath, fileBuffer.GetString(), fileBuffer.GetSize(), currentFolder);

    return App->GetModule<ModuleResources>()->RequestAsset<MaterialAsset>(matPath);
}

void ModelImporter::CheckPathMaterial(const char* filePath, const aiString& file, UID& textureUID)
{
    std::string metaPath;
    // No exists in its file
    if (!ModuleFileSystem::ExistsFile(file.data))
    {
        std::string name = ModuleFileSystem::GetFile(file.data);

        std::string modelPath = ModuleFileSystem::GetPathWithoutFile(filePath);
        // No exists in its model path
        if (!ModuleFileSystem::ExistsFile((modelPath + name).c_str()))
        {
            LOG_INFO("Texture not found!!!");
            return;
        }
        else
        {
            // Exists in its model path
            textureUID = App->GetModule<ModuleAssets>()->CreateMetaFileC(modelPath + name);
        }
    }
    else
    {
        // Exists in its file
        textureUID = App->GetModule<ModuleAssets>()->CopyAndSaveFile(file.data);
    }
}