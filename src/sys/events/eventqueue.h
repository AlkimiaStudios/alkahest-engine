#pragma once

#include "../../macros.h"
#include "event.h"

namespace Alkahest
{
    struct EventQueueItem
    {
        Event* event;
        EventQueueItem* next;
        EventQueueItem(Event* e, EventQueueItem* n): event(e), next(n) {};
    };

    class API EventQueue
    {
    public:
        static EventQueue *getInstance();

        friend class EventDispatcher;

        ~EventQueue();

        void push(Event* e);
        Event* pop();
        unsigned int count() const;
    private:
        EventQueue();

        static EventQueue* m_pInstance;
        EventQueueItem* m_head;
        EventQueueItem* m_tail;
        unsigned int m_size;
    };

    class NOT_EXPORTED EventDispatcher
    {
    public:
        static EventDispatcher *getInstance();

        ~EventDispatcher();

        void run();
        void stop() { m_shouldStop = true; };

        void processTick();
        void registerCallback(EventType t, std::function<void(Event*)> cb, int priority = 0);
        // may include the option in the future to register by category as well as type,
        // but doing so would complicate the process of assigning priority to callbacks
    private:
        EventDispatcher();

        struct CallbackEntry {
            EventType t;
            std::function<void(Event*)> cb;
            int priority;
            CallbackEntry(EventType _t, std::function<void(Event*)> _cb, int p): t(_t), cb(_cb), priority(p) {};
        };
        static bool sortCallbacksByPriority(const CallbackEntry &cb1, const CallbackEntry &cb2);

        std::vector<CallbackEntry> m_callbacks;
        EventQueue* m_eqInstance;
        bool m_shouldStop = false;

        static EventDispatcher* m_pInstance;
    };
}