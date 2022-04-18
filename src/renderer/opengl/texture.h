#pragma once

#include "../../macros.h"
#include "../texture.h"

namespace Alkahest
{
    class NOT_EXPORTED OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D() {};
        virtual ~OpenGLTexture2D() {};

        unsigned int load(const char* file) override;

        void use(unsigned int slot) override;
    };
}