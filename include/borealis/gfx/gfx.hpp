#if !defined(GFX_HPP)
#define GFX_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>

namespace brl
{
    struct GfxShaderValue;
    struct GfxShaderUniform;
    struct GfxTexture;

    struct GfxDrawCall;
    struct AttribGfxBuffer;
    struct GfxMaterial;

    using GfxUniformList = std::map<GfxShaderUniform*, GfxShaderValue>;


    struct GfxWindow {
        bool isOpen() { return !glfwWindowShouldClose((GLFWwindow*)window); }

        int getWidth();
        int getHeight();

    private:
        friend struct GfxEngine;
        void* window;

        int width, height;

        GfxWindow(int w, int h, const char* title);

        void pollEvents();
        void swapBuffers();
        void clear();

    };
    
    struct GfxEngine
    {
        static GfxEngine* instance;

        GfxEngine();
        ~GfxEngine();

        void initialize();
        void shutdown();
        void update();
        bool isRunning() {return mainWindow->isOpen();}
        void insertCall(const GfxDrawCall& call);
        int getFrameCount();

        float getAspectRatio();
        int getMainWidth() { return mainWindow->getWidth(); }
        int getMainHeight() { return mainWindow->getHeight(); }
        
    private:
        bool initialized = false;
        GfxWindow* mainWindow = nullptr;
        std::vector<GfxDrawCall> calls;
        GfxMaterial* blitMaterial;

        int frameCount = 0;
    };
    
    struct GfxShader {
        GfxShader(GLenum format,const void* binary, size_t binarySize) = delete;
        GfxShader(GLenum type,std::string data);

        void destroy();

    private:
        friend struct GfxShaderProgram;
        unsigned int id;
        GLenum type;
    };

    struct GfxShaderUniform {
        std::string name;
        GLenum type;
        int location;
    };

    struct GfxShaderValue {
        int intValue;
        float floatValue;
        glm::vec2 v2value;
        glm::vec3 v3value;
        glm::vec4 v4value;
        glm::mat4 m4value;
        GfxTexture* txValue;
    };

    struct GfxShaderProgram 
    {
        GfxShaderProgram(GfxShader** shaders, int shaderCount, bool deleteOnLoad);

        void use();

    private:
        friend struct GfxMaterial;
        friend struct GfxCamera;
        friend struct GfxFramebufferAttachment;
        unsigned int id;

        GfxShaderUniform* uniforms;
        int uniformCount;

        std::string* textures;
        int textureSlots;

        GfxShaderUniform* getUniform(std::string name) {
            for(int i=0; i<uniformCount; i++){
                if (uniforms[i].name == name)
                {
                    uniforms[i].location = glGetUniformLocation(id, name.c_str());
                    return &uniforms[i];
                }
            }
            return nullptr;
        }

        int getTextureIndex(std::string name)
        {
            for (int i = 0; i < textureSlots; i++)
            {
                if (textures[i] == name)
                {
                    return i;
                }
            }
            return -1;
        }
    };

    struct GfxTexture
    {
    private:
        friend struct GfxMaterial;
        friend struct GfxShader;
        friend struct GfxTexture2d;
        friend struct GfxFramebuffer;
        unsigned id = UINT_MAX;
    };

    struct GfxTexture2d : GfxTexture {

        GfxTexture2d(std::string path);

        int getWidth() {return width;}
        int getHeight() {return height;}

    private:

        int width, height;
    };

    struct GfxMaterial {

        GfxMaterial(GfxShaderProgram* shader) {
            this->shader = shader;
        }

        GfxShaderProgram* getShader() {return shader;}
        
        void setInt(std::string name, int value) {

            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.intValue = value;
            setOverride({shader->getUniform(name), val});
        }

        void setFloat(std::string name, float value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.floatValue = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec2(std::string name, glm::vec2 value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.v2value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec3(std::string name, glm::vec3 value) {

            if (shader->getUniform(name) == nullptr)
                return;

            GfxShaderValue val;
            val.v3value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setVec4(std::string name, glm::vec4 value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.v4value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setMat4(std::string name, glm::mat4 value) {


            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.m4value = value;
            setOverride({shader->getUniform(name), val});
        }

        void setTexture(std::string name, GfxTexture* value)
        {
            if (!shader->getUniform(name))
                return;

            GfxShaderValue val;
            val.txValue = value;
            setOverride({shader->getUniform(name), val});
        }

        void draw(AttribGfxBuffer* buffer, GfxUniformList runtimeOverrides = {});


    private:
        GfxShaderProgram* shader;
        GfxUniformList overrides;

        void setOverride(std::pair<GfxShaderUniform*, GfxShaderValue> pair);
    };

    struct IGfxBuffer 
    {


        virtual void use() {}
        virtual void destroy() {};
        

    private:
        friend struct GfxBuffer;
        friend struct AttribGfxBuffer;

        IGfxBuffer() {};
        unsigned int id = -1;
    };

    struct GfxBuffer : public IGfxBuffer {
        GfxBuffer(GLenum format);

        void use();
        void updateData(GLenum usage, const void* data, size_t size);
        void destroy();

    private:
        friend struct AttribGfxBuffer;
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

    struct AttribGfxBuffer : public IGfxBuffer {
        AttribGfxBuffer();

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

    struct GfxDrawCall {
        GfxMaterial* material;
        AttribGfxBuffer* gfxBuffer;
        const glm::mat4 transform;
    };

    struct GfxFramebufferAttachment : GfxTexture
    {
        GLenum format, internalFormat;
        GLenum type;

        void draw(GfxMaterial* material);

    private:

        static AttribGfxBuffer* fullscreenQuadBuffer;

    };

    struct GfxFramebuffer
    {

        GfxFramebuffer(int width, int height, GfxFramebufferAttachment** attachments = nullptr, int attachmentCount = -1);

        void use();
        void clear();

        GfxFramebufferAttachment* getAttachment(int i );

    private:
        int width, height;
        GfxFramebufferAttachment** attachments;
        int attachmentCount;

        unsigned id;
    };

    struct GfxCamera
    {
        static GfxCamera* mainCamera;

        GfxCamera();

        glm::vec3 position;
        glm::quat rotation;

        float fieldOfView = 45;
        float minLimit = 0.01f;
        float maxLimit = 100.0f;

        GfxFramebuffer* targetFramebuffer;

        void draw(const std::vector<GfxDrawCall>& calls);

    private:
        friend GfxEngine;
        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjMatrix();
        float getAspectRatio();

        GfxFramebuffer* cachedFramebuffer = nullptr;
    };

} // namespace 


#endif // GFX_HPP
