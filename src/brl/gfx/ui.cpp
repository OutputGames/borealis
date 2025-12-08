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

    GfxShaderValue* projValue = new GfxShaderValue;
    GfxShaderValue* timeValue = new GfxShaderValue;

    glm::vec2 screenSize = {GfxEngine::instance->getMainWidth(), GfxEngine::instance->getMainHeight()};
    // Apply to your UI projection matrix
    glm::vec2 virtualSize = GetScaledCanvasSize(screenSize);

    float scaleFactor = GetScaleFactor(screenSize);

    float left = 0;
    float right = virtualSize.x;
    float bottom = virtualSize.y;
    float top = 0;

    projValue->m4value = std::make_shared<std::vector<glm::mat4>>(std::vector<glm::mat4>{glm::ortho(left, right, bottom, top, -1.0f, 1.0f)});

    timeValue->floatValue = glfwGetTime();

    for (const GfxUIDrawCall& call : draw_calls)
    {
        GfxShaderValue* modelValue = new GfxShaderValue;
        modelValue->m4value = std::make_shared<std::vector<glm::mat4>>(std::vector<glm::mat4>{call.transform});
        GfxUniformList overrides;
        overrides.push_back({call.material->getShader()->getUniform("_internalProj"), projValue});
        overrides.push_back({call.material->getShader()->getUniform("_internalModel"), modelValue});
        overrides.push_back({call.material->getShader()->getUniform("_internalTime"), timeValue});

        call.material->draw(call.gfxBuffer, overrides);

        overrides.clear();
    }

}

brl::GfxUIElement::GfxUIElement(GfxCanvas* c)
{
    canvas = c;
}

glm::mat4 brl::GfxUIElement::calculateTransform()
{
    glm::mat4 t(1.0);
    glm::vec2 screenSize = {GfxEngine::instance->getMainWidth(), GfxEngine::instance->getMainHeight()};
    float scaleFactor = canvas->GetScaleFactor(screenSize)*2;

    t = translate(t,(position()*scaleFactor) + (scale()*scaleFactor));
    t *= toMat4(rotation());
    t = glm::scale(t, scale()*scaleFactor);

    return t;
}


brl::GfxImage::GfxImage(GfxCanvas* c) : GfxUIElement(c)
{
   
}

void brl::GfxImage::loadTexture(GfxTexture2d* tex)
{
    localScale = {tex->getWidth(),tex->getHeight(),1};
    texture = tex;
}

void brl::GfxImage::lateUpdate()
{
    GfxUIElement::lateUpdate();

    material->setTexture("_mainTexture", texture);
    material->setVec3("_mainColor", color);

    canvas->insertDrawCall({calculateTransform(), GfxMesh::GetPrimitive(QUAD)->GetSubMesh(0)->buffer, material});
}
