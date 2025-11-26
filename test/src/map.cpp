#include "map.h"
#include "FastNoiseLite.h"

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

void MapController::loadMap()
{
    int mapWidth = 72;
    int mapHeight = 72;
    float spacing = 1.0f;

    auto shaderBins = new brl::GfxShader*[2];

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    //noise.SetFrequency(0.0005f);

    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/map/map.vert"));
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/map/map.frag"));
    auto defaultShader = new brl::GfxShaderProgram(shaderBins, 2, true);

    brl::GfxModel* block = brl::GfxModel::loadModel("models/block_rect.glb");
    for (auto material : block->materials)
    {
        material->reloadShader(defaultShader);
    }

    int chunkCountX = mapWidth / MAP_CHUNK_SIZE;
    int chunkCountY = mapHeight / MAP_CHUNK_SIZE;

    for (int x = -mapWidth / 2; x < mapWidth / 2; ++x)
    {
        for (int y = -mapHeight / 2; y < mapHeight / 2; ++y)
        {
            int absoluteX = x + (mapWidth / 2);
            int absoluteY = y + (mapHeight / 2);

            int index = ((y + mapHeight / 2) * mapWidth) + (x + mapWidth / 2);

            int chunkX = absoluteX / 16;
            int chunkY = absoluteY / 16;

            int chunkIndex = (chunkX * chunkCountY) + chunkY;

            int value = (noise.GetNoise(static_cast<float>(x), static_cast<float>(y)) * 0.5 + 0.5) * 5.f;

            for (int i = 0; i <= value; ++i)
            {


                float blockX = x * spacing;
                float blockY = y * spacing;

                auto blockEntity = block->createEntity();
                blockEntity->localPosition = {blockX, -spacing + (spacing * i), blockY};
                blockEntity->setEulerAngles({0, 180, 0});
                blockEntity->localScale = glm::vec3(spacing / 2);
                blockEntity->setParent(this);

                blockEntity->getEntityInChildren<brl::GfxMeshRenderer>()->instancingID = chunkIndex;
            }
        }
    }
}

void MapController::update()
{
    EcsEntity::update();
}
