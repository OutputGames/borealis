#include "borealis/gfx/gfx.hpp"

brl::GfxAttribBuffer* brl::GfxFramebufferAttachment::fullscreenQuadBuffer = nullptr;

void brl::GfxFramebufferAttachment::draw(GfxMaterial* material)
{
    if (!fullscreenQuadBuffer)
    {
        float quadVertices[] = {
            // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

        auto attribBuffer = new GfxAttribBuffer();


        attribBuffer->use();
        auto buffer = new GfxBuffer(GL_ARRAY_BUFFER);
        buffer->use();
        buffer->updateData(GL_STATIC_DRAW, quadVertices, sizeof(quadVertices));
        attribBuffer->assignBuffer(buffer);


        attribBuffer->insertAttribute(GfxAttribute{2, 4 * sizeof(float), static_cast<void*>(0)});
        attribBuffer->insertAttribute(GfxAttribute{2, 4 * sizeof(float), (void*)(2 * sizeof(float))});

        fullscreenQuadBuffer = attribBuffer;
    }

    GfxUniformList uniforms = {};

    auto sourceValue = std::make_shared<GfxShaderValue>();
    sourceValue->txValue = this;
    uniforms.push_back({material->getShader()->getUniform("_sourceTexture"), sourceValue});

    material->draw(fullscreenQuadBuffer, uniforms);
}

brl::GfxFramebuffer::GfxFramebuffer(int width, int height, GfxFramebufferAttachment** attachments, int attachmentCount)
{
    this->width = width;
    this->height = height;

    if (attachments == nullptr || attachmentCount == -1)
    {
        auto defaultAttachment = new GfxFramebufferAttachment;
        defaultAttachment->format = GL_RGB;
        defaultAttachment->internalFormat = GL_RGB;
        defaultAttachment->type = GL_UNSIGNED_BYTE;

        attachments = new GfxFramebufferAttachment*[1];
        attachmentCount = 1;

        attachments[0] = defaultAttachment;
    }

    this->attachments = attachments;
    this->attachmentCount = attachmentCount;

    glGenFramebuffers(1, &id);
    use();

    for (int i = 0; i < attachmentCount; ++i)
    {
        // generate texture
        if (this->attachments[i]->id == UINT32_MAX)
        {
            glGenTextures(1, &this->attachments[i]->id);
            glBindTexture(GL_TEXTURE_2D, this->attachments[i]->id);
            glTexImage2D(GL_TEXTURE_2D, 0, this->attachments[i]->internalFormat, width, height, 0,
                         this->attachments[i]->format,
                         this->attachments[i]->type, NULL);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // attach it to currently bound framebuffer object
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->attachments[i]->id, 0);
    }

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void brl::GfxFramebuffer::use()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, width, height);
}

void brl::GfxFramebuffer::clear()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

brl::GfxFramebuffer::~GfxFramebuffer()
{
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &id);
}

brl::GfxFramebufferAttachment* brl::GfxFramebuffer::getAttachment(int i)
{
    return attachments[i];
}
