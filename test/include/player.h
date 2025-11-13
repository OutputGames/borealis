#if !defined(PLAYER_H)
#define PLAYER_H
#include <borealis/ecs/entity.hpp>
#include <borealis/gfx/gfx.hpp>

#include "borealis/gfx/sprite.hpp"
#include "borealis/util/input.hpp"

struct PlayerEntity : brl::EcsEntity
{
    PlayerEntity();

    virtual void handleAttack(glm::vec3 dir, float power);

    bool isGuarding = false;

    static std::vector<PlayerEntity*> cachedEntities;
};

struct PlayerController : PlayerEntity
{

    PlayerController();
    void update() override;
    void handleAttack(glm::vec3 dir, float power) override;

private:
    brl::GfxMaterial* material = nullptr;
    brl::GfxMeshRenderer* renderer;

    brl::GfxTexture2dArray *idleSprites, *walkSprites, *attackSprites, *guardSprites;


    brl::UtilCoroutine AttackCoroutine(glm::vec3 dir, float power);

};

#endif // PLAYER_H
