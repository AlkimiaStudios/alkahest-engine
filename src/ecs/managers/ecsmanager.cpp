#include "ecsmanager.h"

namespace Alkahest
{
    Entity Entity::create()
    {
        return ECSManager::createEntity();
    }
}
