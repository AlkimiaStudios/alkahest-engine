#pragma once

#include "../../macros.h"
#include "../text.h"
#include "../../sys/render/shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <GL/gl3w.h>

namespace Alkahest
{
    class NOT_EXPORTED OpenGLFont : public Font
    {
    public:
        OpenGLFont(const std::string& fontPath);
        virtual ~OpenGLFont() {};

        virtual inline void setSize(unsigned int size) { m_size = size; loadFont(m_path); };
        virtual inline std::map<char, Character> getCharacterMap() { return m_characters; };
    private:
        void loadFont(const std::string& fontPath);
    private:
        std::string m_path;
        FT_Library m_lib;
        FT_Face m_face;
        unsigned int m_size = 48;
        std::map<char, Character> m_characters;
    };

    class NOT_EXPORTED OpenGLText2D : public Text2D
    {
    public:
        OpenGLText2D(const std::string& text, Ref<Font> font);
        virtual ~OpenGLText2D() {};

        void draw() override;
        void setShader(Ref<Shader> shader) { m_shader = shader; };
        void setCamera(Ref<Camera> cam) {}; // don't need camera here but it is required

        virtual inline void setText(const std::string& text) { m_text = text; };
        virtual inline void setFont(Ref<Font> font) { m_font = font; };
        virtual inline void setPosition(glm::vec2 pos) { m_pos = pos; };
        virtual inline void setColor(glm::vec3 color) { m_color = glm::normalize(color); };
        virtual inline void setAlpha(float a) { m_alpha = a; };
    private:
        Ref<Font> m_font;
        std::string m_text;
        glm::vec2 m_pos = glm::vec2(10.0f, 10.0f);
        glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);\
        float m_alpha = 1.0f;

        GLuint m_vao;
        GLuint m_vbo;
        Ref<Shader> m_shader;
    };
}