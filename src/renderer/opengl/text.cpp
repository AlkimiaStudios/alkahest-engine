#include "text.h"
#include "../../sys/log/log.h"

#include <glm/glm.hpp>
#include <GL/gl3w.h>

namespace Alkahest
{
    Ref<Font> Font::create(const std::string& fontPath)
    {
        return CreateRef<OpenGLFont>(fontPath);
    }

    Ref<Text2D> Text2D::create(const std::string& text)
    {
        return CreateRef<OpenGLText2D>(text);
    }

    struct Character
    {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        long int Advance;    // Offset to advance to next glyph
    };

    OpenGLFont::OpenGLFont(const std::string& fontPath)
    {
        if (FT_Init_FreeType(&m_lib))
        {
            logError("Could not initialize FreeType!");
        }

        if (FT_New_Face(m_lib, fontPath.c_str(), 0, &m_face))
        {
            logError("Failed to load font! Path: {}", fontPath);
        }

        FT_Set_Pixel_Sizes(m_face, 0, m_size);

        // needed to import variable length textures for characters
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < 128; c++)
        {
            if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
            {
                logError("Error loading character {}", c);
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                m_face->glyph->bitmap.width,
                m_face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                m_face->glyph->bitmap.buffer
            );

            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // now store character for later use
            Character character = {
                texture, 
                glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
                glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
                m_face->glyph->advance.x
            };

            m_characters.insert(std::pair<char, Character>(c, character));
        }

        // resetting the alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        FT_Done_Face(m_face);
        FT_Done_FreeType(m_lib);
    }

    OpenGLText2D::OpenGLText2D(const std::string& text)
    {
        m_text = text;
    }

    void OpenGLText2D::draw()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        

        glDisable(GL_BLEND);
    }
}