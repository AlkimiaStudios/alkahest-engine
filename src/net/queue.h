#pragma once

#include "../macros.h"
#include "common.h"

namespace Alkahest
{
    namespace Net
    {
        template<typename T>
        class NOT_EXPORTED TSQueue
        {
        public:
            TSQueue() = default;
            TSQueue(const TSQueue<T>&) = delete;
            virtual ~TSQueue() { clear(); };
        public:
            const T& front()
            {
                std::scoped_lock lock(m_mtxQueue);
                return m_deqQueue.front();
            };

            const T& back()
            {
                std::scoped_lock lock(m_mtxQueue);
                return m_deqQueue.back();
            };

            T pop_front()
            {
                std::scoped_lock lock(m_mtxQueue);
                auto t = std::move(m_deqQueue.front());
                m_deqQueue.pop_front();
                return t;
            };

            T pop_back()
            {
                std::scoped_lock lock(m_mtxQueue);
                auto t = std::move(m_deqQueue.back());
                m_deqQueue.pop_back();
                return t;
            };

            void push_front(const T& t)
            {
                std::scoped_lock lock(m_mtxQueue);
                m_deqQueue.emplace_back(std::move(t));

                // Notify the condition variable that an item was added to the queue
                std::unique_lock<std::mutex> ul(m_mtxBlocking);
                m_cvBlocking.notify_one();
            };

            void push_back(const T& t)
            {
                std::scoped_lock lock(m_mtxQueue);
                m_deqQueue.emplace_back(std::move(t));

                // Notify the condition variable that an item was added to the queue
                std::unique_lock<std::mutex> ul(m_mtxBlocking);
                m_cvBlocking.notify_one();
            };

            bool empty()
            {
                std::scoped_lock lock(m_mtxQueue);
                return m_deqQueue.empty();
            };

            size_t size()
            {
                std::scoped_lock lock(m_mtxQueue);
                return m_deqQueue.size();
            };

            void clear()
            {
                std::scoped_lock lock(m_mtxQueue);
                m_deqQueue.clear();
            };

            void wait()
            {
                while(empty())
                {
                    std::unique_lock<std::mutex> ul(m_mtxBlocking);
                    m_cvBlocking.wait(ul);
                }
            };
        private:
            std::mutex m_mtxQueue;
            std::deque<T> m_deqQueue;
            std::condition_variable m_cvBlocking;
            std::mutex m_mtxBlocking;
        };
    }
}