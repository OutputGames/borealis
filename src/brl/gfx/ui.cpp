#include "borealis/gfx/ui.hpp"

#include "borealis/gfx/gfx.hpp"

#include "freetype/freetype.h"
#include FT_FREETYPE_H  

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

brl::GfxCanvas* brl::GfxCanvas::mainCanvas = nullptr;

void brl::GfxCanvas::insertDrawCall(const GfxUIDrawCall call)
{
    draw_calls.push_back(call);
}

void brl::GfxCanvas::draw()
{

    auto projValue = std::make_shared<GfxShaderValue>();
    auto timeValue = std::make_shared<GfxShaderValue>();

    glm::vec2 screenSize = {GfxEngine::instance->getMainWidth(), GfxEngine::instance->getMainHeight()};
    // Apply to your UI projection matrix
    glm::vec2 virtualSize = GetScaledCanvasSize(screenSize);

    float scaleFactor = GetScaleFactor(screenSize);

    float left = 0;
    float right = virtualSize.x;
    float bottom = virtualSize.y;
    float top = 0;

    projValue->m4value = CONVERT_UNIFORM_MAT4(std::vector<glm::mat4>{glm::ortho(left, right, bottom, top, -1.0f, 1.0f)});

    timeValue->floatValue = glfwGetTime();

    for (const GfxUIDrawCall& call : draw_calls)
    {
        auto modelValue = std::make_shared<GfxShaderValue>();
        modelValue->m4value = CONVERT_UNIFORM_MAT4(std::vector<glm::mat4>{call.transform});
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
    float scaleFactor = canvas->GetScaleFactor(screenSize);

    t = translate(t,(position()*scaleFactor) + ((scale()*scaleFactor)/2.0f));
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

brl::GfxFont::GfxFontCharacter::GfxFontCharacter(unsigned int i, glm::vec2 s, glm::vec2 b, unsigned int adv)
{
    this->id = i;
    this->size = s;
    this->bearing = b;
    this->advanceOffset = adv;

        // Create VBO for character quad (will be updated per-character)
    charVBO = new GfxBuffer(GL_ARRAY_BUFFER);

    // Create VAO for text rendering
    charVAO = new GfxAttribBuffer();
    charVAO->assignBuffer(charVBO);
    charVAO->mode = GL_TRIANGLES;

    // Configure vertex attributes for text rendering
    // Position (vec2) + TexCoords (vec2) = vec4
    GfxAttribute posTexCoord = {
        .size = 4, .stride = 4 * sizeof(float), .pointer = 0, .format = GL_FLOAT, .normalized = false};
    charVAO->insertAttribute(posTexCoord);

    // Create geometry in LOCAL SPACE (0,0) to (size.x, size.y)
    // The bearing offset will be applied via transform in lateUpdate
    float vertices[6][4] = {};

    // Position relative to bearing
    float xpos = bearing.x;
    float ypos = -(size.y - bearing.y); // Note: this accounts for baseline offset

    float w = size.x;
    float h = size.y;

    // Triangle 1
    vertices[0][0] = xpos;
    vertices[0][1] = ypos + h;
    vertices[0][2] = 0.0f;
    vertices[0][3] = 0.0f;

    vertices[1][0] = xpos;
    vertices[1][1] = ypos;
    vertices[1][2] = 0.0f;
    vertices[1][3] = 1.0f;

    vertices[2][0] = xpos + w;
    vertices[2][1] = ypos;
    vertices[2][2] = 1.0f;
    vertices[2][3] = 1.0f;

    // Triangle 2
    vertices[3][0] = xpos;
    vertices[3][1] = ypos + h;
    vertices[3][2] = 0.0f;
    vertices[3][3] = 0.0f;

    vertices[4][0] = xpos + w;
    vertices[4][1] = ypos;
    vertices[4][2] = 1.0f;
    vertices[4][3] = 1.0f;

    vertices[5][0] = xpos + w;
    vertices[5][1] = ypos + h;
    vertices[5][2] = 1.0f;
    vertices[5][3] = 0.0f;

    charVBO->updateData(GL_STATIC_DRAW, vertices, sizeof(vertices)); // Changed to STATIC_DRAW
}

brl::GfxFont::GfxFont(std::string path)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;

    auto file = brl::readFileBinary(path);

    if (FT_New_Memory_Face(ft, file.data,file.dataSize, 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        std::string charName = "";

        charName.push_back(static_cast<char8_t>(c));
        charName += "_";
        charName += face->family_name;

        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_DEFAULT))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF);

        if (face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
        {
            std::cerr << "Unexpected pixel mode: " << (int)face->glyph->bitmap.pixel_mode << std::endl;
            return;
        }

        // generate texture
        unsigned int texture = -1;

        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture);

        
        glObjectLabel(GL_TEXTURE, texture, -1, charName.c_str());


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                     GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        GfxFontCharacter* character = new GfxFontCharacter{
            texture, glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top), static_cast<unsigned int>(face->glyph->advance.x)
        };
        characters.insert(std::pair<char, GfxFontCharacter*>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

brl::GfxTextRenderer::GfxTextRenderer(GfxCanvas* c) : GfxUIElement(c)
{ canvas = c; }

void brl::GfxTextRenderer::start()
{
    GfxUIElement::start();

}

void brl::GfxTextRenderer::lateUpdate()
{
    GfxUIElement::lateUpdate();

    // Prepare vertex data
    glm::vec2 cursor = glm::vec2(0);
    float scale = glm::length(this->scale());

    glm::mat4 baseTransform = calculateTransform();

    for (char c : text)
    {
        GfxFont::GfxFontCharacter* ch = font->characters[c];

        // Skip characters with no visual representation
        if (ch->size.x == 0 || ch->size.y == 0)
        {
            cursor.x += (ch->advanceOffset >> 6) * scale;
            continue;
        }

        cursor.y = -ch->bearing.y*2;

        // Get or create material for this character
        GfxMaterial* material = getMaterial(glm::vec3(1), ch);

        // Create transform: translate to cursor position, then scale
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(cursor.x, cursor.y, 0.0f));
        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));

        // Combine: baseTransform * translation * scale
        // The character geometry already has bearing offset baked in
        glm::mat4 finalTransform = baseTransform * translation * scaling;

        // Create draw call
        GfxUIDrawCall drawCall;
        drawCall.transform = finalTransform;
        drawCall.gfxBuffer = ch->charVAO;
        drawCall.material = material;

        canvas->draw_calls.push_back(drawCall);

        // Advance cursor for next glyph (divide by 64 because FreeType uses 1/64 pixel units)
        cursor.x += (ch->advanceOffset >> 6) * scale;
    }

}


