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
        void registerComponentImpl()
        {
            m_componentManager->registerComponent<T>();
        }

        template<typename T>
        void registerSystemImpl()
        {
            m_systemManager->registerSystem<T>();
        }

        void initImpl()
        {
            m_entityManager = std::make_unique<EntityManager>();
            m_componentManager = std::make_unique<ComponentManager>();
            m_systemManager = std::make_unique<SystemManager>();

            // Register all engine-defined components and systems
            registerComponentImpl<Components::TransformComponent>();
        };

        Entity createEntityImpl()
        {
            return m_entityManager->createEntity();
        };

        void destroyEntityImpl(Entity e)
        {
            m_entityManager->destroyEntity(e);
            m_componentManager->EntityDestroyed(e);
            m_systemManager->EntityDestroyed(e);
        };

        template<typename T>
        void addComponentToEntityImpl(Entity e, T component)
        {
            m_componentManager->addComponentToEntity(e, component);
            
            ALKAHEST_MASK_TYPE mask = m_entityManager->getMask(e);
            mask |= BIT(m_componentManager->getComponentType<T>());
            m_entityManager->setMask(e, mask);

            m_systemManager->EntityMaskChanged(e, mask);
        };

        template<typename T>
        void removeComponentFromEntityImpl(Entity e)
        {
            m_componentManager->removeComponentFromEntity<T>(e);

            ALKAHEST_MASK_TYPE mask = m_entityManager->getMask(e);
            ALKAHEST_COMPONENT_ID_TYPE componentID = m_componentManager->
                getComponentType<T>();
            mask |= BIT(componentID);
            mask ^= BIT(componentID);
            m_entityManager->setMask(e, mask);

            m_systemManager->EntityMaskChanged(e, mask);
        };

        template<typename T>
        T& getComponentImpl(Entity e)
        {
            return m_componentManager->getComponent<T>(e);
        };

        template<typename T>
        ALKAHEST_COMPONENT_ID_TYPE getComponentTypeImpl()
        {
            return m_componentManager->getComponentType<T>();
        };

        template<typename T>
        void setSystemMaskImpl(ALKAHEST_MASK_TYPE mask)
        {
            m_systemManager->setMask<T>(mask);
        };
    private:
        static ECSManager& getInstance()
        {
            static ECSManager m;
            std::cout << "ECSManager& address: " << &m << std::endl;
            return m;
        }
    public: 
        static void init() { getInstance().initImpl(); };
        static Entity createEntity() { return getInstance().createEntityImpl(); };
        static void destroyEntity(Entity e) { getInstance().destroyEntityImpl(e); };
    public:
        template<typename T>
        static void registerComponent() { getInstance().registerComponentImpl<T>(); };
        
        template<typename T>
        static void registerSystem() { getInstance().registerSystemImpl<T>(); };

        template<typename T>
        static void addComponentToEntity(Entity e, T component)
            { getInstance().addComponentToEntityImpl(e, component); };

        template<typename T>
        static void removeComponentFromEntity(Entity e)
            { getInstance().removeComponentFromEntityImpl<T>(e); };
        
        template<typename T>
        static T& getComponent(Entity e) { return getInstance().getComponentImpl<T>(e); };

        template<typename T>
        static ALKAHEST_COMPONENT_ID_TYPE getComponentType()
            { return getInstance().getComponentTypeImpl<T>(); };
        
        template<typename T>
        static void setSystemMask(ALKAHEST_MASK_TYPE mask) { getInstance().setSystemMaskImpl<T>(mask); };
    private:
        std::unique_ptr<EntityManager> m_entityManager;
        std::unique_ptr<ComponentManager> m_componentManager;
        std::unique_ptr<SystemManager> m_systemManager;
    };

    template<typename T>
    void Entity::addComponent(T& c)
    {
        ECSManager::addComponentToEntity<T>(*this, c);
    }

    template<typename T>
    T& Entity::getComponent()
    {
        return ECSManager::getComponent<T>(*this);
    }

    template<typename T>
    void Entity::removeComponent()
    {
        ECSManager::removeComponentFromEntity<T>(*this);
    }

    namespace Components
    {
        template<typename T>
        void _register()
        {
            ECSManager::registerComponent<T>();
        }
    }

    namespace Systems
    {
        template<typename T>
        void _register()
        {
            ECSManager::registerSystem<T>();
        }

        template<typename T>
        void setMask(ALKAHEST_MASK_TYPE mask)
        {
            ECSManager::setSystemMask<T>(mask);
        }
    }
}
