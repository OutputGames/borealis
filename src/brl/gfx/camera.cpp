#include "borealis/gfx/gfx.hpp"

brl::GfxCamera* brl::GfxCamera::mainCamera;


brl::GfxCamera::GfxCamera()
{
    mainCamera = this;
}

void brl::GfxCamera::draw(std::vector<GfxDrawCall>& calls, const GfxInstancedDrawCallList instancedCalls)
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

    GfxShaderValue* viewValue = new GfxShaderValue;
    GfxShaderValue* projValue = new GfxShaderValue;
    GfxShaderValue* timeValue = new GfxShaderValue;
    GfxShaderValue* cameraPosValue = new GfxShaderValue;

    viewValue->m4value = std::make_shared<std::vector<glm::mat4>>(std::vector<glm::mat4>{GetViewMatrix()});
    projValue->m4value = std::make_shared<std::vector<glm::mat4>>(std::vector<glm::mat4>{GetProjMatrix()});
    timeValue->floatValue = glfwGetTime();
    cameraPosValue->v3value = position;

    for (GfxDrawCall& call : calls)
    {
        GfxShaderValue* modelValue = new GfxShaderValue;
        modelValue->m4value = std::make_shared<std::vector<glm::mat4>>(std::vector<glm::mat4>{call.transform});
        GfxUniformList overrides;
        overrides.push_back({call.material->getShader()->getUniform("_internalView"), viewValue});
        overrides.push_back({call.material->getShader()->getUniform("_internalProj"), projValue});
        overrides.push_back({call.material->getShader()->getUniform("_internalModel"), modelValue});
        overrides.push_back({call.material->getShader()->getUniform("_internalTime"), timeValue});
        overrides.push_back({call.material->getShader()->getUniform("_cameraPosition"), cameraPosValue});


        overrides.insert(overrides.end(), call.uniqueOverrides.begin(), call.uniqueOverrides.end());

        call.material->draw(call.gfxBuffer, overrides);


        overrides.clear();
        call.uniqueOverrides.clear();
    }

    for (const auto& [key, call] : instancedCalls)
    {
        GfxUniformList overrides;
        overrides.push_back({call.material->getShader()->getUniform("_internalView"), viewValue});
        overrides.push_back({call.material->getShader()->getUniform("_internalProj"), projValue});
        overrides.push_back({call.material->getShader()->getUniform("_internalTime"), timeValue});
        overrides.push_back({call.material->getShader()->getUniform("_cameraPosition"), cameraPosValue});


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
