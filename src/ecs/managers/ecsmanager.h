#pragma once

#include "../../macros.h"
#include "../common.h"
#include "entitymanager.h"
#include "componentmanager.h"
#include "systemmanager.h"
#include "../entity.h"

// Engine Components and Systems
#include "../components/include.h"
#include "../systems/include.h"

namespace Alkahest
{
    class NOT_EXPORTED ECSManager
    {
    public:
        // registerComponent and registerSystem are defined here
        // so the can be used in the init function with no issues
        template<typename T>
        void registerComponent()
        {
            m_componentManager->registerComponent<T>();
        }

        template<typename T>
        void registerSystem()
        {
            m_systemManager->registerSystem<T>();
        }

        void init()
        {
            m_entityManager = std::make_unique<EntityManager>();
            m_componentManager = std::make_unique<ComponentManager>();
            m_systemManager = std::make_unique<SystemManager>();

            // Register all engine-defined components and systems
            registerComponent<Components::TransformComponent>();
        };

        Entity createEntity()
        {
            return m_entityManager->createEntity();
        };

        void destroyEntity(Entity e)
        {
            m_entityManager->destroyEntity(e);
            m_componentManager->EntityDestroyed(e);
            m_systemManager->EntityDestroyed(e);
        };

        template<typename T>
        void addComponentToEntity(Entity e, T component)
        {
            m_componentManager->addComponentToEntity(e, component);
            
            ALKAHEST_MASK_TYPE mask = m_entityManager->getMask(e);
            mask |= BIT(m_componentManager->getComponentType<T>());
            m_entityManager->setMask(e, mask);

            m_systemManager->EntityMaskChanged(e, mask);
        };

        template<typename T>
        void removeComponentFromEntity(Entity e, T component)
        {
            m_componentManager->removeComponentFromEntity(e, component);

            ALKAHEST_MASK_TYPE mask = m_entityManager->getMask(e);
            ALKAHEST_COMPONENT_ID_TYPE componentID = m_componentManager->
                getComponentType<T>();
            mask |= BIT(componentID);
            mask ^= BIT(componentID);
            m_entityManager->setMask(e, mask);

            m_systemManager->EntityMaskChanged(e, mask);
        };

        template<typename T>
        T& getComponent(Entity e)
        {
            return m_componentManager->getComponent<T>(e);
        };

        template<typename T>
        ALKAHEST_COMPONENT_ID_TYPE getComponentType()
        {
            return m_componentManager->getComponentType<T>();
        };

        template<typename T>
        void setSystemMask(ALKAHEST_MASK_TYPE mask)
        {
            m_systemManager->setMask<T>(mask);
        };
    private:
        std::unique_ptr<EntityManager> m_entityManager;
        std::unique_ptr<ComponentManager> m_componentManager;
        std::unique_ptr<SystemManager> m_systemManager;
    };

    extern ECSManager gECSManager;

    namespace Components
    {
        template<typename T>
        void _register()
        {
            gECSManager.registerComponent<T>();
        }
    }

    namespace Systems
    {
        template<typename T>
        void _register()
        {
            gECSManager.registerSystem<T>();
        }

        template<typename T>
        void setMask(ALKAHEST_MASK_TYPE mask)
        {
            gECSManager.setSystemMask<T>(mask);
        }
    }
}
