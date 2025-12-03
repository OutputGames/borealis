#include "map.h"
#include "FastNoiseLite.h"
#include "enemy.h"
#include "borealis/util/random.hpp"

MapObject::MapObject(brl::GfxMeshRenderer* renderer)
{
    this->renderer = renderer;
    renderer->setParent(this);
}

void MapObject::update()
{
    renderer->localPosition = glm::vec3{0, 0, 0};
    renderer->lookAt(brl::GfxCamera::mainCamera->position);

}

void MiniMapController::loadMap()
{
    int* mapData;
    mapData = new int[]{
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    };

    int mapWidth = 10;
    int mapHeight = 10;
    float spacing = 2.6f;

    auto shaderBins = new brl::GfxShader*[2];


    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/map/map.vert"));
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/map/map.frag"));
    auto defaultShader = new brl::GfxShaderProgram(shaderBins, 2, true);

    for (int x = -mapWidth / 2; x < mapWidth / 2; ++x)
    {
        for (int y = -mapHeight / 2; y < mapHeight / 2; ++y)
        {
            int index = ((y + mapHeight / 2) * mapWidth) + (x + mapWidth / 2);
            int value = mapData[index];

            for (int i = 0; i <= value; ++i)
            {
                brl::GfxModel* block = brl::GfxModel::loadModel("models/block.glb");
                for (auto material : block->materials)
                {
                    material->reloadShader(defaultShader);
                }


                float blockX = x * 2.2f;
                float blockY = y * spacing;

                if (abs(x) % 2 == 1)
                {
                    blockY += 1.3f;
                }

                auto blockEntity = block->createEntity();
                blockEntity->localPosition = {blockX, 0 + (1.5f * i), blockY};
                blockEntity->setEulerAngles({0, 180, 0});
                blockEntity->localScale = glm::vec3(1.5);
            }
        }
    }

}

void MiniMapController::update()
{
    EcsEntity::update();
}

MapChunk::MapChunk()
{

}

void MapChunk::lateUpdate()
{
    EcsEntity::lateUpdate();

    localPosition.x = ((mapIndexX - _mapController->chunkCountX / 2) * MAP_CHUNK_SIZE) * MAP_BLOCK_SPACING;
    localPosition.z = ((mapIndexY - _mapController->chunkCountY / 2) * MAP_CHUNK_SIZE) * MAP_BLOCK_SPACING;

    glm::mat4 transform = calculateTransform();
    int chunkIndex = (mapIndexX * _mapController->chunkCountY) + mapIndexY;

    auto baseTransform = new glm::mat4[_mapController->dataBlocks.size()];

    for (int i = 0; i < _mapController->dataBlocks.size(); ++i)
    {
        baseTransform[i] = transform * _mapController->dataBlocks[i].getCompositeOffset();
    }


    for (int x = 0; x < MAP_CHUNK_SIZE; ++x)
    {
        for (int z = 0; z < MAP_CHUNK_SIZE; ++z)
        {

            for (int y = 0; y < MAP_MAX_HEIGHT; ++y)
            {
                auto block = GetBlock(x, y, z);


                if (block->type != 0 && block->relativeBlocks != 1)
                {
                    float spacing = MAP_BLOCK_SPACING;
                    float blockX = x * spacing;
                    float blockZ = z * spacing;
                    float blockY = -(spacing / 2) + ((spacing / 2) * y);


                    const auto& dataBlock = _mapController->dataBlocks[block->type - 1];

                    glm::mat4 blockTransform = baseTransform[block->type - 1];
                    blockTransform[3][0] += blockX;
                    blockTransform[3][1] += blockY;
                    blockTransform[3][2] += blockZ;

                    for (auto mesh : _mapController->blockModels[dataBlock.meshType])
                    {
                        for (int i = 0; i < mesh->GetSubMeshCount(); ++i)
                        {
                            auto subMesh = mesh->GetSubMesh(i);
                            brl::GfxEngine::instance->insertCall(dataBlock.materials[i],
                                                                 subMesh->buffer, blockTransform,
                                                                 chunkIndex);
                        }
                    }
                }
            }
        }
    }

    delete[] baseTransform;


}

