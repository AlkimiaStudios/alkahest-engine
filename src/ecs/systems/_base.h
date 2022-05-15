#pragma once

#include "../../macros.h"
#include "../common.h"
#include "../entity.h"

namespace Alkahest
{
    class API System
    {
    public:
        System() {};
        virtual ~System() {};
        virtual void process(Entity e) = 0;
    public:
        virtual void processAll() final
        {
            for (auto const& pair : m_entities)
            {
                auto const& entity = pair.second;
                process(entity);
            }
        };

        virtual void addEntity(Entity e) final
        {
            m_entities.insert({ e.ID, e });
        };

        virtual void removeEntity(Entity e) final
        {
            m_entities.erase(e.ID);
        };

        virtual void EntityDestroyed(Entity e) final
        {
            if (m_entities.find(e.ID) != m_entities.end())
                removeEntity(e);
        };
    protected:
        std::unordered_map<ALKAHEST_ENTITY_ID_TYPE, Entity> m_entities{};
    };

    namespace Systems
    {
        // This function will be defined and implemented within the
        // SystemManager class. It is only listed here for the sake
        // of documentation.
        //template<typename T>
        //static void _register();
    }
}
