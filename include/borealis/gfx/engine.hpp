#if !defined(ENGINE_HPP)
#define ENGINE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util/util.h>

namespace brl
{
    struct GfxDrawCall;
    struct AttribGfxBuffer;
    struct GfxMaterial;

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

    struct GfxDrawCall {
        GfxMaterial* material;
        AttribGfxBuffer* gfxBuffer;
        const glm::mat4 transform;
    };
} // namespace brl


#endif // ENGINE_HPP
