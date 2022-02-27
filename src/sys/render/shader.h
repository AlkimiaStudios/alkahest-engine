#pragma once

#include "../../macros.h"

namespace Alkahest
{
    class API Shader
    {
    public:
        Shader() {};
        virtual ~Shader() {};

        virtual unsigned int getID() = 0;

        virtual void activate() = 0;
        virtual void deactivate() = 0;
        virtual void destroy() = 0;

        static Ref<Shader> create(const char *vertShaderSourceFile, const char *fragShaderSourceFile);
    };
}