#include "borealis/debug/debug.hpp"

brl::GfxMaterial* debugMaterial = nullptr;
brl::GfxMaterial* debugLineMaterial = nullptr;

    // Fix: create a named lvalue for the empty map to satisfy non-const reference requirement
static brl::GfxUniformList emptyUniforms;

void brl_debug::drawMesh(brl::GfxAttribBuffer* buffer, glm::mat4 transform)
{
    if (!debugMaterial)
    {
        debugMaterial = new brl::GfxMaterial(brl::GfxShaderProgram::GetDefaultShader());
    }


    brl::GfxEngine::instance->insertCall(debugMaterial, buffer, transform, emptyUniforms);
}

void brl_debug::drawLine(glm::vec3 start, glm::vec3 end)
{
    if (!debugLineMaterial)
    {
        debugLineMaterial =
            new brl::GfxMaterial(new brl::GfxShaderProgram(brl::readFileString("D:/shaders/defaultline.glsl")));
    }

    glm::mat4 t(1.0);
    t[0] = glm::vec4(start, 0);
    t[1] = glm::vec4(end, 0);

    brl::GfxEngine::instance->insertCall(debugLineMaterial, brl::GfxMesh::GetPrimitive(brl::LINE)->GetSubMesh(0)->buffer, t, emptyUniforms);
}
