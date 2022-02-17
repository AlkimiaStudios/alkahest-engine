#pragma once

#include "../macros.h"
#include "common.h"
#include "message.h"
#include "connection.h"
#include "queue.h"

namespace Alkahest
{
    namespace Net
    {
        template<typename T>
        class API Client
        {
        public:
            Client() {};
            virtual ~Client()
            {
                disconnect();
            };

            bool connect(const std::string& host, const uint16_t port)
            {
                try
                {
                    // Resolve host/port combo
                    asio::ip::tcp::resolver resolver(m_context);
                    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

                    // Create new connection object
                    m_connection = std::make_unique<Connection<T>>(Connection<T>::Owner::Client, m_context, asio::ip::tcp::socket(m_context), m_qIn);

                    // Connect to server using endpoints (prime context)
                    m_connection->connectToServer(endpoints);

                    m_thrContext = std::thread([this]() { m_context.run(); });
                }
                catch (std::exception& e)
                {
                    std::stringstream ss;
                    ss << "Error connecting to server! " << e.what();
                    logError(ss.str());
                    return false;
                }

                logInfo("Connected!");
                return true;
            };

            void disconnect()
            {
                m_context.stop();

                if (isConnected())
                    m_connection->disconnect();

                if (m_thrContext.joinable())
                    m_thrContext.join();
                
                m_connection.release();
            };

            bool isConnected()
            {
                if (m_connection)
                    return m_connection->isConnected();
                return false;
            };

            bool hasIncomingMessages()
            {
                return !m_qIn.empty();
            }

            OwnedMessage<T> getNextMessage()
            {
                return m_qIn.pop_front();
            }

            void send(const Message<T>& msg)
            {
                if (isConnected())
                    m_connection->send(msg);
            };
        private:
            // Each client has a unique connection object
            std::unique_ptr<Connection<T>> m_connection;

            // Queue of incoming messages from the server
            TSQueue<OwnedMessage<T>> m_qIn;

            // The client manages its own ASIO context
            asio::io_context m_context;
            std::thread m_thrContext;
        };
    }
}