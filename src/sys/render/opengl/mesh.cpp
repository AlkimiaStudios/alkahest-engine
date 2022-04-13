#include "mesh.h"
#include "../../log/log.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    Ref<Mesh> Mesh::create(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture>> textures)
    {
        return CreateRef<OpenGLMesh>(vertices, indices, textures);
    }

    OpenGLMesh::OpenGLMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture>> textures)
    {
        // Save data in case we need it later
        m_vertices = vertices;
        m_indices = indices;
        m_textures = textures;

        // Create and bind the VAO
        m_vao = VertexArray::create();
        m_vao->bind();

        // Set up the VBO and EBO
        m_vbo = VertexBuffer::create(vertices);
        m_ebo = ElementBuffer::create(indices);

        // Link the VBO data to the locations in the shader
        m_vao->linkAttribute(m_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), static_cast<void*>(0));
        m_vao->linkAttribute(m_vbo, 1, 3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
        m_vao->linkAttribute(m_vbo, 2, 2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));
        m_vao->linkAttribute(m_vbo, 3, 3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(8 * sizeof(GL_FLOAT)));

        // Unbind everything
        m_vao->unbind();
        m_vbo->unbind();
        m_ebo->unbind();
    }

    OpenGLMesh::~OpenGLMesh()
    {
        // Destroy all the OpenGL data
        m_vao->destroy();
        m_vbo->destroy();
        m_ebo->destroy();

        // Reset all the shared pointers
        m_vao.reset();
        m_vbo.reset();
        m_ebo.reset();
    }

    void OpenGLMesh::draw(Ref<Shader> shader, Ref<Camera> cam)
    {
        // Activate the shader and VAO
        shader->activate();
        m_vao->bind();

        // Counts for different shader types
        unsigned int numDiffuse = 0;
        unsigned int numSpecular = 0;

        for (unsigned int i = 0; i < m_textures.size(); i++)
        {
            std::string num;
            std::string type = m_textures[i]->getType();

            if (type == "diffuse")
                num = std::to_string(numDiffuse++);
            else if (type == "specular")
                num = std::to_string(numSpecular++);
            else
                logError("Invalid texture type! {}", type);
            
            m_textures[i]->setUniformTexture(shader, (type + num).c_str(), i);
            m_textures[i]->bind();
        }

        cam->updateMatrixInShader(shader, "camMatrix");
        glm::vec3 camPos = cam->getPosition();
        glUniform3f(glGetUniformLocation(shader->getID(), "camPos"), camPos.x, camPos.y, camPos.z);

        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }
}