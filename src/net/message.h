#pragma once

#include "../macros.h"
#include "common.h"

namespace Alkahest
{
    namespace Net
    {
        template<typename T>
        struct NOT_EXPORTED Header
        {
            T id{};
            uint32_t size = 0;
        };

        template<typename T>
        struct API Message
        {
            Header<T> header{};
            std::vector<uint8_t> body;

            size_t size() const { return body.size(); };
            std::string toString() const
            {
                std::stringstream ss;
                ss << "ID: " << int(header.id) << " Size: " << header.size;
                return ss.str();
            };

            template<typename DataType>
            void push(const DataType& data)
            {
                // Check that the type is copyable
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be added to vector");

                // Save current vector size
                size_t i = body.size();

                // Resize vector
                body.resize(body.size() + sizeof(DataType));

                // Copy the data into the newly allocated space
                std::memcpy(body.data() + i, &data, sizeof(DataType));

                // Recalculate the message size
                header.size = body.size();
            };

            template<typename DataType>
            DataType& pop_back()
            {
                // Check that the type is copyable
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");

                // Calculate the start of the data
                size_t i = body.size() - sizeof(DataType);

                // Copy the data out
                DataType& data;
                std::memcpy(&data, body.data() + i, sizeof(DataType));

                // Shrink the vector
                body.resize(i);

                // Recalculate the message size
                header.size = body.size();

                // Return data
                return data;
            };
        };

        // Forward declare Connection class
        template<typename T>
        class Connection;

        template<typename T>
        struct OwnedMessage
        {
            std::shared_ptr<Connection<T>> remote = nullptr;
            Message<T> msg;

            std::string toString() const
            {
                return msg.toString();
            };
        };
    }
}