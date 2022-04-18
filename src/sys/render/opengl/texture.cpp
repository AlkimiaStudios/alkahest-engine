#include "texture.h"
#include "../../log/log.h"

namespace Alkahest
{
    Ref<Texture> Texture::create(const char* image, TextureType type, unsigned int slot)
    {
        return CreateRef<OpenGLTexture>(image, type, slot);
    }

    OpenGLTexture::OpenGLTexture(const char* image, TextureType type, unsigned int slot)
    {
        // Assigns the texture type and slot
        m_type = type;
        m_slot = slot;

        // Store the width, height, and number of color channels for the image
        int width, height, colorChannels;

        // Set flip otherwise textures will be upside down
        stbi_set_flip_vertically_on_load(1);

        // Read image file into memory
        unsigned char *imgBytes = stbi_load(image, &width, &height, &colorChannels, 0);

        // Generate texture
        glGenTextures(1, &m_id);

        // Assign texture to a texture unit
        glActiveTexture(GL_TEXTURE0 + slot);
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_INT, imgBytes);
        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Delete image data from mem since OpenGL has it now
        stbi_image_free(imgBytes);

        // Unbind the texture object so it can't be modified again
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLTexture::setUniformTexture(Ref<Shader> shader, const char* uniform, unsigned int unit)
    {
        GLuint uniformLocation = glGetUniformLocation(shader->getID(), uniform);
        // shader must be active to set uniform
        shader->activate();
        // set uniform value
        glUniform1i(uniformLocation, unit);
    }

    void OpenGLTexture::bind()
    {
        glActiveTexture(GL_TEXTURE0 + m_slot);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    void OpenGLTexture::unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLTexture::destroy()
    {
        glDeleteTextures(1, &m_id);
    }
}