void MapChunk::initialize(FastNoiseLite noiseGen)
{
    for (int x = 0; x < MAP_CHUNK_SIZE; ++x)
    {
        for (int z = 0; z < MAP_CHUNK_SIZE; ++z)
        {
            int height = (noiseGen.GetNoise(static_cast<float>(x + (mapIndexX * MAP_CHUNK_SIZE)),
                                            static_cast<float>(z + (mapIndexY * MAP_CHUNK_SIZE))) * 0.5 + 0.5) * 8.f;

            height = glm::clamp(height, 0, MAP_MAX_HEIGHT);


            heights[(x * MAP_CHUNK_SIZE) + z] = height;

            for (int y = 0; y < MAP_MAX_HEIGHT; ++y)
            {
                auto block = MapBlock{};

                if (y <= height)
                {
                    block.type = 1;

                    if (y < height)
                    {
                        block.relativeBlocks = 1;
                    }
                }
                else if (y == height + 1)
                {
                    if (brl::random(0, 10) > 9)
                    {
                        block.type = 2;
                    }
                    else if (brl::random(0, 100) > 95)
                    {
                        block.type = 3;
                    }
                }

                blocks[_3DTO1D(x, y, z)] = block;
            }
        }
    }

}

void MapChunk::postInitializeCheck()
{

}

void BlockData::initialize()
{
    compositeOffset = glm::scale(glm::vec3(MAP_BLOCK_SPACING / 2)) * offset;
}

glm::mat4 BlockData::getCompositeOffset()
{
    return compositeOffset;
}

BlockData* BlockData::clone()
{
    auto newBlock = new BlockData(*this);

    for (int i = 0; i < newBlock->materials.size(); ++i)
    {
        newBlock->materials[i] = new brl::GfxMaterial(*newBlock->materials[i]);
    }

    return newBlock;
}

