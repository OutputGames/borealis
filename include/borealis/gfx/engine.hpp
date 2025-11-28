#if !defined(ENGINE_HPP)
#define ENGINE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>

namespace brl
{
    struct GfxMaterialMgr;
    struct GfxInstancedDrawCall;
    struct GfxMesh;
}

namespace brl
{
    struct GfxDrawCall;
    struct GfxAttribBuffer;
    struct GfxMaterial;

    typedef size_t GfxInstancedCallKey;
    using GfxInstancedDrawCallList = std::map<GfxInstancedCallKey, GfxInstancedDrawCall>;


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
        void insertCall(GfxMaterial* material, GfxAttribBuffer* buffer, const glm::mat4& transform, int instancingIdx = 0);
        int getFrameCount();

        float getAspectRatio();
        int getMainWidth() { return mainWindow->getWidth(); }
        int getMainHeight() { return mainWindow->getHeight(); }
        float getDeltaTime()
        {
            return deltaTime;
        }

        
        std::vector<UtilCoroutine> active_coroutines;

    private:
        friend GfxAttribBuffer;
        friend GfxMesh;
        friend UtilCoroutine;
        friend GfxMaterialMgr;

        GfxMaterialMgr* materialMgr;

        bool initialized = false;
        GfxWindow* mainWindow = nullptr;
        std::vector<GfxDrawCall> calls;
        GfxInstancedDrawCallList instancedCalls;
        GfxMaterial* blitMaterial;

        int frameCount = 0;
        double lastFrameTime = 0, deltaTime = 0;

        GfxMesh* quadMesh;

    };

    struct GfxDrawCall {
        GfxMaterial* material;
        GfxAttribBuffer* gfxBuffer;
        const glm::mat4 transform;
    };

    struct GfxInstancedDrawCall
    {
        GfxMaterial* material;
        std::vector<glm::mat4> transforms;
        GfxAttribBuffer* gfxBuffer;
    };
} // namespace brl


#endif // ENGINE_HPP
