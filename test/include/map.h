#if !defined(MAP_H)
#define MAP_H

#include <borealis/ecs/entity.hpp>
#include <borealis/gfx/gfx.hpp>

#include "borealis/gfx/sprite.hpp"
#include "borealis/util/input.hpp"

struct MapObject : brl::EcsEntity {

    MapObject(brl::GfxMeshRenderer* renderer);

    void update();

private:

    brl::GfxMeshRenderer* renderer;

};

#endif // MAP_H
