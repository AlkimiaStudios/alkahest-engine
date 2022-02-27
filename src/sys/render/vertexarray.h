#pragma once

#include "../../macros.h"
#include "buffer.h"

namespace Alkahest
{
    class API VertexArray
    {
    public:
        VertexArray() {};
        virtual ~VertexArray() {};

        virtual void linkAttribute(Ref<VertexBuffer> vbo, unsigned int layout, unsigned int numComponents, uint32_t type, unsigned long stride, void* offset) = 0;
        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual void destroy() = 0;

        static Ref<VertexArray> create();
    };
}