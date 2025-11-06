#include "borealis/gfx/gfx.hpp"
#include "gfx.hpp"
#include "gfx.hpp"
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

    for (GfxDrawCall* call : calls) {
        call->material->draw(call->gfxBuffer);
    }
    calls.clear();

    mainWindow->pollEvents();
    mainWindow->swapBuffers();
}

void brl::GfxEngine::insertCall(GfxDrawCall* call) {
    calls.push_back(call);
}

void brl::GfxWindow::clear() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void brl::GfxEngine::shutdown() { glfwTerminate(); }

brl::GfxBuffer::GfxBuffer(GLenum format) {
    this->format = format;
    glGenBuffers(1, &id);
}

void brl::GfxBuffer::use() { glBindBuffer(format, id); }

void brl::GfxBuffer::updateData(GLenum usage, const void* data, size_t size) {
    glBufferData(format, size,data,usage);
    this->size = size;
}

void brl::GfxBuffer::destroy() { glDeleteBuffers(1, &id); }

brl::AttribGfxBuffer::AttribGfxBuffer() { glGenVertexArrays(1,&id); }

void brl::AttribGfxBuffer::assignBuffer(GfxBuffer* buffer) {
    use();
    buffer->use();
    vbo = buffer;
}

void brl::AttribGfxBuffer::assignElementBuffer(GfxBuffer* buffer, GLenum format) {
    use();
    buffer->use();

    eboFormat = format;
    ebo = buffer;
}
void brl::AttribGfxBuffer::insertAttribute(GfxAttribute attribute) {
    use();
    vbo->use();

    glVertexAttribPointer(attributeCount, attribute.size, attribute.format, attribute.normalized, attribute.stride, attribute.pointer);
    glEnableVertexAttribArray(attributeCount);

    vertexSize = attribute.stride;

    attributeCount++;
}

void brl::AttribGfxBuffer::use() { glBindVertexArray(id); }

void brl::AttribGfxBuffer::destroy() { glDeleteVertexArrays(1, &id); }

void brl::AttribGfxBuffer::draw() {


}

brl::GfxShader::GfxShader(GLenum type, std::string data) {
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

void brl::GfxShader::destroy() { glDeleteShader(id); }

brl::GfxShaderProgram::GfxShaderProgram(GfxShader** shaders, int shaderCount, bool deleteOnLoad) {
    id = glCreateProgram();

    for (int i = 0; i < shaderCount; i++)
    {
        GfxShader* shader = shaders[i];
        glAttachShader(id,shader->id);
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

    if (deleteOnLoad) {
        for (int i = 0; i < shaderCount; i++)
        {
            GfxShader* shader = shaders[i];
            shader->destroy();
        }
    }

    brl::print("Sucessfully linked shader.");

    GLint totalUniforms = 0;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &totalUniforms);
    uniforms = new brl::GfxShaderUniform[totalUniforms];

    brl::print("\tAmount of uniforms: "+std::to_string(totalUniforms));

    for (int i = 0; i < totalUniforms; i++)
    {
        GLint nameLength = -1;
        GLint num = -1;
        GLenum type = GL_ZERO;
        char name[256];

        glGetActiveUniform(id, i, sizeof(name)-1, &nameLength, &num, &type, name);

        uniforms[i] = GfxShaderUniform{name, type, i};

        std::cout << "\t" << name << " - " << type << std::endl;
    }


    

}

void brl::GfxShaderProgram::use() {
    glUseProgram(id);
}

void brl::GfxMaterial::draw(AttribGfxBuffer * buffer)
{
    
    shader->use();
    for (const auto& _override : overrides) {
        switch (_override.first->type) {
            case GL_FLOAT:
                glUniform1f(_override.first->location, _override.second.floatValue);
            case GL_FLOAT_VEC2:
                glUniform2fv(_override.first->location, 1, _override.second.v2value);
            case GL_INT:
                glUniform1i(_override.first->location, _override.second.intValue);
        }
    }

    if (buffer->ebo) {
        buffer->ebo->use();
        glDrawElements(GL_TRIANGLES, buffer->getSize(), buffer->eboFormat, 0);
    } else {



        glDrawArrays(GL_TRIANGLES, 0,  buffer->getSize());
    }
}
int brl::AttribGfxBuffer::getSize()
{
    if (ebo) {
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
    } else {
        return vbo->size /  vertexSize;
    }
}