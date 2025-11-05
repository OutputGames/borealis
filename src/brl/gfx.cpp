#include "borealis/gfx/gfx.hpp"


brl::GfxWindow::GfxWindow(int w, int h, const char* title)
{
    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        return;
    }


}

void brl::GfxWindow::pollEvents()
{
    glfwPollEvents();
}

void brl::GfxWindow::swapBuffers()
{
    glfwSwapBuffers((GLFWwindow*)window);
}

int brl::GfxWindow::getWidth()
{
    return width;
}

int brl::GfxWindow::getHeight()
{
    return height;
}

brl::GfxEngine::GfxEngine()
{
    initialized = false;
}

brl::GfxEngine::~GfxEngine()
{
    
}

void brl::GfxEngine::initialize()
{
    if (initialized)
        return;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow = new GfxWindow(800, 600, "LearnOpenGL");
    glfwMakeContextCurrent((GLFWwindow*)mainWindow->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }    
    
}

void brl::GfxEngine::update()
{
    mainWindow->clear();

    mainWindow->pollEvents();
    mainWindow->swapBuffers();
}

void brl::GfxWindow::clear() {
    glClearColor(0.3,0.3,0.3,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void brl::GfxEngine::shutdown() {
    glfwTerminate();
}