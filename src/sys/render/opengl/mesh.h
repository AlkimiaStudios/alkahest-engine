#pragma once

#include "../../../macros.h"
#include "../mesh.h"
#include "../vertexarray.h"
#include "../buffer.h"

#include <GL/gl3w.h>

namespace Alkahest
{
    class NOT_EXPORTED OpenGLMesh : public Mesh
    {
    public:
        OpenGLMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, unsigned int matIndex);
        virtual ~OpenGLMesh();

        void draw(Ref<Shader> shader, Ref<Camera> cam) override;

        virtual inline unsigned int getMaterialIndex() { return m_matIndex; };
    private:
        std::vector<Vertex> m_vertices;
        std::vector<GLuint> m_indices;
        unsigned int m_matIndex;

        Ref<VertexArray> m_vao;
        Ref<VertexBuffer> m_vbo;
        Ref<ElementBuffer> m_ebo;
    };
}