#pragma once

#include "../../../macros.h"
#include "../mesh.h"
#include "../vertexarray.h"
#include "../buffer.h"
#include "../texture.h"

#include <GL/gl3w.h>

namespace Alkahest
{
    class NOT_EXPORTED OpenGLMesh : public Mesh
    {
    public:
        OpenGLMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture>> textures);
        virtual ~OpenGLMesh();

        void draw(Ref<Shader> shader, Ref<Camera> cam) override;
    private:
        std::vector<Vertex> m_vertices;
        std::vector<GLuint> m_indices;
        std::vector<Ref<Texture>> m_textures;

        Ref<VertexArray> m_vao;
        Ref<VertexBuffer> m_vbo;
        Ref<ElementBuffer> m_ebo;
    };
}