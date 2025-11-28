#include "borealis/gfx/ui.hpp"

#include "borealis/gfx/gfx.hpp"

brl::GfxCanvas* brl::GfxCanvas::mainCanvas = nullptr;

brl::GfxCanvas::GfxCanvas()
{
    if (!mainCanvas)
        mainCanvas = this;

    referenceResolution = glm::vec2
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

    glm::vec2 screenSize = {GfxEngine::instance->getMainWidth(), GfxEngine::instance->getMainHeight()};
    // Apply to your UI projection matrix
    glm::vec2 virtualSize = GetScaledCanvasSize(screenSize);

    projValue.m4value = glm::ortho(0.0f, virtualSize.x, virtualSize.y, 0.0f, -1.0f, 1.0f);

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

brl::GfxUIElement::GfxUIElement(GfxCanvas* c)
{
    canvas = c;
}

brl::GfxUIRect brl::GfxUIElement::GetRect() const
{
    auto parentSize = canvas->referenceResolution;

    if (parent->getEntity<GfxUIElement>())
        parentSize = parent->getEntity<GfxUIElement>()->GetRect().size;

    // Calculate anchor positions in pixels
    glm::vec2 anchorMinPixels = anchorMin * parentSize;
    glm::vec2 anchorMaxPixels = anchorMax * parentSize;

    // Calculate corners
    glm::vec2 rectMin = anchorMinPixels + offsetMin;
    glm::vec2 rectMax = anchorMaxPixels + offsetMax;

    // Size and position
    glm::vec2 size = rectMax - rectMin;
    glm::vec2 position = rectMin;

    return {position, size};
}

glm::mat4 brl::GfxUIElement::calculateTransform()
{
    glm::vec2 screenSize = {GfxEngine::instance->getMainWidth(), GfxEngine::instance->getMainHeight()};

    // Get scaled canvas size
    glm::vec2 canvasSize = canvas->GetScaledCanvasSize(screenSize);

    // Set root parent size to canvas size
    if (!parent)
    {
        // This is a root element, use canvas size as parent
    }

    // Get final position and size
    glm::vec2 position = GetWorldPosition();
    glm::vec2 size = GetSize();

    // Apply scale factor
    float scaleFactor = canvas->GetScaleFactor(screenSize);
    position *= scaleFactor;
    size *= scaleFactor;

    // Build transform matrix
    auto transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0));
    transform = glm::scale(transform, glm::vec3(size, 1));

    return transform;
}


brl::GfxImage::GfxImage(GfxCanvas* c) :
    GfxUIElement(c)
{
}

void brl::GfxImage::lateUpdate()
{
    GfxUIElement::lateUpdate();

    material->setTexture("_mainTexture", texture);
    material->setVec3("_mainColor", color);

    canvas->insertDrawCall({calculateTransform(), GfxMesh::GetPrimitive(QUAD)->GetSubMesh(0)->buffer, material});
}
