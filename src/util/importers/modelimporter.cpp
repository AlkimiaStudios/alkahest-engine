#include "modelimporter.h"
#include "../../sys/log/log.h"
#include "../../sys/render/buffer.h"
#include "../../renderer/texture.h"
#include "../../sys/render/mesh.h"
#include "../../sys/window/window.h"

#include <GL/gl3w.h>

namespace Alkahest
{
    Assimp::Importer ModelImporter::m_importer;
    unsigned int ModelImporter::m_postProcessFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs;

    Ref<Model> ModelImporter::import(const std::string& pathToFile)
    {
        // Read the model file to generate a scene for processing data
        const aiScene* scene = m_importer.ReadFile(pathToFile, m_postProcessFlags);

        // Report the exception if the import failed
        if (!scene)
        {
            logError("Error importing model file! Path: {}, Importer Error: {}", pathToFile, m_importer.GetErrorString());
            ImportError e = { pathToFile, m_importer.GetErrorString() };
            throw e;
        }

        // Process the file's contents
        return processScene(scene, pathToFile);
    }

    Ref<Model> ModelImporter::processScene(const aiScene* scene, const std::string& pathToFile)
    {
        std::vector<Ref<Mesh>> meshes;

        for (unsigned int i = 0 ; i < scene->mNumMeshes ; i++) {
            const aiMesh* paiMesh = scene->mMeshes[i];
            meshes.push_back(initMesh(paiMesh));
        }

        std::vector<Material> materials = initMaterials(scene, pathToFile);

        Ref<Model> m = Model::create(meshes, materials);
        // Register model with window so it can be drawn
        IWindow::create()->registerRenderable(m);
        return m;
    }

    Ref<Mesh> ModelImporter::initMesh(const aiMesh* mesh)
    {
        const aiVector3D zeroVector(0.0f, 0.0f, 0.0f);

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Populate the vertex attribute vectors
        for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) {
            const aiVector3D& pos = mesh->mVertices[i];

            glm::vec3 norm;
            if (mesh->mNormals) {
                const aiVector3D& normal = mesh->mNormals[i];
                norm = glm::vec3(normal.x, normal.y, normal.z);
            } else {
                norm = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            const aiVector3D& texCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zeroVector;

            Vertex v = {{pos.x, pos.y, pos.z}, {texCoord.x, texCoord.y}, norm};

            vertices.push_back(v);
        }

        // Populate the index buffer
        for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return Mesh::create(vertices, indices, mesh->mMaterialIndex);
    }

    std::string dirFromFilePath(const std::string& pathToFile)
    {
        // Extract the directory part from the file name
        std::string::size_type slash;

        slash = pathToFile.find_last_of("/");

        std::string dir;

        if (slash == std::string::npos) {
            dir = ".";
        }
        else if (slash == 0) {
            dir = "/";
        }
        else {
            dir = pathToFile.substr(0, slash);
        }

        return dir;
    }

    std::vector<Material> ModelImporter::initMaterials(const aiScene* scene, const std::string& pathToFile)
    {
        std::vector<Material> mats;
        std::string dir = dirFromFilePath(pathToFile);

        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            const aiMaterial* mat = scene->mMaterials[i];

            Ref<Texture2D> diffuseTexture = loadTexture(aiTextureType_DIFFUSE, mat, dir, i);
            Ref<Texture2D> specularTexture = loadTexture(aiTextureType_SHININESS, mat, dir, i);

            glm::vec3 ambientColor = loadColorByKey("ambient", mat);
            glm::vec3 diffuseColor = loadColorByKey("diffuse", mat);
            glm::vec3 specularColor = loadColorByKey("specular", mat);

            Material newMat;
            newMat.DiffuseTexture = diffuseTexture;
            newMat.SpecularTexture = specularTexture;
            newMat.AmbientColor = ambientColor;
            newMat.DiffuseColor = diffuseColor;
            newMat.SpecularColor = specularColor;

            mats.push_back(newMat);
        }

        return mats;
    }

    Ref<Texture2D> ModelImporter::loadTexture(aiTextureType type, const aiMaterial* mat, std::string dir, unsigned int index)
    {
        Ref<Texture2D> t = nullptr;

        TextureType internalType;
        switch (type)
        {
        case aiTextureType_DIFFUSE:
            internalType = TextureType::TextureTypeDiffuse;
            break;
        
        case aiTextureType_SHININESS:
            internalType = TextureType::TextureTypeSpecular;
            break;
        
        default:
            internalType = TextureType::TextureTypeUnknown;
            break;
        }

        if (mat->GetTextureCount(type) > 0)
        {
            aiString path;

            if (mat->GetTexture(type, 0, &path, 0, 0, 0, 0, 0) == AI_SUCCESS)
            {
                std::string p(path.data);

                // path cleanup
                for (int i = 0 ; i < p.length() ; i++) {
                    if (p[i] == '\\') {
                        p[i] = '/';
                    }
                }

                if (p.substr(0, 2) == ".\\") {
                    p = p.substr(2, p.size() - 2);
                }

                std::string fullPath = dir + "/" + p;

                t = Texture2D::create();
                t->load(fullPath.c_str());
                t->setType(internalType);
            }
        }

        return t;
    }

    glm::vec3 ModelImporter::loadColorByKey(const char* key, const aiMaterial* mat)
    {
        aiColor3D color(0.0f, 0.0f, 0.0f);
        glm::vec3 rv = {0.0f, 0.0f, 0.0f};

        aiReturn result;

        if (key == "ambient")
        {
            result = mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
        }
        else if (key == "diffuse")
        {
            result = mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        }
        else if (key == "specular")
        {
            result = mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
        }
        else
        {
            return {0.0f, 0.0f, 0.0f};
        }

        if (result == AI_SUCCESS)
        {
            rv = glm::vec3(color.r, color.g, color.b);
        }

        return rv;
    }
}