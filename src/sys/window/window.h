#pragma once

#include "../../macros.h"
#include "../events/event.h"

namespace Alkahest
{
    struct WindowProps
    {
        std::string title;
        unsigned int width;
        unsigned int height;

        WindowProps(std::string t = "Alkahest Game", unsigned int w = 1280,\
            unsigned int h = 768): title(t), width(w), height(h) {};
    };

    class IWindow
    {
    public:
        IWindow() {};
        virtual ~IWindow() {};
        virtual void onUpdate() = 0;
        virtual unsigned int getWidth() const = 0;
        virtual unsigned int getHeight() const = 0;
        virtual void setVSync(bool vsync) = 0;
        virtual bool isVSync() const = 0;
        virtual void setEventCallback(std::function<void(Event*)> e) = 0;

        static IWindow* create(const WindowProps& props = WindowProps());
    };
}