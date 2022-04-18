#pragma once

#include "../../macros.h"
#include "../../renderer/model.h"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // output data structures
#include <assimp/postprocess.h> // post-processing flags

#include <glm/glm.hpp>

namespace Alkahest
{
    // Forward declare model class
    class Model;

    class API ModelImporter
    {
    public:
        static Ref<Model> import(const std::string& pathToFile);
    private:
        static Ref<Model> processScene(const aiScene* scene, const std::string& pathToFile);
        static Ref<Mesh> initMesh(const aiMesh* mesh);
        static std::vector<Material> initMaterials(const aiScene* scene, const std::string& pathToFile);
        static Ref<Texture> loadTexture(aiTextureType type, const aiMaterial* mat, std::string dir, unsigned int index);
        static glm::vec3 loadColorByKey(const char* key, const aiMaterial* mat);
    private:
        static Assimp::Importer m_importer;
        static unsigned int m_postProcessFlags;
    };
}