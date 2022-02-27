#pragma once

#include "../../../macros.h"
#include "../buffer.h"

#include <GL/gl3w.h>
#include <glm/glm.hpp>

namespace Alkahest
{
    class NOT_EXPORTED OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(std::vector<Vertex> vertices);
        virtual ~OpenGLVertexBuffer() {};

        void bind() override;
        void unbind() override;
        void destroy() override;
    private:
        GLuint m_id;
    };

    class NOT_EXPORTED OpenGLElementBuffer : public ElementBuffer
    {
    public:
        OpenGLElementBuffer(std::vector<GLuint> indices);
        virtual ~OpenGLElementBuffer() {};

        void bind() override;
        void unbind() override;
        void destroy() override;
    private:
        GLuint m_id;
    };
}