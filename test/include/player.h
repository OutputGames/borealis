#if !defined(PLAYER_H)
#define PLAYER_H
#include <borealis/ecs/entity.hpp>
#include <borealis/gfx/gfx.hpp>

#include "borealis/gfx/sprite.hpp"
#include "borealis/util/input.hpp"

 
struct PlayerController : brl::EcsEntity {

    PlayerController();
    void update();

private:

    brl::GfxMaterial* material = nullptr;
    brl::GfxMeshRenderer* renderer;

    brl::GfxTexture2dArray* idleSprites, *walkSprites, *attackSprites, *guardSprites;

};

#endif // PLAYER_H
