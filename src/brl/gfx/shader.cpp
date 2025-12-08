#include "borealis/gfx/gfx.hpp"

#define STB_INCLUDE_IMPLEMENTATION
#include "stb_include.h"

static char* stb_include_load_file(char* filename, size_t* plen)
{
    if (plen)
        *plen = brl::readFileBinary(filename).dataSize;

    auto f = brl::readFileString(filename);

    return f.data();
}

void replaceInString(std::string& s, std::string from, std::string to)
{
    size_t start_pos = 0;
    while ((start_pos = s.find(from, start_pos)) != std::string::npos)
    {
        s.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Advance past the replaced text
    }
}


brl::GfxShader::GfxShader(GLenum type, std::string data)
{
    this->type = type;
    id = glCreateShader(type);

    {
        std::string from = "#definclude \"";
        std::string to = "#include \"D:/";
        replaceInString(data, from, to);
    }


    include_info* inc_list;
    int incCount = stb_include_find_includes(data.data(), &inc_list);
    while (incCount > 0)
    {
        include_info info = inc_list[0];

        std::string inc = readFileString(info.filename);

        data.replace(info.offset, info.end - info.offset, inc);
        incCount = stb_include_find_includes(data.data(), &inc_list);
    }

    if (data.contains("#property INSTANCING"))
    {
        isInstanced = true;
        replaceInString(data, "#property INSTANCING", "#define __KEYWORD__INSTANCING");
    }

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

        auto vertexShaderSource =
            "#version 330 core\n"
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
        auto fragmentShaderSource =
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 texCoords;\n"
            "in vec3 normal;\n"
            "in vec3 pos;\n"
            "uniform vec4 color;\n"
            "uniform sampler2D tex;\n"
            "uniform sampler2D norm;\n"
            "vec3 getNormalFromMap()\n"
            "{\n"
            "    vec3 tangentNormal = texture(norm, texCoords).xyz * 2.0 - 1.0;\n"
            "\n"
            "    vec3 Q1  = dFdx(pos);\n"
            "    vec3 Q2  = dFdy(pos);\n"
            "    vec2 st1 = dFdx(texCoords);\n"
            "    vec2 st2 = dFdy(texCoords);\n"
            "\n"
            "    vec3 N   = normalize(normal);\n"
            "    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);\n"
            "    vec3 B  = -normalize(cross(N, T));\n"
            "    mat3 TBN = mat3(T, B, N);\n"
            "\n"
            "    return normalize(TBN * tangentNormal);\n"
            "}"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(texture(tex, texCoords));\n"
            "   vec3 norm = normalize(normal);\n"
            "   vec3 lightDir = -normalize(vec3(1,-1,-1));\n "
            "   float d =max(dot(norm, lightDir), 0.0) + 0.25f;\n"
            "   FragColor = vec4(texture(tex,texCoords).rgb,1.0);\n"
            "}\n\0";

        shaderBins[0] = new GfxShader(GL_VERTEX_SHADER, vertexShaderSource);
        shaderBins[1] = new GfxShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        defaultShader = new GfxShaderProgram(shaderBins, 2, true);
    }

    return defaultShader;
}


brl::GfxShaderBinding::~GfxShaderBinding()
{
    if (value != nullptr)
    {

        delete value;
        value = nullptr;
    }
}

bool brl::GfxUniformList::contains(GfxShaderUniform* uniform)
{
    for (auto& element1 : *this) // Non-const loop for non-const method
    {
        if (element1.uniform == uniform)
            return true;
    }
    return false;
}

