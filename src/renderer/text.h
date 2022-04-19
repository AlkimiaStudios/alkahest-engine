#pragma once

#include "../macros.h"
#include "common.h"
#include "../sys/render/shader.h"

#include <glm/glm.hpp>

namespace Alkahest
{
    struct Character;

    class API Font
    {
    public:
        Font() {};
        virtual ~Font() {};

        virtual void setSize(unsigned int size) = 0;
        virtual std::map<char, Character> getCharacterMap() = 0;

        static Ref<Font> create(const std::string& fontPath);
    };

    class API Text2D : public IRenderable
    {
    public:
        Text2D() {};
        virtual ~Text2D() {};

        virtual void draw() = 0;
        virtual void setShader(Ref<Shader> shader) = 0;
        virtual void setCamera(Ref<Camera> cam) = 0;

        virtual void setText(const std::string& text) = 0;
        virtual void setFont(Ref<Font> font) = 0;
        virtual void setPosition(glm::vec2 pos) = 0;
        virtual void setColor(glm::vec3 color) = 0;
        virtual void setAlpha(float a) = 0;

        static Ref<Text2D> create(const std::string& text, Ref<Font> font);
    };
}