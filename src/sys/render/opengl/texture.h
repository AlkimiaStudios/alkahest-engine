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
        OpenGLTexture(const char* image, TextureType type, unsigned int slot);
        virtual ~OpenGLTexture() {};

        virtual inline TextureType getType() { return m_type; };

        void setUniformTexture(Ref<Shader> shader, const char* uniform, unsigned int unit) override;

        void bind() override;
        void unbind() override;
        void destroy() override;
    private:
        GLuint m_id;
        TextureType m_type;
        GLuint m_slot;
    };
}