#pragma once

#include "../../macros.h"
#include "../common.h"
#include "../entity.h"

namespace Alkahest
{
    class NOT_EXPORTED EntityManager
    {
    public:
        EntityManager()
        {
            m_liveEntityCount = 0;

            for (ALKAHEST_ENTITY_ID_TYPE i = 0; i < ALKAHEST_ENTITY_LIMIT; i++)
            {
                m_availableEntities.push(i);
            }
        };
        ~EntityManager() {};

        Entity createEntity()
        {
            ALKAHEST_ENTITY_ID_TYPE id = m_availableEntities.front();
            m_availableEntities.pop();
            Entity e(id, 0);
            m_liveEntityCount++;

            return e;
        };

        void destroyEntity(Entity e)
        {
            m_availableEntities.push(e.ID);
            m_liveEntityCount--;
        };

        ALKAHEST_MASK_TYPE getMask(Entity e)
        {
            return e.Mask;
        };
        void setMask(Entity e, ALKAHEST_MASK_TYPE mask)
        {
            e.Mask = mask;
        }
    private:
        ALKAHEST_ENTITY_ID_TYPE m_liveEntityCount{};
        std::queue<ALKAHEST_ENTITY_ID_TYPE> m_availableEntities{};
    };
}
