#pragma once

#include "../../../macros.h"
#include "../shader.h"

#include <GL/gl3w.h>

namespace Alkahest
{
    class NOT_EXPORTED OpenGLShader : public Shader
    {
    private:
        std::string readSourceFile(const char* path);
    public:
        OpenGLShader(GLuint id): m_id(id) {};
        OpenGLShader(const char* vertexShaderSourceFile, const char* fragmentShaderSourceFile);

        OpenGLShader() {};

        GLuint getID() override { return m_id; };

        void activate();
        void deactivate();
        void destroy();
    private:
        GLuint m_id;
    };
}