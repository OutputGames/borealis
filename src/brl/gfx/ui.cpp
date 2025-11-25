#include "borealis/gfx/ui.hpp"

#include "borealis/gfx/gfx.hpp"

brl::GfxCanvas* brl::GfxCanvas::mainCanvas = nullptr;

brl::GfxCanvas::GfxCanvas()
{
    if (!mainCanvas)
        mainCanvas = this;

    scaledSize = glm::vec2
    {
        GfxEngine::instance->getMainWidth(),
        GfxEngine::instance->getMainHeight(),
    };
}

void brl::GfxCanvas::earlyUpdate()
{
    EcsEntity::earlyUpdate();
    draw_calls.clear();
}

void brl::GfxCanvas::lateUpdate()
{
    EcsEntity::lateUpdate();

    if (targetedCamera && targetedCamera->gfxCamera != cachedCamera)
    {
        cachedCamera = targetedCamera->gfxCamera;
    }

    if (cachedCamera == nullptr)
    {
        cachedCamera = GfxCamera::mainCamera;
    }
}

void brl::GfxCanvas::insertDrawCall(const GfxUIDrawCall call)
{
    draw_calls.push_back(call);
}

void brl::GfxCanvas::draw()
{

    GfxShaderValue projValue{};
    GfxShaderValue timeValue{};
    float scale = 2.0f;

    float aspect = scaledSize.x / scaledSize.y;
    float mainAspect = GfxEngine::instance->getAspectRatio();

    scale = aspect / mainAspect;

    projValue.m4value = glm::ortho(0.0f, static_cast<float>(GfxEngine::instance->getMainWidth()) / scale, 0.0f,
                                   static_cast<float>(GfxEngine::instance->getMainHeight()) / scale, -1.0f, 1.0f);

    timeValue.floatValue = glfwGetTime();

    for (const GfxUIDrawCall& call : draw_calls)
    {
        GfxShaderValue modelValue{};
        modelValue.m4value = call.transform;
        GfxUniformList overrides;
        overrides.insert({call.material->getShader()->getUniform("_internalProj"), projValue});
        overrides.insert({call.material->getShader()->getUniform("_internalModel"), modelValue});
        overrides.insert({call.material->getShader()->getUniform("_internalTime"), timeValue});

        call.material->draw(call.gfxBuffer, overrides);

        overrides.clear();
    }
}

brl::GfxUIElement::GfxUIElement()
{
}

void brl::GfxUIElement::start()
{
    EcsEntity::start();
    canvas = getEntityInParent<GfxCanvas>();
}

void brl::GfxImage::lateUpdate()
{
    GfxUIElement::lateUpdate();

    canvas->insertDrawCall({calculateTransform(), GfxMesh::GetPrimitive(QUAD)->GetSubMesh(0)->buffer, material});
}
