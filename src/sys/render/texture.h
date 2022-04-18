#pragma once

#include "../../macros.h"
#include "shader.h"

namespace Alkahest
{
    enum TextureType
    {
        Diffuse,
        Specular,

        TextureTypeUnknown = -1
    };

    class API Texture
    {
    public:
        Texture() {};
        virtual ~Texture() {};

        virtual TextureType getType() = 0;

        virtual void setUniformTexture(Ref<Shader> shader, const char* uniform, unsigned int unit) = 0;

        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual void destroy() = 0;

        static Ref<Texture> create(const char* image, TextureType type, unsigned int slot);
    };
}