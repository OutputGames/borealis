#if !defined(MAP_H)
#define MAP_H

#include <borealis/ecs/entity.hpp>
#include <borealis/gfx/gfx.hpp>

#include "borealis/gfx/sprite.hpp"
#include "borealis/util/input.hpp"

struct MapObject : brl::EcsEntity
{

    MapObject(brl::GfxMeshRenderer* renderer);

    void update() override;

private:
    brl::GfxMeshRenderer* renderer;

};

struct MapController : brl::EcsEntity
{

    MapController() = default;

    void loadMap();

    void update() override;

};

#endif // MAP_H
