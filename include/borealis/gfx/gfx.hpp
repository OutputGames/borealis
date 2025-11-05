#if !defined(GFX_HPP)
#define GFX_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <borealis/util.h>

namespace brl
{

    struct GfxWindow {
        bool isOpen() { return !glfwWindowShouldClose((GLFWwindow*)window); }
    private:
        friend struct GfxEngine;
        void* window;

        int width, height;

        GfxWindow(int w, int h, const char* title);

        void pollEvents();
        void swapBuffers();
        void clear();

        int getWidth();
        int getHeight();
    };
    
    struct GfxEngine
    {
        GfxEngine();
        ~GfxEngine();

        void initialize();
        void shutdown();
        void update();
        bool isRunning() {return mainWindow->isOpen();}
        
    private:
        bool initialized = false;
        GfxWindow* mainWindow = nullptr;
    };
    
    struct GfxShader {
        GfxShader(GLenum format,const void* binary, size_t binarySize);
    }

} // namespace 


#endif // GFX_HPP
