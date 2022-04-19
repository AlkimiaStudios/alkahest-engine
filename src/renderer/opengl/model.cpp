#include "model.h"
#include "../../sys/log/log.h"

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

#define DIFFUSE_TEXTURE 0
#define SPECULAR_TEXTURE 1

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
        m_type = RenderType::RenderTypeModel;
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
            
            if (matIndex < m_materials.size())
            {
                if (m_materials[matIndex].DiffuseTexture != nullptr)
                {
                    Ref<Texture2D> d = m_materials[matIndex].DiffuseTexture;
                    d->use(DIFFUSE_TEXTURE);
                    glUniform1i(glGetUniformLocation(m_shader->getID(), "tex0"), DIFFUSE_TEXTURE);
                }

                if (m_materials[matIndex].SpecularTexture != nullptr)
                {
                    Ref<Texture2D> s = m_materials[matIndex].SpecularTexture;
                    s->use(DIFFUSE_TEXTURE);
                    glUniform1i(glGetUniformLocation(m_shader->getID(), "tex1"), SPECULAR_TEXTURE);
                }

                glm::vec3 ambientColor = m_materials[matIndex].AmbientColor;
                //logTrace("AmbientColor: {},{},{}", ambientColor.x, ambientColor.y, ambientColor.z);
                glUniform3f(glGetUniformLocation(m_shader->getID(), "ambientColor"), ambientColor.x, ambientColor.y, ambientColor.z);

                glm::vec3 diffuseColor = m_materials[matIndex].DiffuseColor;
                //logTrace("DiffuseColor: {},{},{}", diffuseColor.x, diffuseColor.y, diffuseColor.z);
                glUniform3f(glGetUniformLocation(m_shader->getID(), "diffuseColor"), diffuseColor.x, diffuseColor.y, diffuseColor.z);

                glm::vec3 specularColor = m_materials[matIndex].SpecularColor;
                //logTrace("SpecularColor: {},{},{}", specularColor.x, specularColor.y, specularColor.z);
                glUniform3f(glGetUniformLocation(m_shader->getID(), "specularColor"), specularColor.x, specularColor.y, specularColor.z);
            }

            m->draw(m_shader, m_cam);
        }
    };

    void OpenGLModel::destroy()
    {
    };
}