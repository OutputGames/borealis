#include <cstdio>
#include <iostream>
#include <borealis/gfx/gfx.hpp>


const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 color;"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(color, 1.0f);\n"
    "}\n\0";

int main(int argc, char const *argv[])
{
    brl::GfxEngine engine;
    engine.initialize();

float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};

unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};  
    
    brl::AttribGfxBuffer* attribBuffer = new brl::AttribGfxBuffer();


    attribBuffer->use();
    brl::GfxBuffer* buffer = new brl::GfxBuffer(GL_ARRAY_BUFFER);
    buffer->use();
    buffer->updateData(GL_STATIC_DRAW, vertices, sizeof(vertices));

    brl::GfxBuffer* elementBuffer = new brl::GfxBuffer(GL_ELEMENT_ARRAY_BUFFER);
    elementBuffer->use();
    elementBuffer->updateData(GL_STATIC_DRAW, indices, sizeof(indices));


    attribBuffer->assignBuffer(buffer);
    attribBuffer->assignElementBuffer(elementBuffer, GL_UNSIGNED_INT);


    attribBuffer->insertAttribute(brl::GfxAttribute{3,3*sizeof(float), (void*)0});

    brl::GfxShader** shaderBins = new brl::GfxShader*[2];

    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, vertexShaderSource);
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    brl::GfxShaderProgram* shader = new brl::GfxShaderProgram(shaderBins, 2, true);


    while(engine.isRunning())
    {
        brl::GfxDrawCall* call = new brl::GfxDrawCall{shader, attribBuffer};
        engine.insertCall(call);

        engine.update();
    }

    engine.shutdown();
    return 0;
}
