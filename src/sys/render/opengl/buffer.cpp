#include "buffer.h"

namespace Alkahest
{
    Ref<VertexBuffer> VertexBuffer::create(std::vector<Vertex> vertices)
    {
        return CreateRef<OpenGLVertexBuffer>(vertices);
    }

    Ref<ElementBuffer> ElementBuffer::create(std::vector<unsigned int> indices)
    {
        return CreateRef<OpenGLElementBuffer>(indices);
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<Vertex> vertices)
    {
        glGenBuffers(1, &m_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    }

    void OpenGLVertexBuffer::bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
    }

    void OpenGLVertexBuffer::unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBuffer::destroy()
    {
        glDeleteBuffers(1, &m_id);
    }

    OpenGLElementBuffer::OpenGLElementBuffer(std::vector<GLuint> indices)
    {
        glGenBuffers(1, &m_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    }

    void OpenGLElementBuffer::bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    }

    void OpenGLElementBuffer::unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void OpenGLElementBuffer::destroy()
    {
        glDeleteBuffers(1, &m_id);
    }
}