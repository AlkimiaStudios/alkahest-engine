#pragma once

#include "../../macros.h"
#include "event.h"

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
    class API WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(int width, int height): m_Width(width), m_Height(height) {};
        SETUP_EVENT_TYPE(WindowResize);
        SETUP_EVENT_FLAGS(EventCategory::Window);
        int getWidth() { return m_Width; }
        int getHeight() { return m_Height; }
        std::string toString() const override { return "WindowResizeEvent: (" + std::to_string(m_Width) + "x" + std::to_string(m_Height) + ")"; };
    private:
        // auto m_Window;
        int m_Width, m_Height;
    };

    class API WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() {};
        SETUP_EVENT_TYPE(WindowClose);
        SETUP_EVENT_FLAGS(EventCategory::Window);
        std::string toString() const override { return "WindowCloseEvent"; };
    private:
        // auto m_Window;
    };
}