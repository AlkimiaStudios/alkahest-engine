#pragma once

#include "../macros.h"
#include "common.h"
#include "message.h"
#include "queue.h"

namespace Alkahest
{
    namespace Net
    {
        template<typename T>
        class API Connection : std::enable_shared_from_this<Connection<T>>
        {
        public:
            enum class Owner
            {
                Client,
                Server
            };

        public:
            Connection(Owner owner, asio::io_context& context, asio::ip::tcp::socket socket, TSQueue<OwnedMessage<T>>& qIn)
                : m_ownerType(owner), m_context(context), m_socket(std::move(socket)), m_qIn(qIn) {};

            virtual ~Connection() {};
            uint32_t getID() const { return id; };
        public:
            void connectToClient(uint32_t uid)
            {
                if (m_ownerType == Owner::Server)
                {
                    // Client initiates connections, all we have to do is accept
                    if (m_socket.is_open())
                    {
                        id = uid;
                        readMessage();
                    }
                }
            };

            void connectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
            {
                if (m_ownerType == Owner::Client)
                {
                    asio::async_connect(m_socket, endpoints,
                    [this](std::error_code ec, asio::ip::tcp::endpoint endpoint){
                        // Connection is valid, prime context to read an incoming message
                        if (!ec)
                        {
                            readMessage();
                        }
                    });
                }
            };

            void disconnect()
            {
                asio::post(m_context,
                    [this]()
                    {
                        m_socket.close();
                    });
            };

            bool isConnected() const { return m_socket.is_open(); };

            void send(const Message<T>& msg)
            {
                asio::post(m_context,
                    [this, msg]()
                    {
                        // If there are messages in the queue already, it is
                        // safe to assume that we are in the process of sending them out
                        bool sending = !m_qOut.empty();
                        // ...so we add our message to the back of the queue
                        m_qOut.push_back(msg);
                        // ...and if we weren't already sending, we start doing so
                        if (!sending)
                        {
                            writeMessage();
                        }
                    });
            };
        private:
            void readMessage()
            {
                asio::async_read(m_socket, asio::buffer(&m_tmpMsg.header, sizeof(Header<T>)),
                    [this](std::error_code ec, std::size_t length)
                    {
                        if (!ec)
                        {
                            // The message header has been read successfully, so we
                            // check to see if there are any bytes to be read for
                            // the message body
                            if (m_tmpMsg.header.size > 0)
                            {
                                asio::async_read(m_socket, asio::buffer(m_tmpMsg.body.data(), m_tmpMsg.header.size),
                                    [this](std::error_code ec, std::size_t length)
                                    {
                                        if (!ec)
                                        {
                                            // The message has been read, so we add it to the queue
                                            addMessageToIncomingQueue();
                                        }
                                        else
                                        {
                                            m_socket.close();
                                            std::stringstream ss;
                                            ss << "Error (" << ec << ") in Connection::readMessage, connection " << id << " closed!";
                                            logError(ss.str());
                                        }
                                    });
                            }
                            else
                            {
                                // If not, we add the message to the queue
                                addMessageToIncomingQueue();
                            }
                        }
                        else
                        {
                            // TODO: Investigate message read failures
                            m_socket.close();
                            std::stringstream ss;
                            ss << "Error (" << ec << ") in Connection::readMessage, connection " << id << " closed!";
                            logError(ss.str());
                        }
                    });
            };

            void writeMessage()
            {
                asio::async_write(m_socket, asio::buffer(&m_qOut.front().header, sizeof(Message<T>)),
                    [this](std::error_code ec, std::size_t length)
                    {
                        // If the error code is set, then there has been an issue.
                        // If not, we continue
                        if (!ec)
                        {
                            // We were successful in sending the header, so we check
                            // to see if there are bytes to write for the body of the message
                            if (m_qOut.front().body.size() > 0)
                            {
                                // There is data in the body, so we send it as well
                                asio::async_write(m_socket, asio::buffer(m_qOut.front().body.data(), m_qOut.front().body.size()),
                                    [this](std::error_code ec, std::size_t length)
                                    {
                                        if (!ec)
                                        {
                                            // We are done with this message, so we pop it
                                            m_qOut.pop_front();
                                            // ...and check if there are more messages to send
                                            if (!m_qOut.empty())
                                            {
                                                writeMessage();
                                            }
                                        }
                                        else
                                        {
                                            // TODO: Investigate various send failure reasons
                                            m_socket.close();
                                            std::stringstream ss;
                                            ss << "Error (" << ec << ") in Connection::writeMessage, connection " << id << " closed!";
                                            logError(ss.str());
                                        }
                                    });
                            }
                            else
                            {
                                // There is no more data for this message, so we are done with that
                                m_qOut.pop_front();
                                // ...and check if there are any more messages to send
                                if (!m_qOut.empty())
                                {
                                    // There are more messages, so we prime the asio context
                                    // to send the next one
                                    writeMessage();
                                }
                            }
                        }
                        else
                        {
                            // ...the writing has failed, so for now we assume that the connection
                            // has closed and we close the socket/connection.
                            // TODO: Investigate various send failure reasons
                            m_socket.close();
                            std::stringstream ss;
                            ss << "Error (" << ec << ") in Connection::writeMessage, connection " << id << " closed!";
                            logError(ss.str());
                        }
                    });
            };

            void addMessageToIncomingQueue()
            {
                if (m_ownerType == Owner::Server)
                {
                    // If we are on the server, we set the owner to a self pointer
                    // and add it to the incoming queue
                    m_qIn.push_back({this->shared_from_this(), m_tmpMsg});
                }
                else
                {
                    // ...otherwise, we know the message came from the server
                    m_qIn.push_back({nullptr, m_tmpMsg});
                }
            };
        private:
            // The connection stores a reference to the ASIO context to use later
            asio::io_context& m_context;

            // Each Connection has a unique socket
            asio::ip::tcp::socket m_socket;

            // Outgoing queue for messages being sent by the owner
            TSQueue<Message<T>> m_qOut;

            // Incoming queue for messages to be read by the owner
            TSQueue<OwnedMessage<T>>& m_qIn;

            // Temporary message object for incoming messages
            Message<T> m_tmpMsg{};

            // The Owner decides some behavior between client/server
            Owner m_ownerType = Owner::Server;
            
            uint32_t id = 0;
        };
    }
}