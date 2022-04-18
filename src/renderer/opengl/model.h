#pragma once

#include "../../macros.h"
#include "../common.h"
#include "../model.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    class API OpenGLModel : public Model
    {
    public:
        OpenGLModel(std::vector<Ref<Mesh>> meshes, std::vector<Material> material);
        ~OpenGLModel() {};

        void draw() override;

        glm::vec3 getPosition() override;
        void setShader(Ref<Shader> shader) override;
        void setCamera(Ref<Camera> cam) override;

        virtual inline void addMaterial(Material mat) { m_materials.push_back(mat); };

        void destroy() override;
    private:
        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        Ref<Shader> m_shader;
        Ref<Camera> m_cam;

        std::vector<Ref<Mesh>> m_meshes;
        std::vector<Material> m_materials;
    };
}