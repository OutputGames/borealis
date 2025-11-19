#if !defined(BUFFER_HPP)
#define BUFFER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>

namespace brl
{
    struct GfxShaderValue;
    struct GfxShaderUniform;
    struct GfxTexture;
    struct GfxDrawCall;
    struct GfxAttribBuffer;
    struct GfxMaterial;
    
    struct IGfxBuffer 
    {


        virtual void use() {}
        virtual void destroy() {};
        

    private:
        friend struct GfxBuffer;
        friend struct GfxAttribBuffer;

        IGfxBuffer() {};
        unsigned int id = -1;
    };

    struct GfxBuffer : public IGfxBuffer {
        GfxBuffer(GLenum format);

        void use();
        void updateData(GLenum usage, const void* data, size_t size);
        void destroy();

    private:
        friend struct GfxAttribBuffer;
        GLenum format;
        size_t size;
    };

    struct GfxAttribute {
        int size;
        int stride;
        const void* pointer;

        GLenum format = GL_FLOAT;
        bool normalized = false;
    };

    enum GfxPrimitiveType
    {
        QUAD
    };
 
    struct GfxAttribBuffer : public IGfxBuffer {
        GfxAttribBuffer();
        ~GfxAttribBuffer();

        void assignBuffer(GfxBuffer* buffer);
        void assignElementBuffer(GfxBuffer* buffer, GLenum format);
        void insertAttribute(GfxAttribute attribute);

        void use();
        void destroy();

        int getSize();

    private:
        friend GfxMaterial;
        GfxBuffer* vbo = nullptr, *ebo = nullptr;
        GLenum eboFormat = GL_UNSIGNED_INT;
        int attributeCount = 0;
        size_t vertexSize;
    };


} // namespace brl


#endif // BUFFER_HPP
