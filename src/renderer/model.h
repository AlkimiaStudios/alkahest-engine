#pragma once

#include "../macros.h"
#include "common.h"
#include "transform.h"
#include "../sys/render/mesh.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    class API Model : public IRenderable
    {
    public:
        Model() {};
        virtual ~Model() {};

        virtual void draw() = 0;

        virtual glm::vec3 getPosition() = 0;
        virtual void setShader(Ref<Shader> shader) = 0;
        virtual void setCamera(Ref<Camera> cam) = 0;

        virtual void addMaterial(Material mat) = 0;

        virtual void destroy() = 0;

        Transform transform;

        static Ref<Model> create(std::vector<Ref<Mesh>> meshes, std::vector<Material> materials);
    };
}