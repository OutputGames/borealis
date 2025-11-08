#include "borealis/gfx/gfx.hpp"

brl::GfxCamera* brl::GfxCamera::mainCamera;


brl::GfxCamera::GfxCamera()
{
    mainCamera = this;
}

void brl::GfxCamera::draw(const std::vector<GfxDrawCall>& calls)
{
    if (targetFramebuffer != cachedFramebuffer && targetFramebuffer != nullptr)
    {
        // destroy old buffer
    }

    if (cachedFramebuffer == nullptr)
    {
        cachedFramebuffer =
            new GfxFramebuffer(GfxEngine::instance->getMainWidth(), GfxEngine::instance->getMainHeight());
    }

    cachedFramebuffer->use();
    cachedFramebuffer->clear();

    GfxShaderValue viewValue{};
    GfxShaderValue projValue{};
    viewValue.m4value = GetViewMatrix();
    projValue.m4value = GetProjMatrix();

    for (const GfxDrawCall& call : calls)
    {
        GfxShaderValue modelValue{};
        modelValue.m4value = call.transform;
        std::map<GfxShaderUniform*, GfxShaderValue> overrides;
        overrides.insert({call.material->getShader()->getUniform("_internalView"), viewValue});
        overrides.insert({call.material->getShader()->getUniform("_internalProj"), projValue});
        overrides.insert({call.material->getShader()->getUniform("_internalModel"), modelValue});

        call.material->draw(call.gfxBuffer, overrides);

        overrides.clear();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 brl::GfxCamera::GetViewMatrix()
{
    auto view = glm::mat4(1.0);

    glm::vec3 fwd = glm::vec3(0, 0, 1) * rotation;
    glm::vec3 up = glm::vec3(0, 1, 0) * rotation;

    view = lookAt(position, position + fwd, up);

    return view;
}

glm::mat4 brl::GfxCamera::GetProjMatrix()
{
    auto proj = glm::mat4(1.0);

    proj = glm::perspective(fieldOfView, getAspectRatio(), minLimit, maxLimit);

    return proj;
}

float brl::GfxCamera::getAspectRatio()
{
    return GfxEngine::instance->getAspectRatio();
}

void brl::EcsCamera::earlyUpdate()
{
    EcsEntity::earlyUpdate();

    gfxCamera->fieldOfView = fieldOfView;
    gfxCamera->minLimit = minLimit;
    gfxCamera->maxLimit = maxLimit;
    gfxCamera->position = position();
    gfxCamera->rotation = rotation();
}

