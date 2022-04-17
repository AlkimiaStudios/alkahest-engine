#include "modelimporter.h"
#include "../../sys/log/log.h"
#include "../../sys/render/buffer.h"
#include "../../sys/render/texture.h"
#include "../../sys/render/mesh.h"
#include "../../sys/window/window.h"

namespace Alkahest
{
    Assimp::Importer ModelImporter::m_importer;
    unsigned int ModelImporter::m_postProcessFlags = aiProcessPreset_TargetRealtime_MaxQuality;

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
        return processScene(scene);
    }

    Ref<Model> ModelImporter::processScene(const aiScene* scene)
    {
        std::vector<Ref<Mesh>> meshes;

        for (unsigned int i = 0 ; i < scene->mNumMeshes ; i++) {
            const aiMesh* paiMesh = scene->mMeshes[i];
            meshes.push_back(initMesh(paiMesh));
        }

        //TODO: grab texture data
        std::vector<Material> materials;

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
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        return Mesh::create(vertices, indices, {}, mesh->mMaterialIndex);
    }
}