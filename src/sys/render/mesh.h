#pragma once

#include "../../macros.h"
#include "shader.h"
#include "camera.h"
#include "buffer.h"

namespace Alkahest
{
    class API Mesh
    {
    public:
        Mesh() {};
        virtual ~Mesh() {};

        virtual void draw(Ref<Shader> shader, Ref<Camera> cam) = 0;

        virtual unsigned int getMaterialIndex() = 0;

        static Ref<Mesh> create(std::vector<Vertex> vertices, std::vector<uint32_t> indices, unsigned int matIndex = -1);
    };
}