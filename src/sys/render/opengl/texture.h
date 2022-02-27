#pragma once

#include "../../../macros.h"
#include "../texture.h"

#include <GL/gl3w.h>
#include <stb_image.h>

namespace Alkahest
{
    class NOT_EXPORTED OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(const char* image, uint32_t texType, uint32_t slot, uint32_t format, uint32_t pixelType);
        virtual ~OpenGLTexture() {};

        void setUniformTexture(Ref<Shader> shader, const char* uniform, unsigned int unit) override;

        void bind() override;
        void unbind() override;
        void destroy() override;
    private:
        GLuint m_id;
        GLenum m_type;
    };
}