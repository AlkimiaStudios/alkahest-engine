#include "texture.h"

#include <GL/gl3w.h>
#include <stb_image.h>

namespace Alkahest
{
    Ref<Texture2D> Texture2D::create()
    {
        return CreateRef<OpenGLTexture2D>();
    }

    unsigned int OpenGLTexture2D::load(const char* file)
    {
        m_sourceFile = file;

        int width, height, colorChannels;

        stbi_set_flip_vertically_on_load(1);

        unsigned char* data = stbi_load(file, &width, &height, &colorChannels, 0);

        glGenTextures(1, &m_id);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);

        // Configures the type of algorithm that is used to make the image smaller or bigger
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Configures the way the texture repeats (if it does at all)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        uint32_t format;
        switch (colorChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGBA;
            break;
        }

        // Assign the image to the texture object
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Delete image data from mem since OpenGL has it now
        stbi_image_free(data);

        // Unbind the texture object so it can't be modified again
        glBindTexture(GL_TEXTURE_2D, 0);

        return m_id;
    }

    void OpenGLTexture2D::use(unsigned int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }
}