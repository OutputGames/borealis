#if !defined(ENEMY_H)
#define ENEMY_H
#include "borealis/ecs/entity.hpp"
#include "borealis/gfx/model.hpp"
#include "borealis/gfx/texture.hpp"

struct EnemyController : brl::EcsEntity
{

    EnemyController();
    void update() override;
    void handleAttack(glm::vec3 dir, float power);

    static std::vector<EnemyController*> cachedEnemies;

    bool isGuarding;

private:
    brl::GfxMaterial* material = nullptr;
    brl::GfxMeshRenderer* renderer;

    brl::GfxTexture2dArray *idleSprites, *walkSprites, *attackSprites, *guardSprites;

    glm::vec2 velocity = glm::vec2{0};
    float attackDelay = 3.0f;
    float attackTimer = 0;

    float startAttackDistance = 0;


    brl::UtilCoroutine AttackCoroutine(glm::vec3 dir, float power);


};

#endif // ENEMY_H
