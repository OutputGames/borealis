#if !defined(MAP_H)
#define MAP_H

#include <borealis/ecs/entity.hpp>
#include <borealis/gfx/gfx.hpp>

#include "actor.h"
#include "borealis/gfx/sprite.hpp"
#include "borealis/util/input.hpp"

struct PlayerController;
struct MapChunk;
class FastNoiseLite;
struct MapController;

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

#define MAP_CHUNK_SIZE 32
#define MAP_MAX_HEIGHT 16
#define MAP_BLOCK_SPACING 1.5f
#define MAP_CHUNK_BLOCK_COUNT MAP_CHUNK_SIZE*MAP_CHUNK_SIZE * MAP_MAX_HEIGHT

#define _3DTO1D(x, y, z) x + MAP_CHUNK_SIZE *(y + MAP_MAX_HEIGHT * z)

struct MapBlock
{
    int type = 0;

private:
    friend MapChunk;
    int relativeBlocks = 0;
};


struct MapChunk : brl::EcsEntity
{


    MapBlock* blocks = new MapBlock[MAP_CHUNK_BLOCK_COUNT];

    MapChunk();

    void lateUpdate() override;

private:
    friend MapController;

    int mapIndexX, mapIndexY;
    MapController* _mapController;

    int* heights = new int[MAP_CHUNK_SIZE * MAP_CHUNK_SIZE];

    MapBlock* GetBlock(int x, int y, int z)
    {
        return &blocks[_3DTO1D(x, y, z)];
    }

    void initialize(FastNoiseLite noiseGen);
    void postInitializeCheck();

};

struct BlockData
{
    std::vector<brl::GfxMaterial*> materials;
    int meshType;
    glm::mat4 offset = glm::mat4(1.0);

    void initialize();
    glm::mat4 getCompositeOffset();
    BlockData* clone();

private:
    glm::mat4 compositeOffset = glm::mat4(1.0);
};

using MapBlockModel = std::vector<brl::GfxMesh*>;

struct MapController : brl::EcsEntity
{

    MapController() = default;
    void loadMap();

    void update() override;

    BlockData& GetBlockData(int index)
    {
        return dataBlocks[index];
    }

    int GetHeight(int x, int z);

private:
    friend MapChunk;
    friend ActorBehaviour;
    MapChunk** chunks;
    int chunkCountX, chunkCountY;

    std::vector<BlockData> dataBlocks;
    std::vector<MapBlockModel> blockModels;

    static MapController* Instance;


    MapChunk* GetChunk(int x, int y);

    MapBlock* GetBlock(int x, int y, int z)
    {
        int chunkX = x / MAP_CHUNK_SIZE;
        int chunkZ = z / MAP_CHUNK_SIZE;

        int relativeChunkX = x % MAP_CHUNK_SIZE;
        int relativeChunkZ = z % MAP_CHUNK_SIZE;

        return GetChunk(chunkX, chunkZ)->GetBlock(relativeChunkX, y, relativeChunkZ);

    }


};

#endif // MAP_H
