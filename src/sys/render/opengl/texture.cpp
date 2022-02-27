#include "texture.h"
#include "../../log/log.h"

namespace Alkahest
{
    Ref<Texture> Texture::create(const char* image, uint32_t texType, uint32_t slot, uint32_t format, uint32_t pixelType)
    {
        return CreateRef<OpenGLTexture>(image, texType, slot, format, pixelType);
    }

    OpenGLTexture::OpenGLTexture(const char* image, uint32_t texType, uint32_t slot, uint32_t format, uint32_t pixelType)
    {
        // Assigns the texture type
        m_type = texType;

        // Store the width, height, and number of color channels for the image
        int width, height, colorChannels;

        // Set flip otherwise textures will be upside down
        stbi_set_flip_vertically_on_load(1);

        // Read image file into memory
        unsigned char *imgBytes = stbi_load(image, &width, &height, &colorChannels, 0);

        // Generate texture
        glGenTextures(1, &m_id);

        // Assign texture to a texture unit
        glActiveTexture(slot);
        glBindTexture(texType, m_id);

        // Configures the type of algorithm that is used to make the image smaller or bigger
        glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Configures the way the texture repeats (if it does at all)
        glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Assign the image to the texture object
        glTexImage2D(texType, 0, GL_RGBA, width, height, 0, format, pixelType, imgBytes);
        // Generate mipmaps
        glGenerateMipmap(texType);

        // Delete image data from mem since OpenGL has it now
        stbi_image_free(imgBytes);

        // Unbind the texture object so it can't be modified again
        glBindTexture(texType, 0);
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
        glBindTexture(m_type, m_id);
    }

    void OpenGLTexture::unbind()
    {
        glBindTexture(m_type, 0);
    }

    void OpenGLTexture::destroy()
    {
        glDeleteTextures(1, &m_id);
    }
}