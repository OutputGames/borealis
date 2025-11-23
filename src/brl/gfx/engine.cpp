#include <thread>

#include <renderdoc/renderdoc_app.h>
using RENDERDOC_GetAPIFunc = void(RENDERDOC_CC*)(RENDERDOC_API_1_1_0* api);

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "borealis/gfx/gfx.hpp"
#include "borealis/util/input.hpp"

brl::GfxEngine* brl::GfxEngine::instance;

RENDERDOC_API_1_1_2* rdoc_api = NULL;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


brl::GfxWindow::GfxWindow(int w, int h, const char* title)
{
    window = glfwCreateWindow(w, h, title, NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        return;
    }

    width = w;
    height = h;


    glfwSetFramebufferSizeCallback(static_cast<GLFWwindow*>(window), framebuffer_size_callback);


}

void brl::GfxWindow::pollEvents()
{
    glfwPollEvents();
}

void brl::GfxWindow::swapBuffers()
{
    glfwSwapBuffers(static_cast<GLFWwindow*>(window));
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
    calls.clear();
    delete blitMaterial;
    delete quadMesh;
}


void brl::GfxEngine::initialize()
{
    if (initialized)
        return;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow = new GfxWindow(640, 360, "untitled cp1 game");
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(mainWindow->window));

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }


    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_MULTISAMPLE);


    //glEnable(GL_CULL_FACE);


    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                              const GLchar* message, const void* userParam)
                           {
                               std::cerr << "GL Debug: " << message << std::endl;
                           },
                           nullptr);
    instance = this;

    auto vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aUV;\n"


        "out vec2 texCoords;\n"


        "void main()\n"
        "{\n"
        "   texCoords = aUV;"
        "   gl_Position = vec4(aPos,0, 1.0);\n"


        "}\0";
    auto fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 texCoords;\n"
        "uniform sampler2D _sourceTexture;\n"


        "void main()\n"
        "{\n"
        "   FragColor = vec4(texture(_sourceTexture,texCoords).rgb,1);\n"


        "}\n\0";

    auto shaderBins = new GfxShader*[2];

    shaderBins[0] = new GfxShader(GL_VERTEX_SHADER, vertexShaderSource);
    shaderBins[1] = new GfxShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    auto shader = new GfxShaderProgram(shaderBins, 2, true);


    blitMaterial = new GfxMaterial(shader);

    {
        std::vector<GfxVertex> vertices = {
            {{-1, -1, 0}, {0, 0, 1}, {0, 0}},
            {{-1, 1, 0}, {0, 0, 1}, {0, 1}},
            {{1, 1, 0}, {0, 0, 1}, {1, 1}},
            {{1, -1, 0}, {0, 0, 1}, {1, 0}},

        };

        std::vector<unsigned> indices = {0, 1, 2, 2, 3, 0};

        auto attribBuffer = new GfxAttribBuffer();


        attribBuffer->use();

        auto buffer = new GfxBuffer(GL_ARRAY_BUFFER);
        buffer->use();
        buffer->updateData(GL_STATIC_DRAW, vertices.data(), sizeof(GfxVertex) * vertices.size());

        auto elementBuffer = new GfxBuffer(GL_ELEMENT_ARRAY_BUFFER);
        elementBuffer->use();
        elementBuffer->updateData(GL_STATIC_DRAW, indices.data(), sizeof(unsigned) * indices.size());


        attribBuffer->assignBuffer(buffer);
        attribBuffer->assignElementBuffer(elementBuffer, GL_UNSIGNED_INT);


        attribBuffer->insertAttribute(GfxAttribute{3, 8 * sizeof(float), static_cast<void*>(0)});
        attribBuffer->insertAttribute(GfxAttribute{3, 8 * sizeof(float), (void*)(3 * sizeof(float))});
        attribBuffer->insertAttribute(GfxAttribute{2, 8 * sizeof(float), (void*)(6 * sizeof(float))});

        quadMesh = new GfxMesh(attribBuffer);
    }

    InputMgr::init(static_cast<GLFWwindow*>(mainWindow->window));


#ifdef _WIN32
    if (HMODULE mod = LoadLibrary(reinterpret_cast<LPCWSTR>("renderdoc.dll")))
    {
        auto RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
        assert(ret == 1);
    }
    else
    {
        DWORD error = GetLastError();
        std::cout << "Failed to load DLL. Error code: " << error << std::endl;
    }
#else // For Linux/macOS
    void* renderdoc_module = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
    if (renderdoc_module)
    {
        RENDERDOC_GetAPIFunc get_api_func = (RENDERDOC_GetAPIFunc)dlsym(renderdoc_module, "RENDERDOC_GetAPI");
        if (get_api_func)
        {
            get_api_func(&rd_api);
        }
        dlclose(renderdoc_module); // Close the handle as we only need the function pointer
    }
#endif

    // To start a frame capture, call StartFrameCapture.
    // You can specify NULL, NULL for the device to capture on if you have only one device and
    // either no windows at all or only one window, and it will capture from that device.
    // See the documentation below for a longer explanation

    // Your rendering should happen here

    // stop the capture

}

auto start_time = std::chrono::high_resolution_clock::now();

void brl::GfxEngine::update()
{
    bool cap = false;
    if (InputMgr::getKeyDown(GLFW_KEY_P))
        cap = true;

    if (rdoc_api && cap)
        rdoc_api->StartFrameCapture(NULL, NULL);


    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> frame_time = end_time - start_time;

    double target_frame_time_ms = 1000.0 / (90.);

    if (frame_time.count() < target_frame_time_ms)
    {
        std::this_thread::sleep_for(
            std::chrono::duration<double, std::milli>(target_frame_time_ms - frame_time.count()));
    }


    GfxCamera::mainCamera->draw(calls);

    calls.clear();

    mainWindow->clear();
    GfxCamera::mainCamera->cachedFramebuffer->getAttachment(0)->draw(blitMaterial);


    InputMgr::update();
    mainWindow->pollEvents();
    mainWindow->swapBuffers();

    frameCount++;

    double frameTime = glfwGetTime();
    deltaTime = frameTime - lastFrameTime;
    lastFrameTime = frameTime;

    for (auto it = active_coroutines.begin(); it != active_coroutines.end();)
    {
        it->resume(deltaTime);
        if (it->is_done())
        {
            it = active_coroutines.erase(it);
        }
        else
        {
            ++it;
        }
    }

    start_time = std::chrono::high_resolution_clock::now();

    if (rdoc_api && cap)
        rdoc_api->EndFrameCapture(NULL, NULL);

}

void brl::GfxEngine::insertCall(const GfxDrawCall& call)
{
    calls.push_back(call);
}

int brl::GfxEngine::getFrameCount()
{
    return frameCount;
}

float brl::GfxEngine::getAspectRatio()
{
    return static_cast<float>(mainWindow->getWidth()) / static_cast<float>(mainWindow->getHeight());
}


void brl::GfxWindow::clear()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
}

void brl::GfxEngine::shutdown()
{
    glfwTerminate();
}
