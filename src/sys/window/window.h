#pragma once

#include "../../macros.h"
#include "../events/event.h"

// Much of the early systems developed for AlkahestEngine were developed following
// along with the Game Engine series from The Cherno (Yan Chernikov) as he built
// the Hazel game engine. A future refactor of these systems is planned to
// eventually re-work this code to adapt it more closely to the direction the
// AlkahestEngine is heading. In the meantime, however, thank you to Yan for
// providing one of the most educational channels on YT when it comes to C++
// and OpenGL development.
//
// In compliance with the Hazel license, a copy of said license is available at
// {project_root}/.licenses/Hazel.LICENSE
//
// The code within this file has been modified from the original source, and
// this notice is provided in compliance with the above license terms.

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