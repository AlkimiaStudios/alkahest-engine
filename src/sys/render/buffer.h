#pragma once

#include "../../macros.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 texCoords;
        glm::vec3 normals;
    };
    
    class API VertexBuffer
    {
    public:
        VertexBuffer() {};
        virtual ~VertexBuffer() {};

        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual void destroy() = 0;

        static Ref<VertexBuffer> create(std::vector<Vertex> vertices);
    };

    class API ElementBuffer
    {
    public:
        ElementBuffer() {};
        virtual ~ElementBuffer() {};

        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual void destroy() = 0;

        static Ref<ElementBuffer> create(std::vector<unsigned int> indices);
    };
}