void MapController::loadMap()
{
    Instance = this;

    float spacing = 1.0f;

    auto shaderBins = new brl::GfxShader*[2];

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(std::rand());
    //noise.SetFrequency(0.0005f);


    {
        shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/map/map.vert"));
        shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/map/map.frag"));
        auto defaultShader = new brl::GfxShaderProgram(shaderBins, 2, true);

        {
            auto blockModel = brl::GfxModel::loadModel("models/block_rect.glb");
            for (auto material : blockModel->materials)
            {
                material->reloadShader(defaultShader);
            }

            blockModels.push_back(blockModel->meshes);

            auto block = BlockData{};
            block.materials = blockModel->materials;
            block.meshType = 0;
            block.initialize();
            dataBlocks.push_back(block);
        }
    }

    {
        shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/map_crosshatch/map.vert"));
        shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/map_crosshatch/map.frag"));
        auto defaultShader = new brl::GfxShaderProgram(shaderBins, 2, true);

        {
            auto blockModel = brl::GfxModel::loadModel("models/block_crosshatch.glb");
            for (auto material : blockModel->materials)
            {
                material->reloadShader(defaultShader);
            }

            blockModels.push_back(blockModel->meshes);
            auto block = BlockData{};

            block.materials = blockModel->materials;
            block.meshType = 1;

            block.offset *= glm::toMat4(glm::quat(glm::radians(glm::vec3{0, 0, 0})));
            block.initialize();
            dataBlocks.push_back(block);
        }
    }

    blockModels.push_back({brl::GfxMesh::GetPrimitive(brl::QUAD)});

    {

        shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/map_object/vtx.glsl"));
        shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/map_object/anim_frg.glsl"));
        auto anim_shader = new brl::GfxShaderProgram(shaderBins, 2, true);

        auto material = new brl::GfxMaterial(anim_shader);
        material->setTexture("tex",
                             brl::GfxSprite::extractSpritesToArray(
                                 brl::GfxTexture2d::loadTexture("textures/Trees/Tree1.png"), 192, 256, true));


        auto block = BlockData{};

        block.materials = {material};
        block.meshType = 1;


        auto uni = material->getUniform("tex");

        float scale = 3.f;
        float scaleY = (scale * (uni.txValue->getHeight() / uni.txValue->getWidth())) * 2.0f;

        block.offset *= glm::translate(glm::vec3{0, (scaleY * 0.875f), 0});
        block.offset *= glm::toMat4(glm::quat(glm::radians(glm::vec3{0, 0, 0})));
        block.offset *= glm::scale(glm::vec3{scale, scaleY, scale});
        block.initialize();
        dataBlocks.push_back(block);
    }


    chunkCountX = 16;
    chunkCountY = 16;

    chunks = new MapChunk*[chunkCountX * chunkCountY];

    for (int x = 0; x < chunkCountX; ++x)
    {
        for (int y = 0; y < chunkCountY; ++y)
        {
            int chunkIndex = (x * chunkCountY) + y;


            auto chunk = new MapChunk();
            chunk->setParent(this);
            chunk->_mapController = this;

            chunks[chunkIndex] = chunk;


            chunk->mapIndexX = x;
            chunk->mapIndexY = y;

            chunk->initialize(noise);

        }
    }

    for (int x = 0; x < chunkCountX; ++x)
    {
        for (int y = 0; y < chunkCountY; ++y)
        {
            GetChunk(x, y)->postInitializeCheck();
        }
    }

    // place spawners

    int spawnersMax = 5;
    int spawnerCount = 0;
    for (int x = 0; x < chunkCountX*MAP_CHUNK_SIZE; ++x)
    {
        for (int y = 0; y < chunkCountY * MAP_CHUNK_SIZE; ++y)
        {
            if (brl::random(0, 1000) >= 999)
            {
                auto enemySpawner = new EnemySpawner();
            
                auto blockX = (x * MAP_BLOCK_SPACING) - ((chunkCountX*MAP_CHUNK_SIZE) / 2);
                ;
                auto blockZ = (y * MAP_BLOCK_SPACING) - ((chunkCountY * MAP_CHUNK_SIZE) / 2);

                enemySpawner->localPosition = {blockX,GetHeight(x,y),blockZ};

                spawnerCount++;
            }
            if (spawnerCount >= spawnersMax)
            {
                break;
            }
        }
        if (spawnerCount >= spawnersMax)
        {
            break;
        }
    }
}

void MapController::update()
{
    EcsEntity::update();

    auto camera = brl::GfxCamera::mainCamera;

    auto chunkX = ((camera->position.x / MAP_BLOCK_SPACING) / (MAP_CHUNK_SIZE)) + (chunkCountX / 2);
    auto chunkY = ((camera->position.z / MAP_BLOCK_SPACING) / (MAP_CHUNK_SIZE)) + (chunkCountY / 2);

    int radius = 1;

    for (int i = 0; i < chunkCountX * chunkCountY; ++i)
    {
        chunks[i]->active = false;
    }

    for (int x = -radius; x <= radius; ++x)
    {
        for (int y = -radius; y <= radius; ++y)
        {
            auto cx = chunkX + x;
            auto cy = chunkY + y;

            auto chunk = GetChunk(cx, cy);

            if (chunk)
                chunk->active = true;
        }
    }

}

int MapController::GetHeight(int x, int z)
{
    int chunkX = x / MAP_CHUNK_SIZE;
    int chunkZ = z / MAP_CHUNK_SIZE;

    int relativeChunkX = x % MAP_CHUNK_SIZE;
    int relativeChunkZ = z % MAP_CHUNK_SIZE;

    return GetChunk(chunkX, chunkZ)->heights[(relativeChunkX * MAP_CHUNK_SIZE) + relativeChunkZ];
}

MapController* MapController::Instance;

MapChunk* MapController::GetChunk(int x, int y)
{
    int chunkIndex = (x * chunkCountY) + y;
    return chunks[chunkIndex];
}
