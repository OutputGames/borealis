#if !defined(ENEMY_H)
#define ENEMY_H


#include "borealis/ecs/entity.hpp"
#include "borealis/gfx/model.hpp"
#include "borealis/gfx/texture.hpp"
#include "actor.h"

enum EnemyTeam
{
    Red = 0,
    Blue,
    Yellow,
    Black
};

struct EnemyController : ActorBehaviour
{

    EnemyController(EnemyTeam t);
    void update() override;
    void handleAttack(glm::vec3 dir, float power) override;
    void onDestroy() override;

    static std::vector<EnemyController*> cachedEnemies;

    bool isGuarding;
    EnemyTeam Team = Red;

private:
    brl::GfxMaterial* material = nullptr;
    brl::GfxMeshRenderer* renderer;

    HealthBarBehavior* healthBar;

    static brl::GfxTexture2dArray *idleSprites, *walkSprites, *attackSprites, *guardSprites;

    glm::vec2 velocity = glm::vec2{0};
    float attackDelay = 3.0f;
    float attackTimer = 0;

    float startAttackDistance = 0;

    void onDeath() override;

    brl::UtilCoroutine AttackCoroutine(glm::vec3 dir, float power);
    brl::UtilCoroutine DeathCoroutine();


};

struct EnemySpawner : ActorBehaviour
{
    EnemySpawner(EnemyTeam team);
    void start() override;
    void update() override;

    float radius = 3.0f;
    int count = 3;
    EnemyTeam team = Red;
};

#endif // ENEMY_H
