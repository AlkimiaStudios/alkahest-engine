#pragma once

#include "../macros.h"
#include "../sys/window/window.h"
#include "../sys/events/event.h"
#include "../sys/events/eventqueue.h"

namespace Alkahest
{
    class API Application
    {
    public:
        Application();
        virtual ~Application();
        void init();
        virtual void run();
        virtual void update() {};
        void stop() { m_shouldStop = true; };
    protected:
        std::unique_ptr<IWindow> m_window;
        std::unique_ptr<EventDispatcher> m_dispatcher;
        bool m_shouldStop = false;

        using super = Application;
    private:
        std::thread m_eventThread;
        static void onEvent(Event* e);
    };

    Application* CreateApplication();
}