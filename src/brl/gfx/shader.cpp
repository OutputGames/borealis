#include "borealis/gfx/gfx.hpp"

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
        exit(-1);
    }
}

void brl::GfxShader::destroy()
{
    glDeleteShader(id);
}

static brl::GfxShaderProgram* defaultShader = nullptr;

brl::GfxShaderProgram* brl::GfxShaderProgram::GetDefaultShader()
{
    if (!defaultShader)
    {

        auto shaderBins = new GfxShader*[2];

        auto vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec3 aNorm;\n"
            "layout (location = 2) in vec2 aUV;\n"
            "uniform mat4 _internalModel;\n"
            "uniform mat4 _internalView;\n"
            "uniform mat4 _internalProj;\n"
            "out vec2 texCoords;\n"
            "out vec3 normal;\n"
            "out vec3 pos;\n"
            "void main()\n"
            "{\n"
            "   texCoords = aUV;"
            "   normal = mat3(transpose(inverse(_internalModel))) * aNorm;"
            "   pos = vec3(_internalModel * vec4(aPos, 1.0));"
            "   gl_Position = _internalProj * _internalView * _internalModel * vec4(aPos, 1.0);\n"
            "}\0";
        auto fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 texCoords;\n"
            "in vec3 normal;\n"
            "in vec3 pos;\n"
            "uniform vec3 color;\n"
            "uniform sampler2D tex;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(texture(tex, texCoords));\n"
            "   vec3 norm = normalize(normal);\n"
            "   vec3 lightDir = normalize(vec3(0,10,-5) - pos);\n "
            "   float d =max(dot(norm, lightDir), 0.0) + 0.25f;\n"
            "   FragColor = vec4(d*texture(tex,texCoords).rgb,1.0);\n"
            "}\n\0";

        shaderBins[0] = new GfxShader(GL_VERTEX_SHADER, vertexShaderSource);
        shaderBins[1] = new GfxShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        defaultShader = new GfxShaderProgram(shaderBins, 2, true);
    }

    return defaultShader;
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
        exit(-1);
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

void brl::GfxMaterial::draw(GfxAttribBuffer* buffer,
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
