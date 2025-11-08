#include "borealis/gfx/gfx.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

brl::GfxEngine* brl::GfxEngine::instance;
brl::GfxCamera* brl::GfxCamera::mainCamera;

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
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(mainWindow->window));

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }


    glEnable(GL_DEPTH_TEST);

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

}

void brl::GfxEngine::update()
{

    GfxCamera::mainCamera->draw(calls);

    calls.clear();

    mainWindow->clear();
    GfxCamera::mainCamera->cachedFramebuffer->getAttachment(0)->draw(blitMaterial);

    mainWindow->pollEvents();
    mainWindow->swapBuffers();

    frameCount++;

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        std::cerr << "OpenGL Error: " << err << std::endl;
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
}

void brl::GfxEngine::shutdown() { glfwTerminate(); }

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
    glBufferData(format, size, data, usage);
    this->size = size;
}

void brl::GfxBuffer::destroy()
{
    glDeleteBuffers(1, &id);
}

brl::AttribGfxBuffer::AttribGfxBuffer()
{
    glGenVertexArrays(1, &id);
}

void brl::AttribGfxBuffer::assignBuffer(GfxBuffer* buffer)
{
    use();
    buffer->use();
    vbo = buffer;
}

void brl::AttribGfxBuffer::assignElementBuffer(GfxBuffer* buffer, GLenum format)
{
    use();
    buffer->use();

    eboFormat = format;
    ebo = buffer;
}

void brl::AttribGfxBuffer::insertAttribute(GfxAttribute attribute)
{
    use();
    vbo->use();

    glVertexAttribPointer(attributeCount, attribute.size, attribute.format, attribute.normalized, attribute.stride,
                          attribute.pointer);
    glEnableVertexAttribArray(attributeCount);

    vertexSize = attribute.stride;

    attributeCount++;
}

void brl::AttribGfxBuffer::use()
{
    glBindVertexArray(id);
}

void brl::AttribGfxBuffer::destroy()
{
    glDeleteVertexArrays(1, &id);
}


brl::GfxShader::GfxShader(GLenum type, std::string data)
{
    this->type = type;
    id = glCreateShader(type);
    const char* src = data.c_str();
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed.\n" << infoLog << std::endl;
    }
}

void brl::GfxShader::destroy()
{
    glDeleteShader(id);
}

brl::GfxShaderProgram::GfxShaderProgram(GfxShader** shaders, int shaderCount, bool deleteOnLoad)
{
    id = glCreateProgram();

    for (int i = 0; i < shaderCount; i++)
    {
        GfxShader* shader = shaders[i];
        glAttachShader(id, shader->id);
    }
    glLinkProgram(id);

    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cerr << "Shader linking failed.\n" << infoLog << std::endl;
        return;
    }

    if (deleteOnLoad)
    {
        for (int i = 0; i < shaderCount; i++)
        {
            GfxShader* shader = shaders[i];
            shader->destroy();
        }
    }

    print("Sucessfully linked shader.");

    GLint totalUniforms = 0;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &totalUniforms);
    uniforms = new GfxShaderUniform[totalUniforms];

    print("\tAmount of uniforms: " + std::to_string(totalUniforms));

    uniformCount = totalUniforms;

    textureSlots = 0;
    std::vector<std::string> _textures;

    for (int i = 0; i < totalUniforms; i++)
    {
        GLint nameLength = -1;
        GLint num = -1;
        GLenum type = GL_ZERO;
        char name[256];

        glGetActiveUniform(id, i, sizeof(name) - 1, &nameLength, &num, &type, name);

        uniforms[i] = GfxShaderUniform{name, type, i};

        std::string typeName = "";

        switch (type)
        {
            case GL_FLOAT:
                typeName = "float";
                break;
            case GL_INT:
                typeName = "int";
                break;
            case GL_FLOAT_VEC2:
                typeName = "vec2";
                break;
            case GL_FLOAT_VEC3:
                typeName = "vec3";
                break;
            case GL_FLOAT_VEC4:
                typeName = "vec4";
                break;
            case GL_FLOAT_MAT4:
                typeName = "mat4";
                break;
            case GL_SAMPLER_2D:
                typeName = "texture2D";
                textureSlots++;
                _textures.push_back(name);
                break;
        }

        std::cout << "\t" << name << " - " << typeName << std::endl;
    }

    textures = new std::string[textureSlots];

    use();

    for (int i = 0; i < textureSlots; ++i)
    {
        textures[i] = _textures[i];
        glUniform1i(getUniform(textures[i])->location, i);
    }

}

void brl::GfxShaderProgram::use()
{
    glUseProgram(id);


}

