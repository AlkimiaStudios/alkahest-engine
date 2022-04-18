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
            unsigned int matIndex = m->getMaterialIndex();
            
            if (m_materials[matIndex].DiffuseTexture != nullptr)
            {
                m_materials[matIndex].DiffuseTexture->setUniformTexture(m_shader, "tex0", 0);
                m_materials[matIndex].DiffuseTexture->bind();
            }

            if (m_materials[matIndex].SpecularTexture != nullptr)
            {
                m_materials[matIndex].SpecularTexture->setUniformTexture(m_shader, "tex1", 1);
                m_materials[matIndex].SpecularTexture->bind();
            }

            glm::vec3 ambientColor = m_materials[matIndex].AmbientColor;
            glUniform3f(glGetUniformLocation(m_shader->getID(), "ambientColor"), ambientColor.x, ambientColor.y, ambientColor.z);

            glm::vec3 diffuseColor = m_materials[matIndex].DiffuseColor;
            glUniform3f(glGetUniformLocation(m_shader->getID(), "diffuseColor"), diffuseColor.x, diffuseColor.y, diffuseColor.z);

            glm::vec3 specularColor = m_materials[matIndex].SpecularColor;
            glUniform3f(glGetUniformLocation(m_shader->getID(), "specularColor"), specularColor.x, specularColor.y, specularColor.z);

            m->draw(m_shader, m_cam);
        }
    };

    void OpenGLModel::destroy()
    {
    };
}