bool brl::GfxTextRenderer::TextMaterialKey::operator<(const TextMaterialKey& other) const
{
    if (textureID->id != other.textureID->id)
        return textureID->id < other.textureID->id;
    if (color.r != other.color.r)
        return color.r < other.color.r;
    if (color.g != other.color.g)
        return color.g < other.color.g;
    return color.b < other.color.b;
}

brl::GfxMaterial* brl::GfxTextRenderer::getMaterial(glm::vec3 color, GfxFont::GfxFontCharacter* textureID)
{
    TextMaterialKey key = {color, textureID};

    auto it = materialCache.find(key);
    if (it != materialCache.end())
    {
        return it->second;
    }

    // You'll need to implement this based on your GfxMaterial structure
    // This should create a shader with the following vertex and fragment shaders:

    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
        out vec2 TexCoords;
        
        uniform mat4 _internalProj;
        uniform mat4 _internalModel;
        
        void main() {

            vec4 p = _internalProj * _internalModel * vec4(vertex.xy, 0.0, 1.0);

            gl_Position = p;
            TexCoords = vertex.zw;
            TexCoords.y = 1.0 - TexCoords.y;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;
        
        uniform sampler2D text;
        uniform vec3 textColor;
        
        void main() {
        
            float r = texture(text,TexCoords).r;

            float m = 0.4f;
            float t = 0.1f;

            color = vec4( (r > m) ? r+m : 0.0f);
        }
    )";

    // Create material using your engine's material system

    auto shaderBins = new brl::GfxShader*[2];


    shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, vertexShaderSource);
    shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    auto shader = new brl::GfxShaderProgram(shaderBins, 2, true);

    GfxMaterial* material = new GfxMaterial(shader);
    // TODO: Set up shaders based on your material system
    // material->setShader(vertexShaderSource, fragmentShaderSource);

    material->setTexture("text", textureID);

    materialCache[key] = material;

    return material;
}

void brl::GfxTextRenderer::debugPrintCharacterInfo()
{
    std::cout << "=== Text Renderer Debug ===" << std::endl;
    std::cout << "Text: \"" << text << "\"" << std::endl;
    std::cout << "Scale: " << glm::length(this->scale()) << std::endl;

    glm::vec2 cursor = glm::vec2(0);
    float scale = glm::length(this->scale());

    for (char c : text)
    {
        GfxFont::GfxFontCharacter* ch = font->characters[c];

        std::cout << "Char '" << c << "':" << std::endl;
        std::cout << "  Size: (" << ch->size.x << ", " << ch->size.y << ")" << std::endl;
        std::cout << "  Bearing: (" << ch->bearing.x << ", " << ch->bearing.y << ")" << std::endl;
        std::cout << "  Advance: " << (ch->advanceOffset >> 6) << std::endl;
        std::cout << "  Cursor pos: (" << cursor.x << ", " << cursor.y << ")" << std::endl;

        cursor.x += (ch->advanceOffset >> 6) * scale;
    }
}
