#pragma once

#include "../../../macros.h"
#include "../vertexarray.h"

#include <GL/gl3w.h>

namespace Alkahest
{
    class NOT_EXPORTED OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray() {};

        void linkAttribute(Ref<VertexBuffer> vbo, unsigned int layout, unsigned int numComponents, uint32_t type, unsigned long stride, void* offset) override;

        void bind() override;
        void unbind() override;
        void destroy() override;
    private:
        GLuint m_id;
    };
}