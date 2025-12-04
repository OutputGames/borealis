#if !defined(PLAYER_H)
#define PLAYER_H
#include <borealis/ecs/entity.hpp>
#include <borealis/gfx/gfx.hpp>

#include "actor.h"
#include "borealis/gfx/sprite.hpp"
#include "borealis/util/input.hpp"

enum EnemyTeam : int;

struct PlayerEntity : ActorBehaviour
{
    PlayerEntity();

    void handleAttack(glm::vec3 dir, float power) override;

    bool isGuarding = false;
    EnemyTeam Team;


    static std::vector<PlayerEntity*> cachedEntities;
};

struct PlayerController : PlayerEntity
{

    PlayerController(EnemyTeam team);
    void update() override;
    void handleAttack(glm::vec3 dir, float power) override;

    brl::GfxImage* healthBarImage;


private:
    brl::GfxMaterial* material = nullptr;
    brl::GfxMeshRenderer* renderer;

    brl::GfxTexture2dArray *idleSprites, *walkSprites, *attackSprites, *guardSprites;


    brl::UtilCoroutine AttackCoroutine(glm::vec3 dir, float power);

};

#endif // PLAYER_H
