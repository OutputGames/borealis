#include "borealis/gfx/model.hpp"

/*
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
*/

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

    return material;
}

brl::GfxModel::GfxModel(std::string path)
{
    /*
    Assimp::Importer importer;

    IoFile file = readFileBinary(path);

    const aiScene* scene = importer.ReadFileFromMemory(file.data, file.dataSize,
                                                       aiProcess_Triangulate | aiProcess_FlipUVs);

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
                vtx.uv = {v.x, v.y};
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

        auto desc = new GfxMaterialDescription;

        materials.push_back(desc);
    }

    rootNode = processNode(scene->mRootNode, scene);
    */
}

brl::EcsEntity* brl::GfxModel::createEntity()
{
    return rootNode->createEntity();
}

/*
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
*/

void brl::GfxMeshRenderer::lateUpdate()
{
    EcsEntity::lateUpdate();

    GfxEngine::instance->insertCall(GfxDrawCall{material, mesh, calculateTransform()});
}
