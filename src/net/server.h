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
        class API Server
        {
        public:
            Server(uint16_t port) : m_asioAcceptor(m_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {};
            virtual ~Server() { stop(); };

            bool start()
            {
                try
                {
                    // Prime the asio context with work to do, i.e. wait for a client connection
                    waitForConnection();

                    m_thrContext = std::thread([this]() { m_context.run(); });
                }
                catch (std::exception& e)
                {
                    std::stringstream ss;
                    ss << "Server Error: " << e.what();
                    logError(ss.str());
                    return false;
                }
                
                logInfo("Server started!");
                return true;
            };

            void stop()
            {
                m_context.stop();

                if (m_thrContext.joinable())
                {
                    m_thrContext.join();
                }
            };

            // ASYNC
            void waitForConnection()
            {
                m_asioAcceptor.async_accept(
                    [this](std::error_code ec, asio::ip::tcp::socket socket)
                    {
                        if (!ec)
                        {
                            std::stringstream ss;
                            ss << "New connection: " << socket.remote_endpoint();
                            logInfo(ss.str());

                            std::shared_ptr<Connection<T>> newConn = std::make_shared<Connection<T>>(Connection<T>::Owner::Server, m_context, std::move(socket), m_qIn);

                            if (onClientConnect(newConn))
                            {
                                m_deqConnections.push_back(std::move(newConn));
                                m_deqConnections.back()->connectToClient(m_idCounter++);
                                ss.clear();
                                ss << "Connection approved! ID: " << m_deqConnections.back()->getID();
                                logInfo(ss.str());
                            }
                        }
                        else
                        {
                            logError("Connection denied.");
                        }

                        waitForConnection();
                    }
                );
            };

            void messageClient(std::shared_ptr<Connection<T>> client, const Message<T>& msg)
            {
                if (client && client->isConnected())
                {
                    client->send(msg);
                }
                else
                {
                    // The client is no longer connected, so we clean up
                    onClientDisconnect(client);

                    // We reset the pointer
                    client.reset();

                    // And remove from the deque
                    m_deqConnections.erase(std::remove(m_deqConnections.begin(), m_deqConnections.end(), client), m_deqConnections.end());
                }
            };

            void messageAllClients(const Message<T>& msg, std::shared_ptr<Connection<T>> pIgnoreClient = nullptr)
            {
                bool disconnectedClientExists = false;

                for (auto& client : m_deqConnections)
                {
                    if (client && client->isConnected())
                    {
                        if (client != pIgnoreClient)
                        {
                            client->send(msg);
                        }
                    }
                    else
                    {
                        disconnectedClientExists = true;
                        onClientDisconnect(client);
                        client.reset();
                    }
                }

                if (disconnectedClientExists)
                {
                    // TODO: remove invalid clients from deque
                }
            };

            void update(size_t maxMessages = -1, bool wait = true)
            {
                if (wait)
                {
                    m_qIn.wait();
                }

                size_t msgCount = 0;
                while (msgCount < maxMessages && !m_qIn.empty())
                {
                    auto msg = m_qIn.pop_front();

                    onMessage(msg.remote, msg.msg);

                    msgCount++;
                }
            };
        protected:
            virtual bool onClientConnect(std::shared_ptr<Connection<T>> client)
            {
                return false;
            };

            virtual void onClientDisconnect(std::shared_ptr<Connection<T>> client) {};
            virtual void onMessage(std::shared_ptr<Connection<T>> client, Message<T>& msg) {};
        protected:
            // Queue for incoming messages
            TSQueue<OwnedMessage<T>> m_qIn;

            // Set of active, valid connections
            std::deque<std::shared_ptr<Connection<T>>> m_deqConnections;

            // The server handles its own ASIO context
            asio::io_context m_context;
            // Thread for context processing. At some point in the future,
            // it may make sense to instead use a thread pool, specifically
            // for server instances (shouldn't need for client)
            // client = single thread in pool?
            std::thread m_thrContext;

            // ASIO acceptor for handling incoming connections
            asio::ip::tcp::acceptor m_asioAcceptor;

            // Identifier provided to connected clients
            uint32_t m_idCounter = 1;
        };
    }
}