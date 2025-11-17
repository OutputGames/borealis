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

void brl::EcsEntity::lookAt(glm::vec3 point, glm::vec3 worldUp)
{
    glm::vec3 forward = normalize(point - position());
    glm::vec3 right = normalize(cross(worldUp, forward));
    glm::vec3 up = cross(forward, right);

    // Build rotation matrix (like Unity’s Transform rotation basis)
    glm::mat3 rotationMatrix(right, up, forward);

    // Convert matrix to quaternion
    localRotation = quat_cast(rotationMatrix);
}

void brl::EcsEntity::setEulerAngles(glm::vec3 euler) { localRotation = glm::quat(radians(euler)); }

void brl::EcsEntity::destroy()
{

    EcsEngine::instance->destroyEntity(this);

    for (auto child : children)
    {
        child->destroy();
    }

    onDestroy();
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

void brl::EcsEntity::setParent(EcsEntity* e)
{
    if (parent)
    {
        parent->children.erase(std::find(parent->children.begin(), parent->children.end(), this));
    }

    parent = e;
    parent->children.push_back(this);
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

glm::mat4 brl::EcsEntity::calculateTransform()
{
    glm::mat4 t(1.0);

    t = translate(t, position());
    t *= toMat4(rotation());
    t = glm::scale(t, scale());

    return t;
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
