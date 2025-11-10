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
    GfxShaderValue timeValue{};
    viewValue.m4value = GetViewMatrix();
    projValue.m4value = GetProjMatrix();
    timeValue.floatValue = glfwGetTime();

    for (const GfxDrawCall& call : calls)
    {
        GfxShaderValue modelValue{};
        modelValue.m4value = call.transform;
        std::map<GfxShaderUniform*, GfxShaderValue> overrides;
        overrides.insert({call.material->getShader()->getUniform("_internalView"), viewValue});
        overrides.insert({call.material->getShader()->getUniform("_internalProj"), projValue});
        overrides.insert({call.material->getShader()->getUniform("_internalModel"), modelValue});
        overrides.insert({call.material->getShader()->getUniform("_internalTime"), timeValue});

        call.material->draw(call.gfxBuffer, overrides);

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
        proj = glm::perspective(glm::radians(fieldOfView), getAspectRatio(), minLimit, maxLimit);
    }
    else
    {
        float halfHeight = orthographicSize;
        float halfWidth = halfHeight * getAspectRatio();

        float left = -halfWidth;
        float right = halfWidth;
        float bottom = -halfHeight;
        float top = halfHeight;


        proj = glm::ortho(left, right, bottom, top, minLimit, maxLimit);
    }

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
    gfxCamera->orthographicSize = orthographicSize;
    gfxCamera->minLimit = minLimit;
    gfxCamera->maxLimit = maxLimit;
    gfxCamera->type = type;

    gfxCamera->position = position();
    gfxCamera->rotation = rotation();
}
