#include "model.hpp"

#include <iostream>

Model::Model() {}

void Model::load(std::string path)
{
    Assimp::Importer import;

    const aiScene *scene = import.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    directory = "";

    processNode(scene->mRootNode, scene);
}

void Model::draw(Shader shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].draw(shader);
    }
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0])
        {
            vertex.texcoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.texcoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "iAlbedo");
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "iNormal");
        std::vector<Texture> metallicMaps = loadMaterialTextures(material, aiTextureType_LIGHTMAP, "iMetallic");

        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::cout << "aiTextureType_NONE: " << material->GetTextureCount(aiTextureType_NONE) << std::endl;
        std::cout << "aiTextureType_DIFFUSE: " << material->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
        std::cout << "aiTextureType_SPECULAR: " << material->GetTextureCount(aiTextureType_SPECULAR) << std::endl;
        std::cout << "aiTextureType_AMBIENT: " << material->GetTextureCount(aiTextureType_AMBIENT) << std::endl;
        std::cout << "aiTextureType_EMISSIVE: " << material->GetTextureCount(aiTextureType_EMISSIVE) << std::endl;
        std::cout << "aiTextureType_HEIGHT: " << material->GetTextureCount(aiTextureType_HEIGHT) << std::endl;
        std::cout << "aiTextureType_NORMALS: " << material->GetTextureCount(aiTextureType_NORMALS) << std::endl;
        std::cout << "aiTextureType_SHININESS: " << material->GetTextureCount(aiTextureType_SHININESS) << std::endl;
        std::cout << "aiTextureType_OPACITY: " << material->GetTextureCount(aiTextureType_OPACITY) << std::endl;
        std::cout << "aiTextureType_DISPLACEMENT: " << material->GetTextureCount(aiTextureType_DISPLACEMENT) << std::endl;
        std::cout << "aiTextureType_LIGHTMAP: " << material->GetTextureCount(aiTextureType_LIGHTMAP) << std::endl;
        std::cout << "aiTextureType_REFLECTION: " << material->GetTextureCount(aiTextureType_REFLECTION) << std::endl;
        std::cout << "aiTextureType_UNKNOWN: " << material->GetTextureCount(aiTextureType_UNKNOWN) << std::endl;
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        bool skip = false;

        mat->GetTexture(type, i, &str);

        for (unsigned int j = 0; j < texturesLoaded.size(); j++)
        {
            if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(texturesLoaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            Texture texture;
            std::string path = directory + str.C_Str();

            texture.id = texture.load(path);

            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }

    return textures;
}