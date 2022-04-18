#pragma once

#include "../macros.h"

namespace Alkahest
{
    enum TextureType
    {
        TextureTypeDiffuse,
        TextureTypeSpecular,

        TextureTypeUnknown = -1
    };

    class Texture2D
    {
    public:
        Texture2D() {};
        virtual ~Texture2D() {};

        virtual unsigned int load(const char* file) = 0;

        virtual void use(unsigned int slot) = 0;

        unsigned int getID() { return m_id; };
        TextureType getType() { return m_type; };
        const char* getSourceFile() { return m_sourceFile; };

        void setType(TextureType type) { m_type = type; };

        static Ref<Texture2D> create();
    protected:
        unsigned int m_id;
        TextureType m_type;
        const char* m_sourceFile;
    };
}