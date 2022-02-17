#include "eventqueue.h"
#include "../log/log.h"

namespace Alkahest
{
    EventQueue* EventQueue::m_pInstance = nullptr;
    EventDispatcher* EventDispatcher::m_pInstance = nullptr;

    static std::mutex eventQueueMutex;
    static std::mutex eventDispatcherMutex;

    EventQueue *EventQueue::getInstance()
    {
        if (m_pInstance == nullptr)
            m_pInstance = new EventQueue();
        return m_pInstance;
    }

    EventQueue::EventQueue()
    {
        m_head = m_tail = nullptr;
        m_size = 0;
    }

    EventQueue::~EventQueue()
    {
        // Acquire lock to ensure atomicity
        std::lock_guard<std::mutex> l(eventQueueMutex);

        // loop through queue to delete any remaining pointers
        while (m_head != nullptr)
        {
            EventQueueItem* h = m_head;
            m_head = h->next;
            delete h;
        }
        m_tail = nullptr;
    }

    void EventQueue::push(Event* e)
    {
        // Acquire lock to ensure atomicity
        std::lock_guard<std::mutex> l(eventQueueMutex);

        // Create new EventQueueItem on heap
        EventQueueItem* i = new EventQueueItem(e, nullptr);

        // empty queue edge case
        if (m_head == nullptr)
        {
            m_head = i;
            m_tail = i;
            m_size = 1;
            return;
        }

        // add item as new tail
        m_tail->next = i;
        m_tail = i;
        m_size++;
    }

    Event* EventQueue::pop()
    {
        // Acquire lock to ensure atomicity
        std::lock_guard<std::mutex> l(eventQueueMutex);

        // Save head pointer for deletion after pop
        EventQueueItem* h = m_head;
        Event* e = h->event;
        m_head = h->next;
        m_size--;
        delete h;  // delete here to close loop from push()
        return e;
    }

    unsigned int EventQueue::count() const
    {
        // Using std::shared_lock to allow non-blocking read-only access
        std::lock_guard<std::mutex> l(eventQueueMutex);

        return m_size;
    };

    EventDispatcher *EventDispatcher::getInstance()
    {
        if (m_pInstance == nullptr)
            m_pInstance = new EventDispatcher();
        return m_pInstance;
    }

    EventDispatcher::EventDispatcher()
    {
        m_eqInstance = EventQueue::getInstance();
    }

    EventDispatcher::~EventDispatcher()
    {
        // Delete the EventQueue when the Dispatcher
        // is destroyed, as the queue shouldn't be
        // left without a dispatcher. This may be moved
        // in the future.
        delete m_eqInstance;
    }

    void EventDispatcher::run()
    {
        while (!m_shouldStop)
        {
            processTick();
        }
    }

    bool EventDispatcher::sortCallbacksByPriority(const CallbackEntry &cb1, const CallbackEntry &cb2)
    {
        return (cb1.priority < cb2.priority);
    }

    void EventDispatcher::processTick()
    {
        if (m_eqInstance->count() == 0)
            return; // don't process if there are no events

        logTrace("Events waiting in queue: " + std::to_string(m_eqInstance->count()));
        
        // Acquire lock to ensure atomicity
        std::unique_lock<std::mutex> l(eventDispatcherMutex);

        // grab event
        Event* e = m_eqInstance->pop();

        // create vector of callbacks that should be called
        std::vector<CallbackEntry> valid_callbacks;
        for (int i = 0; i < m_callbacks.size(); i++)
        {
            if (m_callbacks[i].t == e->getEventType())
            {
                valid_callbacks.push_back(m_callbacks[i]);
            }
        }
        // sort callbacks by priority
        std::sort(valid_callbacks.begin(), valid_callbacks.end(), sortCallbacksByPriority);

        // Unlocking here because we are no longer in
        // dangerous territory, just calling some CBs
        l.unlock();

        for(int i = 0; i < valid_callbacks.size(); i++)
        {
            // This is a very dirty way of handling callbacks, as the
            // function should NOT be called in this thread for
            // potential of blocking the entire event queue, but
            // it'll have to work for now
            valid_callbacks[i].cb(e);
        }
    }

    void EventDispatcher::registerCallback(EventType t, std::function<void(Event*)> cb, int priority)
    {
        // Acquire lock to ensure atomicity
        std::lock_guard<std::mutex> l(eventDispatcherMutex);

        // Since there is no good way to compare callbacks,
        // all callbacks will be added to the dispatcher.
        // This leaves it up to other processes to not register
        // the same callback more than once to guard against
        // unexpected behavior
        m_callbacks.push_back(CallbackEntry(t, cb, priority));
    }
}