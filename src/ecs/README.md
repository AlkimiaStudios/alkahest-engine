# Alkahest Entity-Component-System Setup

### Available Types

- `Alkahest::Entity` - Base entity type that essentially maps to an ID
- `Alkahest::Component` - Base component type that can be derived by the user to extend ECS functionality
- `Alkahest::Components::<ComponentType>` - Engine-defined components that can are available to the user
- `Alkahest::System` - Base system type that can be derived by the user to extend ECS functionality
- `Alkahest::Systems::<SystemType>` - Engine-defined systems that are available to the user

### Hidden Classes
- `EntityManager` - Tracks existing entities and initializes new ones, as well as tracking which entities have which components
- `ComponentManager` - Tracks all component types and manages arrays of data for existing components
- `SystemManager` - Tracks all system types and notifies them of changes
- `ECSManager` - Maintains the Entity, Component, and Systems Managers and coordinates interactivity between them

## Use

### Creating Entities

When creating a new Entity, call the `Entity::create` method.

```cxx
Entity myEntity = Entity::create();
```

The created Entity will automatically be registered with the `EntityManager`.

### Creating Components and Systems

When creating a new Component/System, call the corresponding parent's `register` method.

```cxx
struct MyCustomComponent : public Component
{
    int attributeA;
    float attributeB;
};

Component::register(MyCustomComponent);

class MyCustomSystem : public System
{
public:
    void perform(Entity e)
    {
        // Do something with data
    }
};

System::register(MyCustomSystem);
```

### Adding Components to Entities

To add a component to an entity, use the overloaded `+=` operator:

```cxx
myEntity += MyCustomComponent;
```

The component data can then be edited using the `Entity::getComponent<T>` method:

```cxx
myEntity.getComponent<MyCustomComponent>().attributeA = 0;
```

> ### Developer Note:
> When editing a single attribute of a component, doing the above should pose no issue. However, it may be the case that successive calls to edit individual attributes on a single component could cause a performance issue. If editing multiple attributes on the same component, it is preferred to create a new version of the component with the correct data and reassign the component only once:
> ```cxx
> MyCustomComponent updated(43, 2.21f);
> myEntity.getComponent<MyCustomComponent>() = updated;
> ```
> This approach should theoretically save on a few CPU cycles.

## Requirements

### Entities

Entities should always function only as a container for an ID. The `Entity` class may include some additional helper methods, but they should not add any data or processing to the ECS flow directly.

### Components

Components should be `struct` objects that contain only data. The data should be of a simple (read: serializeable) type, and there should be no logic contained in the Components.

### Systems

Systems should contain a single overridden function from the base `System` type: `perform`. The `perform` function takes a single argument, an Entity that contains the necessary Component data for the System to perform its logic. The derived System may also contain any helper functions that are necessary to complete its operation, but should contain no data.