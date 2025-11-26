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

struct MiniMapController : brl::EcsEntity
{

    MiniMapController() = default;

    void loadMap();

    void update() override;

};

#define MAP_CHUNK_SIZE 16

struct MapChunk : brl::EcsEntity
{
    struct MapBlock
    {
        int type = 0;
    };

    MapBlock* blocks = new MapBlock[pow(MAP_CHUNK_SIZE, 3)];

    MapChunk();

    void lateUpdate() override;
};

struct MapController : brl::EcsEntity
{

    MapController() = default;
    void loadMap();

    void update() override;

};

#endif // MAP_H
