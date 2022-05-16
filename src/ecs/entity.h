#pragma once

#include "../macros.h"
#include "common.h"
#include "components/_base.h"

namespace Alkahest
{
    // Forward declare EntityManager class
    class EntityManager;
    
    class API Entity
    {
    public:
        friend class EntityManager;
        ~Entity() {};

        ALKAHEST_ENTITY_ID_TYPE ID;
        ALKAHEST_MASK_TYPE Mask;

        template<typename T>
        void addComponent(T& c);

        template<typename T>
        T& getComponent();

        template<typename T>
        void removeComponent();
    public:
        static Entity create();
    private:
        Entity(ALKAHEST_ENTITY_ID_TYPE id, ALKAHEST_MASK_TYPE mask) :
            ID(id), Mask(mask) {};
    };
}