void brl::GfxMaterial::draw(AttribGfxBuffer* buffer,
                            std::map<GfxShaderUniform*, GfxShaderValue> runtimeOverrides)
{
    buffer->use();
    shader->use();


    for (const auto& _override : overrides)
    {
        //std::cout << "overriding uniform at " << _override.first->location << "(" << _override.first->name << ")"<< std::endl;
        switch (_override.first->type)
        {
            case GL_FLOAT:
                glUniform1f(_override.first->location, _override.second.floatValue);
                break;
            case GL_FLOAT_VEC2:
                glUniform2f(_override.first->location, _override.second.v2value.x, _override.second.v2value.y);
                break;
            case GL_FLOAT_VEC3:
                glUniform3f(_override.first->location, _override.second.v3value.x, _override.second.v3value.y,
                            _override.second.v3value.z);
                break;
            case GL_FLOAT_VEC4:
                glUniform4fv(_override.first->location, 1, value_ptr(_override.second.v4value));
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(_override.first->location, 1,GL_FALSE, value_ptr(_override.second.m4value));
                break;
            case GL_INT:
                glUniform1i(_override.first->location, _override.second.intValue);
                break;
            case GL_SAMPLER_2D:
                glActiveTexture(GL_TEXTURE0 + shader->getTextureIndex(_override.first->name));
                glBindTexture(GL_TEXTURE_2D, _override.second.txValue->id);
                break;
            default:
                break;
        }
    }

    for (const auto& _override : runtimeOverrides)
    {
        if (!_override.first)
            continue;

        switch (_override.first->type)
        {
            case GL_FLOAT:
                glUniform1f(_override.first->location, _override.second.floatValue);
                break;
            case GL_FLOAT_VEC2:
                glUniform2f(_override.first->location, _override.second.v2value.x, _override.second.v2value.y);
                break;
            case GL_FLOAT_VEC3:
                glUniform3f(_override.first->location, _override.second.v3value.x, _override.second.v3value.y,
                            _override.second.v3value.z);
                break;
            case GL_FLOAT_VEC4:
                glUniform4fv(_override.first->location, 1, value_ptr(_override.second.v4value));
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(_override.first->location, 1, GL_FALSE, value_ptr(_override.second.m4value));
                break;
            case GL_INT:
                glUniform1i(_override.first->location, _override.second.intValue);
                break;
            case GL_SAMPLER_2D:
            {
                int i = shader->getTextureIndex(_override.first->name);
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, _override.second.txValue->id);
            }
            break;
            default:
                break;
        }
    }

    if (buffer->ebo)
    {
        buffer->ebo->use();


        glDrawElements(GL_TRIANGLES, buffer->getSize(), buffer->eboFormat, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, buffer->getSize());
    }
}

void brl::GfxMaterial::setOverride(std::pair<GfxShaderUniform*, GfxShaderValue> pair)
{
    if (overrides.contains(pair.first))
    {
        overrides[pair.first] = pair.second;
    }
    else
    {
        overrides.insert(pair);
    }
}

int brl::AttribGfxBuffer::getSize()
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

brl::AttribGfxBuffer* brl::GfxFramebufferAttachment::fullscreenQuadBuffer = nullptr;

void brl::GfxFramebufferAttachment::draw(GfxMaterial* material)
{
    if (!fullscreenQuadBuffer)
    {
        float quadVertices[] = {
            // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

        auto attribBuffer = new AttribGfxBuffer();


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

    GfxShaderValue sourceValue = {};
    sourceValue.txValue = this;
    uniforms.insert({material->getShader()->getUniform("_sourceTexture"), sourceValue});

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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // attach it to currently bound framebuffer object
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->attachments[i]->id, 0);
    }

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void brl::GfxFramebuffer::use()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void brl::GfxFramebuffer::clear()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

brl::GfxFramebufferAttachment* brl::GfxFramebuffer::getAttachment(int i)
{
    return attachments[i];
}

brl::GfxCamera::GfxCamera()
{
    mainCamera = this;
}

void brl::GfxCamera::draw(const std::vector<GfxDrawCall>& calls)
{
    if (targetFramebuffer != cachedFramebuffer && targetFramebuffer != nullptr)
    {
        // destroy old buffer
    }

    if (cachedFramebuffer == nullptr)
    {
        cachedFramebuffer =
            new GfxFramebuffer(GfxEngine::instance->getMainWidth(), GfxEngine::instance->getMainHeight());
    }

    cachedFramebuffer->use();
    cachedFramebuffer->clear();

    GfxShaderValue viewValue{};
    GfxShaderValue projValue{};
    viewValue.m4value = GetViewMatrix();
    projValue.m4value = GetProjMatrix();

    for (const GfxDrawCall& call : calls)
    {
        GfxShaderValue modelValue{};
        modelValue.m4value = call.transform;
        std::map<GfxShaderUniform*, GfxShaderValue> overrides;
        overrides.insert({call.material->getShader()->getUniform("_internalView"), viewValue});
        overrides.insert({call.material->getShader()->getUniform("_internalProj"), projValue});
        overrides.insert({call.material->getShader()->getUniform("_internalModel"), modelValue});

        call.material->draw(call.gfxBuffer, overrides);

        overrides.clear();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 brl::GfxCamera::GetViewMatrix()
{
    auto view = glm::mat4(1.0);

    glm::vec3 fwd = glm::vec3(0, 0, 1) * rotation;
    glm::vec3 up = glm::vec3(0, 1, 0) * rotation;

    view = lookAt(position, position + fwd, up);

    return view;
}

glm::mat4 brl::GfxCamera::GetProjMatrix()
{
    auto proj = glm::mat4(1.0);

    proj = glm::perspective(fieldOfView, getAspectRatio(), minLimit, maxLimit);

    return proj;
}

float brl::GfxCamera::getAspectRatio()
{
    return GfxEngine::instance->getAspectRatio();
}

brl::GfxTexture2d::GfxTexture2d(std::string path)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        GLenum internalFormat = GL_RGB8;

        switch (nrChannels)
        {
            case 1:
                format = GL_R;
                internalFormat = GL_R8;
                break;
            case 2:
                format = GL_RG;
                internalFormat = GL_RG8;
                break;
            case 3:
                format = GL_RGB;
                internalFormat = GL_RGB8;
                break;
            case 4:
                format = GL_RGBA;
                internalFormat = GL_RGBA8;
        }


        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
