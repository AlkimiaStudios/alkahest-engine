#include "text.h"
#include "../../sys/log/log.h"
#include "../../sys/window/window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Alkahest
{
    Ref<Font> Font::create(const std::string& fontPath)
    {
        return CreateRef<OpenGLFont>(fontPath);
    }

    Ref<Text2D> Text2D::create(const std::string& text, Ref<Font> font)
    {
        Ref<Text2D> t = CreateRef<OpenGLText2D>(text, font);
        IWindow* w = IWindow::create();
        w->registerRenderable(t);
        return t;
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
        m_path = fontPath;
        loadFont(fontPath);
    }

    void OpenGLFont::loadFont(const std::string& fontPath)
    {
        m_characters.clear();

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

    OpenGLText2D::OpenGLText2D(const std::string& text, Ref<Font> font)
    {
        m_text = text;
        m_font = font;
        m_type = RenderType::RenderTypeText;

        // The text renderer for now is using the primitive implementation of
        // VAOs and VBOs instead of the class version. This is because the class
        // version only currently accepts the Vertex struct in its data arrays
        // and this currently seems like the only application of a non-Vertex-based
        // VAO and VBO that I'll need. If more arise then I'll look into re-working
        // this setup.
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        // Using dynamic draw because we will be manually updating buffer data in
        // the OpenGLText2D::draw() function
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void OpenGLText2D::draw()
    {
        std::map<char, Character> characters = m_font->getCharacterMap();
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Grab the current viewport height and width
        IWindow* w = IWindow::create();  // Despite the name, this returns the current window instance if it already exists
        int width = w->getWidth();
        int height = w->getHeight();
        // Use the w+h to create our projection matrix
        glm::mat4 projectionMatrix = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

        m_shader->activate();
        glUniformMatrix4fv(glGetUniformLocation(m_shader->getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniform3f(glGetUniformLocation(m_shader->getID(), "textColor"), m_color.x, m_color.y, m_color.z);
        glUniform1f(glGetUniformLocation(m_shader->getID(), "alpha"), m_alpha);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(m_vao);
        glm::vec2 pos = glm::vec2(m_pos.x, m_pos.y);
        if (pos.x < 0)
            pos.x = width + pos.x;
        if (pos.y < 0)
            pos.y = height + pos.y;

        std::string::const_iterator c;
        for (c = m_text.begin(); c != m_text.end(); c++)
        {
            Character ch = characters[*c];

            float x = pos.x + ch.Bearing.x;
            float y = pos.y - (ch.Size.y - ch.Bearing.y);

            float w = ch.Size.x;
            float h = ch.Size.y;

            float vertices[6][4] =
            {
                { x    , y + h, 0.0f, 0.0f },
                { x    , y    , 0.0f, 1.0f },
                { x + w, y    , 1.0f, 1.0f },
                { x    , y + h, 0.0f, 0.0f },
                { x + w, y    , 1.0f, 1.0f },
                { x + w, y + h, 1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // advance the cursor to draw next character
            pos.x += static_cast<float>(ch.Advance >> 6);
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glDisable(GL_BLEND);
    }
}