brl::GfxShaderProgram::GfxShaderProgram(GfxShader** shaders, int shaderCount, bool deleteOnLoad)
{
    id = glCreateProgram();

    for (int i = 0; i < shaderCount; i++)
    {
        GfxShader* shader = shaders[i];
        if (shader->isInstanced)
            instancingEnabled = true;

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

    process();

}

brl::GfxShaderProgram::GfxShaderProgram(std::string compositeSource)
{

    // start shader splitting
    std::istringstream iss(compositeSource); // Create an input string stream
    std::string line;

    std::vector<GfxShader*> shaders;

    int sectionStart = -1;
    bool isInShader = false;
    GLenum shaderType;

    int i = 0;
    // Loop through the lines using std::getline
    while (std::getline(iss, line))
    {
        if (line.starts_with("#define __STARTSHADER"))
        {
            isInShader = true;
        }
        else
        {
            if (isInShader)
            {
                if (line.starts_with("#define __SHADERTYPE"))
                {
                    sectionStart = i + line.size() + 1;

                    std::string type = line.substr(std::string("#define __SHADERTYPE ").size());

                    if (type.contains("VTX"))
                    {
                        shaderType = GL_VERTEX_SHADER;
                    }
                    else if (type.contains("FRAG"))
                        shaderType = GL_FRAGMENT_SHADER;
                }

                if (line.starts_with("#define __ENDSHADER"))
                {
                    std::string section = compositeSource.substr(sectionStart, i - sectionStart);
                    shaders.push_back(new GfxShader(shaderType, section));
                    isInShader = false;
                }
            }
        }
        i += line.size() + 1;
    }


    id = glCreateProgram();

    for (int i = 0; i < shaders.size(); i++)
    {
        GfxShader* shader = shaders[i];
        if (shader->isInstanced)
            instancingEnabled = true;

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

    for (int i = 0; i < shaders.size(); i++)
    {
        GfxShader* shader = shaders[i];
        shader->destroy();
    }


    process();
}

brl::GfxShaderProgram::~GfxShaderProgram()
{
    glDeleteProgram(id);
}

void brl::GfxShaderProgram::use()
{
    glUseProgram(id);


}

void brl::GfxShaderProgram::process()
{
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

        uniforms[i] = GfxShaderUniform{};
        uniforms[i].name = name;
        uniforms[i].type = static_cast<GfxUniformType>(type);
        uniforms[i].location = i;
        uniforms[i].count = num;

        std::string typeName = "";

        switch (type)
        {
            case GL_FLOAT:
                typeName = "float";
                break;
            case GL_INT:
                typeName = "int";
                break;
            case GL_BOOL:
                typeName = "bool";
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
            case GL_SAMPLER_2D_ARRAY:
                typeName = "texture2DArray";
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

brl::GfxMaterial::GfxMaterial(GfxShaderProgram* shader)
{
    this->shader = shader;
    GfxMaterialMgr::GetInstance()->InsertToRegistry(this);
}

brl::GfxMaterial::~GfxMaterial()
{
    overrides.clear();
    GfxMaterialMgr::GetInstance()->RemoveFromRegistry(this);
}

void brl::GfxMaterial::reloadShader(GfxShaderProgram* shader)
{
    this->shader = shader;
    GfxUniformList prevOverrides = overrides;

    overrides.clear();

    for (auto override : prevOverrides)
    {
        if (shader->getUniform(override.uniform->name))
        {
            overrides.push_back(override);
        }
    }
}

size_t brl::GfxMaterial::getHash()
{
    size_t hash = 14695981039346656037ULL;

    // shader id hashing
    uint32_t shaderBits = shader->id;
    hash ^= shaderBits;
    hash *= 1099511628211ULL;

    // material id hashing
    uint32_t materialBits = registryIndex;
    hash ^= materialBits;
    hash *= 1099511628211ULL;

    return hash;
}

void brl::GfxMaterial::draw(GfxAttribBuffer* buffer,
                            GfxUniformList runtimeOverrides)
{
    buffer->use();
    shader->use();


    for (const auto& _override : overrides)
    {
        if (!_override.uniform)
            continue;
        

        //std::cout << "overriding uniform at " << _override.uniform->location << "(" << _override.uniform->name << ")"<< std::endl;
        switch (_override.uniform->type)
        {
            case GL_FLOAT:
                glUniform1f(_override.uniform->location, _override.value->floatValue);
                break;
            case GL_FLOAT_VEC2:
                glUniform2f(_override.uniform->location, _override.value->v2value.x, _override.value->v2value.y);
                break;
            case GL_FLOAT_VEC3:
                glUniform3f(_override.uniform->location, _override.value->v3value.x, _override.value->v3value.y,
                            _override.value->v3value.z);
                break;
            case GL_FLOAT_VEC4:
                glUniform4fv(_override.uniform->location, 1, value_ptr(_override.value->v4value));
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(_override.uniform->location, _override.value->m4value.get()->size(), GL_FALSE,
                                   glm::value_ptr((*_override.value->m4value)[0]));
                break;
            case GL_INT:
            case GL_BOOL:
                glUniform1i(_override.uniform->location, _override.value->intValue);
                break;
            case GL_SAMPLER_2D:
                glActiveTexture(GL_TEXTURE0 + shader->getTextureIndex(_override.uniform->name));
                if (_override.value->txValue)
                {
                    glBindTexture(GL_TEXTURE_2D, _override.value->txValue->id);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, GfxTexture2d::getWhiteTexture()->id);

                }
                break;
            case GL_SAMPLER_2D_ARRAY:
            {
                int i = shader->getTextureIndex(_override.uniform->name);
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D_ARRAY, _override.value->txValue->id);
            }
            break;
            default:
                break;
        }
    }

    for (const auto& _override : runtimeOverrides)
    {
        if (!_override.uniform)
            continue;

        switch (_override.uniform->type)
        {
            case GL_FLOAT:
                glUniform1f(_override.uniform->location, _override.value->floatValue);
                break;
            case GL_FLOAT_VEC2:
                glUniform2f(_override.uniform->location, _override.value->v2value.x, _override.value->v2value.y);
                break;
            case GL_FLOAT_VEC3:
                glUniform3f(_override.uniform->location, _override.value->v3value.x, _override.value->v3value.y,
                            _override.value->v3value.z);
                break;
            case GL_FLOAT_VEC4:
                glUniform4fv(_override.uniform->location, 1, value_ptr(_override.value->v4value));
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(_override.uniform->location, _override.value->m4value.get()->size(), GL_FALSE,
                                   glm::value_ptr((*_override.value->m4value)[0]));
                break;
            case GL_INT:
                glUniform1i(_override.uniform->location, _override.value->intValue);
                break;
            case GL_SAMPLER_2D:
            {
                int i = shader->getTextureIndex(_override.uniform->name);
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, _override.value->txValue->id);
            }
            break;
            case GL_SAMPLER_2D_ARRAY:
            {
                int i = shader->getTextureIndex(_override.uniform->name);
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D_ARRAY, _override.value->txValue->id);
            }
            break;
            default:
                break;
        }
    }

    if (buffer->ebo)
    {
        buffer->ebo->use();


        glDrawElements(buffer->mode, buffer->getSize(), buffer->eboFormat, 0);
    }
    else
    {
        glDrawArrays(buffer->mode, 0, buffer->getSize());
    }
}

