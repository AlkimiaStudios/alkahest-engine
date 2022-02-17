#pragma once

#include "../../macros.h"
#include "event.h"

namespace Alkahest
{
    class API MouseEvent : public Event
    {
    public:
        SETUP_EVENT_FLAGS(EventCategory::Input | EventCategory::Mouse);
    protected:
        MouseEvent() {};
    };

    class API MouseButtonEvent : public MouseEvent
    {
    public:
        int getButton() const { return m_Button; };
    protected:
        MouseButtonEvent() {};
        int m_Button;
    };

    class API MouseButtonDownEvent : public MouseButtonEvent
    {
    public:
        MouseButtonDownEvent(int button, bool repeated): m_Repeated(repeated) { m_Button = button; };
        SETUP_EVENT_TYPE(MouseButtonDown);
        std::string toString() const override { return "MouseButtonDownEvent: " + std::to_string(m_Button); };
    private:
        bool m_Repeated = false;
    };

    class API MouseButtonUpEvent : public MouseButtonEvent
    {
    public:
        MouseButtonUpEvent(int button) { m_Button = button; };
        SETUP_EVENT_TYPE(MouseButtonUp);
        std::string toString() const override { return "MouseButtonUpEvent: " + std::to_string(m_Button); };
    };

    class API MouseMoveEvent : public MouseEvent
    {
    public:
        MouseMoveEvent(double x, double y): m_X(x), m_Y(y) {};
        SETUP_EVENT_TYPE(MouseMove);
        double getX() const { return m_X; };
        double getY() const { return m_Y; };
        std::string toString() const override { return "MouseMoveEvent: (" + std::to_string(m_X) + "," + std::to_string(m_Y) + ")"; };
    private:
        double m_X, m_Y;
    };

    class API MouseScrollEvent : public MouseEvent
    {
    public:
        MouseScrollEvent(double x, double y): m_X(x), m_Y(y) {};
        SETUP_EVENT_TYPE(MouseScroll);
        double getX() const { return m_X; };
        double getY() const { return m_Y; };
        std::string toString() const override { return "MouseScrollEvent: (" + std::to_string(m_X) + "," + std::to_string(m_Y) + ")"; };
    private:
        double m_X, m_Y;
    };
}