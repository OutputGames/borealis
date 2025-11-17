#include "borealis/gfx/model.hpp"


#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "borealis/gfx/engine.hpp"
#include "borealis/gfx/shader.hpp"

brl::EcsEntity* brl::GfxModelNode::createEntity()
{
    auto entity = new EcsEntity;

    entity->name = name;
    entity->localPosition = position;
    entity->localRotation = rotation;
    entity->localScale = scale;

    for (int i = 0; i < childCount; ++i)
    {
        EcsEntity* e = children[i]->createEntity();
        e->setParent(entity);
    }

    for (int i = 0; i < meshCount; ++i)
    {
        GfxMesh* mesh = model->meshes[meshIndices[i]];

        auto renderer = new GfxMeshRenderer();
        renderer->name = mesh->name;
        renderer->mesh = mesh->buffer;
        renderer->material = model->materials[mesh->materialIndex]->
            createMaterial(GfxShaderProgram::GetDefaultShader());
        renderer->setParent(entity);
    }

    return entity;
}

brl::GfxMaterial* brl::GfxMaterialDescription::createMaterial(GfxShaderProgram* shader)
{
    auto material = new GfxMaterial(shader);

    material->setVec3("color", color_uniforms[{AI_MATKEY_BASE_COLOR}]);
    material->setTexture("tex", texture_uniforms[aiTextureType_DIFFUSE]);
    material->setTexture("nrm", texture_uniforms[aiTextureType_NORMALS]);

    return material;
}

void brl::GfxMaterialDescription::deriveColor(aiMaterial* material, std::string id, unsigned type, unsigned idx)
{
    aiColor3D v;

    material->Get(id.c_str(), type, idx, v);


    auto key = GfxMaterialKey{id, type, idx};

    color_uniforms.insert_or_assign(key, glm::vec3{v.r, v.g, v.b});

}

void brl::GfxMaterialDescription::deriveTexture(aiMaterial* material, unsigned type, const aiScene* scene)
{
    aiString path;

    material->GetTexture(static_cast<aiTextureType>(type), 0, &path);

    auto tex = scene->GetEmbeddedTexture(path.C_Str());

    if (tex)
    {
        // add internal texture loading

        int width, height, nrChannels;

        unsigned char* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(tex->pcData), tex->mWidth, &width,
                                                    &height, &nrChannels, 4);
        if (data)
        {
            GLenum format = GL_RGB;
            GLenum internalFormat = GL_RGB8;

            auto pcData = new Color32[width * height];


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
                    format = GL_RGBA;
                    internalFormat = GL_RGBA;
                    break;
                case 4:
                    format = GL_RGBA;
                    internalFormat = GL_RGBA8;
            }

            for (int i = 0; i < width * height * 4; i += 4)
            {

                unsigned char r = data[i]; // Red component
                unsigned char g = data[i + 1]; // Green component
                unsigned char b = data[i + 2]; // Blue component
                unsigned char a = data[i + 3]; // Alpha component
                pcData[i / 4] = Color32{r, g, b, a};
            }

            texture_uniforms.insert_or_assign(type, new GfxTexture2d(pcData, width, height));

        }
        else
        {
            exit(-1);
        }
        stbi_image_free(data);


    }
    else
    {
        // add external texture loading
    }
}

brl::GfxModel::GfxModel(std::string path)
{

    Assimp::Importer importer;

    IoFile file = readFileBinary(path);

    const aiScene* scene = importer.ReadFileFromMemory(file.data, file.dataSize,
                                                       aiProcess_Triangulate | aiProcess_FlipUVs |
                                                       aiProcess_RemoveRedundantMaterials | aiProcess_GenUVCoords);

    if (!scene)
    {
        std::cerr << importer.GetErrorString() << std::endl;
        exit(-1);
    }
    for (int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* aMesh = scene->mMeshes[i];

        std::vector<GfxVertex> vertices;

        for (int j = 0; j < aMesh->mNumVertices; ++j)
        {
            GfxVertex vtx{};

            {
                aiVector3D v = aMesh->mVertices[j];
                vtx.position = {v.x, v.y, v.z};
            }

            if (aMesh->HasNormals())
            {
                aiVector3D v = aMesh->mNormals[j];
                vtx.normal = {v.x, v.y, v.z};
            }

            if (aMesh->HasTextureCoords(0))
            {
                aiVector3D v = aMesh->mTextureCoords[0][j];
                vtx.uv = {v.x, 1.0 - v.y};
            }


            vertices.push_back(vtx);

        }

        std::vector<unsigned> indices;

        for (unsigned int i = 0; i < aMesh->mNumFaces; i++)
        {
            aiFace face = aMesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        auto mesh = new GfxMesh;

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

        mesh->buffer = attribBuffer;
        mesh->name = aMesh->mName.C_Str();

        mesh->materialIndex = aMesh->mMaterialIndex;

        meshes.push_back(mesh);

    }

    for (int i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* aiMaterial = scene->mMaterials[i];

        std::string name = aiMaterial->GetName().C_Str();

        auto desc = new GfxMaterialDescription;

        desc->deriveColor(aiMaterial, AI_MATKEY_BASE_COLOR);
        desc->deriveTexture(aiMaterial, aiTextureType_DIFFUSE, scene);
        desc->deriveTexture(aiMaterial, aiTextureType_NORMALS, scene);

        materials.push_back(desc);
    }

    rootNode = processNode(scene->mRootNode, scene);

}

brl::EcsEntity* brl::GfxModel::createEntity()
{
    return rootNode->createEntity();
}


brl::GfxModelNode* brl::GfxModel::processNode(aiNode* aNode, const aiScene* scene)
{
    auto node = new GfxModelNode;
    node->name = aNode->mName.C_Str();

    aiVector3D p, s;
    aiQuaternion r;

    aNode->mTransformation.Decompose(s, r, p);

    node->position = {p.x, p.y, p.z};
    node->rotation = {r.w, r.x, r.y, r.z};
    node->scale = {s.x, s.y, s.z};

    node->meshIndices = new unsigned int[aNode->mNumMeshes];
    for (int i = 0; i < aNode->mNumMeshes; ++i)
    {
        node->meshIndices[i] = aNode->mMeshes[i];
    }

    node->children = new GfxModelNode*[aNode->mNumChildren];
    for (int i = 0; i < aNode->mNumChildren; ++i)
    {
        node->children[i] = processNode(aNode->mChildren[i], scene);
    }

    node->childCount = aNode->mNumChildren;
    node->meshCount = aNode->mNumMeshes;

    node->model = this;

    return node;
}


void brl::GfxMeshRenderer::lateUpdate()
{
    EcsEntity::lateUpdate();

    GfxEngine::instance->insertCall(GfxDrawCall{material, mesh, calculateTransform()});
}
