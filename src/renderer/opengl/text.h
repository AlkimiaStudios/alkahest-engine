#pragma once

#include "../../macros.h"
#include "../text.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Alkahest
{
    class NOT_EXPORTED OpenGLFont : public Font
    {
    public:
        OpenGLFont(const std::string& fontPath);
        virtual ~OpenGLFont() {};

        virtual inline void setSize(unsigned int size) { m_size = size; };
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
        OpenGLText2D(const std::string& text);
        virtual ~OpenGLText2D() {};

        void draw() override;
        void setShader(Ref<Shader> shader) {};
        void setCamera(Ref<Camera> cam) {};

        virtual inline void setText(const std::string& text) { m_text = text; };
        virtual inline void setFont(Ref<Font> font) { m_font = font; };
        virtual inline void setPosition(glm::vec2 pos) { m_pos = pos; };
    private:
        Ref<Font> m_font;
        std::string m_text;
        glm::vec2 pos;
    };
}