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
        OpenGLTexture(const char* image, const char* texType, uint32_t slot, uint32_t format, uint32_t pixelType);
        virtual ~OpenGLTexture() {};

        virtual inline const char* getType() { return m_type; };

        void setUniformTexture(Ref<Shader> shader, const char* uniform, unsigned int unit) override;

        void bind() override;
        void unbind() override;
        void destroy() override;
    private:
        GLuint m_id;
        const char* m_type;
        GLuint m_slot;
    };
}