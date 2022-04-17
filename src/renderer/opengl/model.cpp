#include "model.h"

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

namespace Alkahest
{
    Ref<Model> Model::create(std::vector<Ref<Mesh>> meshes, std::vector<Material> materials)
    {
        return CreateRef<OpenGLModel>(meshes, materials);
    }

    OpenGLModel::OpenGLModel(std::vector<Ref<Mesh>> meshes, std::vector<Material> materials)
    {
        m_meshes = meshes;
        m_materials = materials;
    }

    glm::vec3 OpenGLModel::getPosition()
    {
        return m_position;
    }

    void OpenGLModel::setShader(Ref<Shader> shader)
    {
        m_shader = shader;
    }

    void OpenGLModel::setCamera(Ref<Camera> cam)
    {
        m_cam = cam;
    }

    void OpenGLModel::draw()
    {
        m_shader->activate();
        glUniformMatrix4fv(glGetUniformLocation(m_shader->getID(), "model"), 1, GL_FALSE, glm::value_ptr(transform.getMatrix()));

        for (Ref<Mesh> m : m_meshes)
        {
            m->draw(m_shader, m_cam);
        }
    };

    void OpenGLModel::destroy()
    {
    };
}