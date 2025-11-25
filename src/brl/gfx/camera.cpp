#include "borealis/gfx/gfx.hpp"

brl::GfxCamera* brl::GfxCamera::mainCamera;


brl::GfxCamera::GfxCamera()
{
    mainCamera = this;
}

void brl::GfxCamera::draw(const std::vector<GfxDrawCall>& calls, const GfxInstancedDrawCallList instancedCalls)
{
    if (targetFramebuffer != cachedFramebuffer && targetFramebuffer != nullptr)
    {
        delete cachedFramebuffer;
        cachedFramebuffer = targetFramebuffer;
    }

    if (cachedFramebuffer == nullptr)
    {
        cachedFramebuffer = new GfxFramebuffer(GfxEngine::instance->getMainWidth(),
                                               GfxEngine::instance->getMainHeight());
    }

    cachedFramebuffer->use();
    cachedFramebuffer->clear();

    GfxShaderValue viewValue{};
    GfxShaderValue projValue{};
    GfxShaderValue timeValue{};
    viewValue.m4value = GetViewMatrix();
    projValue.m4value = GetProjMatrix();
    timeValue.floatValue = glfwGetTime();

    for (const GfxDrawCall& call : calls)
    {
        GfxShaderValue modelValue{};
        modelValue.m4value = call.transform;
        GfxUniformList overrides;
        overrides.insert({call.material->getShader()->getUniform("_internalView"), viewValue});
        overrides.insert({call.material->getShader()->getUniform("_internalProj"), projValue});
        overrides.insert({call.material->getShader()->getUniform("_internalModel"), modelValue});
        overrides.insert({call.material->getShader()->getUniform("_internalTime"), timeValue});

        call.material->draw(call.gfxBuffer, overrides);

        overrides.clear();
    }

    for (const auto& [key, call] : instancedCalls)
    {
        GfxUniformList overrides;
        overrides.insert({call.material->getShader()->getUniform("_internalView"), viewValue});
        overrides.insert({call.material->getShader()->getUniform("_internalProj"), projValue});
        overrides.insert({call.material->getShader()->getUniform("_internalTime"), timeValue});

        call.material->drawInstanced(call.transforms, call.gfxBuffer, overrides);

        overrides.clear();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 brl::GfxCamera::GetViewMatrix()
{
    auto view = glm::mat4(1.0);
    glm::mat4 rotationMatrix = mat4_cast(rotation);
    auto upVector = glm::vec3(rotationMatrix[1]);
    auto fwdVector = glm::vec3(rotationMatrix[2]);

    view = lookAt(position, position + fwdVector, upVector);

    return view;
}

glm::mat4 brl::GfxCamera::GetProjMatrix()
{
    auto proj = glm::mat4(1.0);

    if (type == PERSPECTIVE)
    {
        proj = GetPerspectiveProjMatrix();
    }
    else
    {
        proj = GetOrthoProjMatrix();
    }

    return proj;
}

glm::mat4 brl::GfxCamera::GetOrthoProjMatrix()
{
    float halfHeight = orthographicSize;
    float halfWidth = halfHeight * cachedFramebuffer->getAspectRatio();

    float left = -halfWidth;
    float right = halfWidth;
    float bottom = -halfHeight;
    float top = halfHeight;


    return glm::ortho(left, right, bottom, top, minLimit, maxLimit);
}

glm::mat4 brl::GfxCamera::GetPerspectiveProjMatrix()
{
    return glm::perspective(glm::radians(fieldOfView), cachedFramebuffer->getAspectRatio(), minLimit, maxLimit);

}

float brl::GfxCamera::getAspectRatio() { return cachedFramebuffer->getAspectRatio(); }

void brl::EcsCamera::earlyUpdate()
{
    EcsEntity::earlyUpdate();

    gfxCamera->fieldOfView = fieldOfView;
    gfxCamera->orthographicSize = orthographicSize;
    gfxCamera->minLimit = minLimit;
    gfxCamera->maxLimit = maxLimit;
    gfxCamera->type = type;

    gfxCamera->position = position();
    gfxCamera->rotation = rotation();

    gfxCamera->targetFramebuffer = targetFramebuffer;
}
