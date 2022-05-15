#pragma once

#include "../../macros.h"
#include "../common.h"
#include "../entity.h"
#include "../systems/_base.h"

namespace Alkahest
{
    class NOT_EXPORTED SystemManager
    {
    public:
        template<typename T>
        void registerSystem()
        {
            const char* typeName = typeid(T).name();

            auto system = std::make_shared<T>();
            m_systems.insert({ typeName, system });
        };

        template<typename T>
        void setMask(ALKAHEST_MASK_TYPE mask)
        {
            const char* typeName = typeid(T).name();

            m_masks.insert({ typeName, mask });
        };
        
        void EntityDestroyed(Entity e)
        {
            for (auto const& pair : m_systems)
            {
                auto const& system = pair.second;
                system->EntityDestroyed(e);
            }
        };

        void EntityMaskChanged(Entity e, ALKAHEST_MASK_TYPE mask)
        {
            for (auto const& pair : m_systems)
            {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemMask = m_masks[type];

                if ((mask & systemMask) == systemMask)
                    system->addEntity(e);
                else
                    system->removeEntity(e);
            }
        };
    private:
        std::unordered_map<const char*, ALKAHEST_MASK_TYPE> m_masks{};
        std::unordered_map<const char*, std::shared_ptr<System>> m_systems{};
    };
}
