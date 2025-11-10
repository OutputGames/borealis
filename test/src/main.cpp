#include <cstdio>
#include <iostream>
#include <borealis/gfx/gfx.hpp>

int main(int argc, const char* argv[])
{
    system("..\\tools\\out\\resource_packer.exe ../test/resources/ ../out/");

    brl::IoEngine ioMgr;
    brl::EcsEngine entityMgr;


    brl::GfxEngine engine;
    engine.initialize();

    auto camera = new brl::EcsCamera();
    camera->localPosition = {0, 2.5f, 5.f};
    camera->fieldOfView = 75.f;
    camera->type = brl::ORTHOGRAPHIC;

    auto shaderBins = new brl::GfxShader*[2];

    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, brl::readFileString("shaders/test/vtx.glsl"));
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, brl::readFileString("shaders/test/frg.glsl"));
    auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);

    auto texture = new brl::GfxTexture2d("textures/test.png");

    auto material = new brl::GfxMaterial(shader);
    // material->setVec3("color", brl::vector3{1,0,0});
    material->setTexture("tex", texture);


    auto renderer = new brl::GfxMeshRenderer();
    renderer->mesh = brl::GfxAttribBuffer::GetPrimitive(brl::QUAD);
    renderer->material = material;
    renderer->localPosition = {0, 1.f, 0};


    auto floorRenderer = new brl::GfxMeshRenderer();
    floorRenderer->mesh = brl::GfxAttribBuffer::GetPrimitive(brl::QUAD);
    floorRenderer->material = material;
    floorRenderer->setEulerAngles({-90, 0, 0});
    floorRenderer->localScale = glm::vec3(2.5);


    while (engine.isRunning())
    {
        renderer->setEulerAngles({0, glm::sin(glfwGetTime()) * 45.0f, 0,});
        camera->lookAt({0, 0, 0});


        entityMgr.update();
        engine.update();
    }

    engine.shutdown();
    return 0;
}
