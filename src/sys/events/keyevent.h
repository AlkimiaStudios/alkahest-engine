#pragma once

#include "../../macros.h"
#include "event.h"

namespace Alkahest
{
    class API KeyEvent : public Event
    {
    public:
        SETUP_EVENT_FLAGS(EventCategory::Input | EventCategory::Keyboard);
    protected:
        KeyEvent() {};
        int m_Keycode;
    };

    class API KeyDownEvent : public KeyEvent
    {
    public:
        KeyDownEvent(int keycode, bool repeated): m_Repeated(repeated) { m_Keycode = keycode; };
        SETUP_EVENT_TYPE(KeyDown);
        std::string toString() const override { return "KeyDownEvent: " + std::to_string(m_Keycode); };
    private:
        bool m_Repeated = false;
    };

    class API KeyUpEvent : public KeyEvent
    {
    public:
        KeyUpEvent(int keycode) { m_Keycode = keycode; };
        SETUP_EVENT_TYPE(KeyUp);
        std::string toString() const override { return "KeyUpEvent: " + std::to_string(m_Keycode); };
    };
}