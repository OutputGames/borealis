#include <cstdio>
#include <iostream>
#include <borealis/gfx/gfx.hpp>


auto vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aUV;\n"
    "uniform mat4 _internalModel;\n"
    "uniform mat4 _internalView;\n"
    "uniform mat4 _internalProj;\n"
    "out vec2 texCoords;\n"
    "void main()\n"
    "{\n"
    "   texCoords = aUV;"
    "   gl_Position = _internalProj * _internalView * _internalModel * vec4(aPos, 1.0);\n"
    "}\0";
auto fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoords;\n"
    "uniform vec3 color;\n"
    "uniform sampler2D tex;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(texture(tex, texCoords));\n"
    "}\n\0";

int main(int argc, const char* argv[])
{
    brl::EcsEntityMgr entityMgr;


    brl::GfxEngine engine;
    engine.initialize();

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    auto attribBuffer = new brl::AttribGfxBuffer();


    attribBuffer->use();
    auto buffer = new brl::GfxBuffer(GL_ARRAY_BUFFER);
    buffer->use();
    buffer->updateData(GL_STATIC_DRAW, vertices, sizeof(vertices));

    //auto elementBuffer = new brl::GfxBuffer(GL_ELEMENT_ARRAY_BUFFER);
    //elementBuffer->use();
    //elementBuffer->updateData(GL_STATIC_DRAW, indices, sizeof(indices));


    attribBuffer->assignBuffer(buffer);
    //attribBuffer->assignElementBuffer(elementBuffer, GL_UNSIGNED_INT);


    attribBuffer->insertAttribute(brl::GfxAttribute{3, 5 * sizeof(float), static_cast<void*>(0)});
    attribBuffer->insertAttribute(brl::GfxAttribute{2, 5 * sizeof(float), (void*)(3 * sizeof(float))});

    auto shaderBins = new brl::GfxShader*[2];

    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, vertexShaderSource);
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);

    auto texture = new brl::GfxTexture2d("test/resources/dice.png");

    auto material = new brl::GfxMaterial(shader);
    //material->setVec3("color", brl::vector3{1,0,0});
    material->setTexture("tex", texture);

    glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 5.0f, -15.0f),
                                 glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                 glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
                                 glm::vec3(1.3f, -2.0f, -2.5f), glm::vec3(1.5f, 2.0f, -2.5f),
                                 glm::vec3(1.5f, 0.2f, -1.5f), glm::vec3(-1.3f, 1.0f, -1.5f)};

    auto camera = new brl::EcsCamera();
    camera->localPosition = {0, 0, -15.f};

    while (engine.isRunning())
    {

        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            auto model = glm::mat4(1.0f);
            model = translate(model, cubePositions[i]);
            float angle = (20.0f * i) + (glfwGetTime() * 20.f);
            model = rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            auto call = brl::GfxDrawCall{material, attribBuffer, model};
            engine.insertCall(call);
        }

        entityMgr.update();
        engine.update();
    }

    engine.shutdown();
    return 0;
}
