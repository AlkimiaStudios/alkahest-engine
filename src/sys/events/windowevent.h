#pragma once

#include "../../macros.h"
#include "event.h"

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