#if !defined(ACTOR_H)
#define ACTOR_H

#include "borealis/ecs/entity.hpp"
#include "borealis/gfx/model.hpp"
#include "borealis/gfx/texture.hpp"

struct ActorBehaviour : brl::EcsEntity
{

    ActorBehaviour();
    void update() override;
    virtual void handleAttack(glm::vec3 dir, float power);

protected:
    float health = 1.f;
    bool isAlive = true;

    virtual void onDeath();

};

struct HealthBarBehavior : brl::EcsEntity
{

    HealthBarBehavior();
    void update() override;

    float health = 1.0f;

private:
    brl::GfxMaterial* material = nullptr;
    brl::GfxMeshRenderer* renderer;

};


#endif // ACTOR_H
