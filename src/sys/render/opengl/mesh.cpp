#include "mesh.h"
#include "../../log/log.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    Ref<Mesh> Mesh::create(std::vector<Vertex> vertices, std::vector<uint32_t> indices, unsigned int matIndex)
    {
        return CreateRef<OpenGLMesh>(vertices, indices, matIndex);
    }

    OpenGLMesh::OpenGLMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, unsigned int matIndex)
    {
        // Save data in case we need it later
        m_vertices = vertices;
        m_indices = indices;
        m_matIndex = matIndex;

        // Create and bind the VAO
        m_vao = VertexArray::create();
        m_vao->bind();

        // Set up the VBO and EBO
        m_vbo = VertexBuffer::create(vertices);
        m_ebo = ElementBuffer::create(indices);

        // Link the VBO data to the locations in the shader
        m_vao->linkAttribute(m_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), static_cast<void*>(0));
        m_vao->linkAttribute(m_vbo, 1, 2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
        m_vao->linkAttribute(m_vbo, 2, 3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(5 * sizeof(GL_FLOAT)));

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

        cam->updateMatrixInShader(shader, "camMatrix");
        glm::vec3 camPos = cam->getPosition();
        glUniform3f(glGetUniformLocation(shader->getID(), "camPos"), camPos.x, camPos.y, camPos.z);

        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }
}