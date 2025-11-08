#include "borealis/ecs/entity.hpp"

brl::EcsEngine* brl::EcsEngine::instance;

glm::vec3 brl::EcsEntity::position()
{
    glm::vec3 p = localPosition;

    if (parent)
        p += parent->position();

    return p;
}

glm::quat brl::EcsEntity::rotation()
{
    glm::quat p = localRotation;

    if (parent)
        p *= parent->rotation();

    return p;
}

glm::vec3 brl::EcsEntity::scale()
{
    glm::vec3 p = localScale;

    if (parent)
        p *= parent->scale();

    return p;
}

void brl::EcsEntity::destroy()
{

    EcsEngine::instance->destroyEntity(this);

    for (auto child : children)
    {
        child->destroy();
    }
}

void brl::EcsEntity::setActive(bool active)
{
    if (this->active != active)
    {
        if (active)
        {
            onEnable();
        }
        else
        {
            onDisable();
        }
    }

    this->active = active;

}

bool brl::EcsEntity::isSelfActive()
{
    return active;
}

bool brl::EcsEntity::isGlobalActive()
{
    if (parent)
    {
        return parent->isGlobalActive() && isSelfActive();
    }
    return isSelfActive();
}

brl::EcsEntity::EcsEntity()
{
    EcsEngine::instance->entities.push_back(this);
}

void brl::EcsEntity::awake()
{
}

void brl::EcsEntity::start()
{
}

void brl::EcsEntity::earlyUpdate()
{
}

void brl::EcsEntity::update()
{
}

void brl::EcsEntity::lateUpdate()
{
}

void brl::EcsEntity::fixedUpdate()
{
}

void brl::EcsEntity::onEnable()
{
}

void brl::EcsEntity::onDisable()
{
}

void brl::EcsEntity::onDestroy()
{

}

void brl::EcsEntity::calculateTransform()
{

}

void brl::EcsEngine::destroyEntity(EcsEntity* e)
{
    std::erase(entities, e);
}

brl::EcsEngine::EcsEngine()
{
    instance = this;
}

void brl::EcsEngine::update()
{
    // early update
    for (auto entity : entities)
    {
        if (entity->isGlobalActive())
            entity->earlyUpdate();
    }

    // main update
    for (auto entity : entities)
    {
        if (entity->isGlobalActive())
            entity->update();
    }

    // late update
    for (auto entity : entities)
    {
        if (entity->isGlobalActive())
            entity->lateUpdate();
    }
}
