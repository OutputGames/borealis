#include "borealis/gfx/gfx.hpp"

int brl::GfxAttribBuffer::getSize()
{
    if (ebo)
    {
        int size = 0;

        int elementSize = 0;

        switch (eboFormat)
        {
            case GL_UNSIGNED_INT:
                elementSize = sizeof(unsigned int);
                break;

            default:
                break;
        }

        size = ebo->size / elementSize;
        return size;
    }
    return vbo->size / vertexSize;
}

size_t brl::GfxAttribBuffer::getHash()
{
    size_t hash = 14695981039346656037ULL;

    uint32_t vboBits = vbo->id;
    hash ^= vboBits;
    hash *= 1099511628211ULL;

    if (ebo)
    {
        uint32_t eboBits = ebo->id;
        hash ^= eboBits;
        hash *= 1099511628211ULL;
    }


    return hash;

}

brl::GfxBuffer::GfxBuffer(GLenum format)
{
    this->format = format;
    glGenBuffers(1, &id);
}

void brl::GfxBuffer::use()
{
    glBindBuffer(format, id);
}

void brl::GfxBuffer::updateData(GLenum usage, const void* data, size_t size)
{
    use();
    glBufferData(format, size, data, usage);
    this->size = size;
}

void brl::GfxBuffer::destroy()
{
    glDeleteBuffers(1, &id);
}

brl::GfxAttribBuffer::GfxAttribBuffer()
{
    glGenVertexArrays(1, &id);
}

brl::GfxAttribBuffer::~GfxAttribBuffer()
{
    destroy();
}

void brl::GfxAttribBuffer::assignBuffer(GfxBuffer* buffer)
{
    use();
    buffer->use();
    vbo = buffer;
}

void brl::GfxAttribBuffer::assignElementBuffer(GfxBuffer* buffer, GLenum format)
{
    use();
    buffer->use();

    eboFormat = format;
    ebo = buffer;
}

void brl::GfxAttribBuffer::insertAttribute(GfxAttribute attribute, GfxBuffer* bufferToUse)
{
    use();
    if (bufferToUse)
        bufferToUse->use();
    else
        vbo->use();

    if (attribute.format != GL_INT)
        glVertexAttribPointer(attributeCount, attribute.size, attribute.format, attribute.normalized, attribute.stride,attribute.pointer);
    else
        glVertexAttribIPointer(attributeCount, attribute.size, attribute.format, attribute.stride,
                              attribute.pointer);

    glEnableVertexAttribArray(attributeCount);

    if (attribute.divisor > -1)
        glVertexAttribDivisor(attributeCount, attribute.divisor);

    vertexSize = attribute.stride;

    attributeCount++;
}

void brl::GfxAttribBuffer::ensureReadyForInstancing()
{
    if (!setupAttributesForInstancing)
    {
        auto buffer = new GfxBuffer(GL_ARRAY_BUFFER);
        buffer->use();

        instanceBuffer = buffer;

        // basic transformation matrices
        insertAttribute({4, sizeof(glm::mat4), static_cast<void*>(0), GL_FLOAT, false, 1}, instanceBuffer);
        insertAttribute({4, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)), GL_FLOAT, false, 1}, instanceBuffer);
        insertAttribute({4, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)), GL_FLOAT, false, 1}, instanceBuffer);
        insertAttribute({4, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)), GL_FLOAT, false, 1}, instanceBuffer);

        setupAttributesForInstancing = true;
    }
}

void brl::GfxAttribBuffer::updateInstanceBuffer(std::vector<glm::mat4> instances)
{
    instanceBuffer->updateData(GL_DYNAMIC_DRAW, instances.data(), instances.size() * sizeof(glm::mat4));
}

void brl::GfxAttribBuffer::use()
{
    glBindVertexArray(id);
}

void brl::GfxAttribBuffer::destroy()
{
    vbo->destroy();
    ebo->destroy();
    glDeleteVertexArrays(1, &id);
}