void brl::GfxMaterial::drawInstanced(std::vector<glm::mat4> transforms, GfxAttribBuffer* buffer,
                                     GfxUniformList runtimeOverrides)
{
    buffer->ensureReadyForInstancing();
    buffer->updateInstanceBuffer(transforms);

    buffer->use();
    shader->use();


    for (const auto& _override : overrides)
    {
        // std::cout << "overriding uniform at " << _override.uniform->location << "(" << _override.uniform->name << ")"<<
        // std::endl;
        switch (_override.uniform->type)
        {
            case GL_FLOAT:
                glUniform1f(_override.uniform->location, _override.value->floatValue);
                break;
            case GL_FLOAT_VEC2:
                glUniform2f(_override.uniform->location, _override.value->v2value.x, _override.value->v2value.y);
                break;
            case GL_FLOAT_VEC3:
                glUniform3f(_override.uniform->location, _override.value->v3value.x, _override.value->v3value.y,
                            _override.value->v3value.z);
                break;
            case GL_FLOAT_VEC4:
                glUniform4fv(_override.uniform->location, 1, value_ptr(_override.value->v4value));
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(_override.uniform->location, _override.value->m4value.get()->size(), GL_FALSE,
                                   glm::value_ptr((*_override.value->m4value)[0]));
                break;
            case GL_INT:
            case GL_BOOL:
                glUniform1i(_override.uniform->location, _override.value->intValue);
                break;
            case GL_SAMPLER_2D:
                glActiveTexture(GL_TEXTURE0 + shader->getTextureIndex(_override.uniform->name));
                if (_override.value->txValue)
                {
                    glBindTexture(GL_TEXTURE_2D, _override.value->txValue->id);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, GfxTexture2d::getWhiteTexture()->id);
                }
                break;
            case GL_SAMPLER_2D_ARRAY:
            {
                int i = shader->getTextureIndex(_override.uniform->name);
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D_ARRAY, _override.value->txValue->id);
            }
            break;
            default:
                break;
        }
    }

    for (const auto& _override : runtimeOverrides)
    {
        if (!_override.uniform)
            continue;

        switch (_override.uniform->type)
        {
            case GL_FLOAT:
                glUniform1f(_override.uniform->location, _override.value->floatValue);
                break;
            case GL_FLOAT_VEC2:
                glUniform2f(_override.uniform->location, _override.value->v2value.x, _override.value->v2value.y);
                break;
            case GL_FLOAT_VEC3:
                glUniform3f(_override.uniform->location, _override.value->v3value.x, _override.value->v3value.y,
                            _override.value->v3value.z);
                break;
            case GL_FLOAT_VEC4:
                glUniform4fv(_override.uniform->location, 1, value_ptr(_override.value->v4value));
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(_override.uniform->location, _override.value->m4value.get()->size(), GL_FALSE,
                                   glm::value_ptr((*_override.value->m4value)[0]));
                break;
            case GL_INT:
                glUniform1i(_override.uniform->location, _override.value->intValue);
                break;
            case GL_SAMPLER_2D:
            {
                int i = shader->getTextureIndex(_override.uniform->name);
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, _override.value->txValue->id);
            }
            break;
            case GL_SAMPLER_2D_ARRAY:
            {
                int i = shader->getTextureIndex(_override.uniform->name);
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D_ARRAY, _override.value->txValue->id);
            }
            break;
            default:
                break;
        }
    }

    if (buffer->ebo)
    {
        buffer->ebo->use();


        glDrawElementsInstanced(GL_TRIANGLES, buffer->getSize(), buffer->eboFormat, 0, transforms.size());
    }
    else
    {
        glDrawArraysInstanced(GL_TRIANGLES, 0, buffer->getSize(), transforms.size());
    }
}

void brl::GfxMaterial::setOverride(GfxShaderBinding pair)
{
    if (overrides.contains(pair.uniform))
    {
        overrides[pair.uniform] = pair.value;
    }
    else
    {
        overrides.push_back(pair);
    }
}

brl::GfxMaterialMgr* brl::GfxMaterialMgr::GetInstance()
{
    return GfxEngine::instance->materialMgr;
}

void brl::GfxMaterialMgr::InsertToRegistry(GfxMaterial* material)
{
    material->registryIndex = material_registry.size();
    material_registry.push_back(material);
}

void brl::GfxMaterialMgr::RemoveFromRegistry(GfxMaterial* material)
{
}

brl::GfxMaterialMgr::~GfxMaterialMgr()
{
    for (auto& materialRegistry : material_registry)
    {
        delete materialRegistry;
    }
}
