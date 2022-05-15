#pragma once

#include "../../macros.h"
#include "../common.h"
#include "../../sys/log/log.h"

namespace Alkahest
{
    class NOT_EXPORTED BaseComponentArray
    {
    public:
        virtual ~BaseComponentArray() = default;
        virtual void EntityDestroyed(Entity e) = 0;
    };

    template<typename T>
    class NOT_EXPORTED ComponentArray : public BaseComponentArray
    {
    public:
        void insertData(Entity e, T component)
        {
            size_t newIndex = m_size;
            m_mEntityToIndex[e.ID] = newIndex;
            m_mIndexToEntity[newIndex] = e.ID;
            m_componentArray[newIndex] = component;
            m_size++;
        };

        void removeData(Entity e)
        {
            // Copy the element at the end of the array into the deleted
            // element's place to maintain the density of the array
            size_t indexOfRemovedEntity = m_mEntityToIndex[e.ID];
            size_t indexOfLastElement = m_size - 1;
            m_componentArray[indexOfRemovedEntity] =
                m_componentArray[indexOfLastElement];

            // Update the maps so for the new indices
            ALKAHEST_ENTITY_ID_TYPE id = m_mIndexToEntity[indexOfLastElement];
            m_mEntityToIndex[id] = indexOfRemovedEntity;
            m_mIndexToEntity[indexOfRemovedEntity] = id;

            m_mEntityToIndex.erase(e.ID);
            m_mIndexToEntity.erase(indexOfLastElement);

            m_size--;
        };

        T& getData(Entity e)
        {
            if (m_mEntityToIndex.find(e.ID) == m_mEntityToIndex.end())
            {
                logError("Attempting to retrieve non-existent component!");
                //TODO: Create relevant ECS errors
                throw AlkahestError{};
            }

            return m_componentArray[m_mEntityToIndex[e.ID]];
        };

        void EntityDestroyed(Entity e) override
        {
            if (m_mEntityToIndex.find(e.ID) != m_mEntityToIndex.end())
                removeData(e);
        };
    private:
        std::array<T, ALKAHEST_ENTITY_LIMIT> m_componentArray{};
        std::unordered_map<ALKAHEST_ENTITY_ID_TYPE, size_t> m_mEntityToIndex{};
        std::unordered_map<size_t, ALKAHEST_ENTITY_ID_TYPE> m_mIndexToEntity{};
        size_t m_size{};
    };

    class NOT_EXPORTED ComponentManager
    {
    public:
        template<typename T>
        void registerComponent()
        {
            const char* typeName = typeid(T).name();
            m_componentTypes.insert({ typeName, m_nextComponentType++ });
            m_componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });
        };

        template<typename T>
        ALKAHEST_COMPONENT_ID_TYPE getComponentType()
        {
            const char* typeName = typeid(T).name();
            if (m_componentTypes.find(typeName) == m_componentTypes.end())
            {
                logError("Component not registered before use!");
                throw AlkahestError{};
            }
            return m_componentTypes[typeName];
        };

        template<typename T>
        void addComponentToEntity(Entity e, T component)
        {
            getComponentArray<T>()->insertData(e, component);
        };

        template<typename T>
        T& getComponent(Entity e)
        {
            getComponentArray<T>()->getData(e);
        };

        template<typename T>
        void removeComponentFromEntity(Entity e)
        {
            getComponentArray<T>()->removeData(e);
        };

        void EntityDestroyed(Entity e)
        {
            for (auto const& pair : m_componentArrays)
            {
                auto const& cArr = pair.second;
                cArr->EntityDestroyed(e);
            }
        };
    private:
        std::unordered_map<const char*, ALKAHEST_COMPONENT_ID_TYPE> m_componentTypes{};
        std::unordered_map<const char*, std::shared_ptr<BaseComponentArray>> m_componentArrays{};
        ALKAHEST_COMPONENT_ID_TYPE m_nextComponentType{};

        // Convenience function to get the array for a given type
        template<typename T>
        std::shared_ptr<ComponentArray<T>> getComponentArray()
        {
            const char* typeName = typeid(T).name();
            if (m_componentTypes.find(typeName) == m_componentTypes.end())
            {
                logError("Component not registered before use!");
                throw AlkahestError{};
            }
            return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeName]);
        };
    };
}
