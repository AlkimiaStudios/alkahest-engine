#include "vertexarray.h"

namespace Alkahest
{
    Ref<VertexArray> VertexArray::create()
    {
        return CreateRef<OpenGLVertexArray>();
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glGenVertexArrays(1, &m_id);
    }

    void OpenGLVertexArray::linkAttribute(Ref<VertexBuffer> vbo, unsigned int layout, unsigned int numComponents, uint32_t type, unsigned long stride, void* offset)
    {
        vbo->bind();
        glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
        glEnableVertexAttribArray(layout);
        vbo->unbind();
    }

    void OpenGLVertexArray::bind()
    {
        glBindVertexArray(m_id);
    }

    void OpenGLVertexArray::unbind()
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::destroy()
    {
        glDeleteVertexArrays(1, &m_id);
    }
}