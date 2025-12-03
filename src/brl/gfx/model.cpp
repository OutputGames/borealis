#include "borealis/gfx/model.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include "borealis/gfx/engine.hpp"
#include "borealis/gfx/shader.hpp"

std::map<std::string, brl::GfxModel*> brl::GfxModel::cachedModels;

brl::GfxSubMesh::~GfxSubMesh()
{
    delete buffer;
}

brl::GfxMesh* brl::GfxMesh::GetPrimitive(GfxPrimitiveType type)
{
    switch (type)
    {
        case QUAD:
            return GfxEngine::instance->quadMesh;
            break;
        case CIRCLE:
            return GfxEngine::instance->circleMesh;
            break;
        case LINE:
            return GfxEngine::instance->lineMesh;
            break;
    }
    return nullptr;
}

brl::GfxMesh::GfxMesh(GfxAttribBuffer* buffer)
{
    subMeshes = new GfxSubMesh*{new GfxSubMesh{buffer, 0}};
    subMeshCount = 1;
}

brl::GfxMesh::~GfxMesh() { delete[] subMeshes; }

brl::EcsEntity* brl::GfxModelNode::createEntity()
{
    auto entity = new EcsEntity;

    entity->name = name;
    entity->localPosition = position;
    entity->localRotation = rotation;
    entity->localScale = scale;


    if (mesh > -1)
    {
        GfxMesh* _mesh = model->meshes[this->mesh];

        auto renderer = new GfxMeshRenderer();
        renderer->name = _mesh->name;
        renderer->mesh = _mesh;


        renderer->materials.clear();
        for (int i = 0; i < _mesh->subMeshCount; ++i)
        {
            auto subMesh = _mesh->subMeshes[i];
            renderer->materials.push_back(model->materials[subMesh->materialIndex]);
        }


        renderer->setParent(entity);

    }


    for (int i = 0; i < childCount; ++i)
    {
        EcsEntity* e = children[i]->createEntity();
        e->setParent(entity);
    }

    return entity;
}

brl::GfxMaterial* brl::GfxMaterialDescription::createMaterial(GfxShaderProgram* shader)
{
    auto material = new GfxMaterial(shader);


    material->setVec4("color", baseColorValue);
    material->setTexture("tex", baseColorTexture);


    return material;
}


brl::GfxModel::GfxModel(std::string path)
{
    cachedModels.insert_or_assign(path, this);

    IoFile file = readFileBinary(path);

    std::cout << file.dataSize << std::endl;

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadBinaryFromMemory(&model, &err, &warn, file.data, file.dataSize, "");

    file.free(true);

    if (!ret)
    {
        std::cout << "Model loading error: " << err << std::endl;
        exit(-1);
    }


    for (int i = 0; i < model.meshes.size(); ++i)
    {
        tinygltf::Mesh tmesh = model.meshes[i];


        auto mesh = new GfxMesh;
        mesh->subMeshCount = tmesh.primitives.size();
        mesh->subMeshes = new GfxSubMesh*[mesh->subMeshCount];


        for (int j = 0; j < tmesh.primitives.size(); ++j)
        {
            auto prim = tmesh.primitives[j];
            const tinygltf::Accessor& pos_accessor = model.accessors[prim.attributes["POSITION"]];
            const tinygltf::Accessor& nrm_accessor = model.accessors[prim.attributes["NORMAL"]];
            const tinygltf::Accessor& tx0_accessor = model.accessors[prim.attributes["TEXCOORD_0"]];

            const tinygltf::BufferView& pos_bufferView = model.bufferViews[pos_accessor.bufferView];
            const tinygltf::BufferView& nrm_bufferView = model.bufferViews[nrm_accessor.bufferView];
            const tinygltf::BufferView& tx0_bufferView = model.bufferViews[tx0_accessor.bufferView];

            const tinygltf::Buffer& pos_buffer = model.buffers[pos_bufferView.buffer];
            const tinygltf::Buffer& nrm_buffer = model.buffers[nrm_bufferView.buffer];
            const tinygltf::Buffer& tx0_buffer = model.buffers[tx0_bufferView.buffer];

            auto positions = reinterpret_cast<const float*>(&pos_buffer.data[pos_bufferView.byteOffset + pos_accessor.
                byteOffset]);
            auto normals = reinterpret_cast<const float*>(&nrm_buffer.data[nrm_bufferView.byteOffset + nrm_accessor.
                byteOffset]);
            auto tx0s = reinterpret_cast<const float*>(&tx0_buffer.data[tx0_bufferView.byteOffset + tx0_accessor.
                byteOffset]);

            std::vector<GfxVertex> vertices;

            for (size_t h = 0; h < pos_accessor.count; ++h)
            {

                GfxVertex vtx{};

                vtx.position = {positions[h * 3 + 0], positions[h * 3 + 1], positions[h * 3 + 2]};
                vtx.normal = {normals[h * 3 + 0], normals[h * 3 + 1], normals[h * 3 + 2]};
                vtx.uv = {tx0s[h * 2 + 0], 1.0 - tx0s[h * 2 + 1]};
                vertices.push_back(vtx);
            }

            std::vector<unsigned> indices;
            if (prim.indices >= 0)
            {
                const tinygltf::Accessor& indexAccessor = model.accessors[prim.indices];
                const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
                const tinygltf::Buffer& indexBuffer = model.buffers[indexView.buffer];

                const void* indexData = &indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset];

                for (size_t i = 0; i < indexAccessor.count; i++)
                {
                    uint32_t index = 0;

                    switch (indexAccessor.componentType)
                    {
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                            index = static_cast<const uint16_t*>(indexData)[i];
                            break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                            index = static_cast<const uint32_t*>(indexData)[i];
                            break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                            index = static_cast<const uint8_t*>(indexData)[i];
                            break;
                    }

                    indices.push_back(index);
                }
            }

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


            attribBuffer->insertAttribute(GfxAttribute{3, 8 * sizeof(float), static_cast<void*>(nullptr)});
            attribBuffer->insertAttribute(GfxAttribute{3, 8 * sizeof(float), (void*)(3 * sizeof(float))});
            attribBuffer->insertAttribute(GfxAttribute{2, 8 * sizeof(float), (void*)(6 * sizeof(float))});

            auto subMesh = new GfxSubMesh;
            subMesh->buffer = attribBuffer;
            subMesh->materialIndex = prim.material;

            mesh->subMeshes[j] = subMesh;

        }

        mesh->name = tmesh.name;


        meshes.push_back(mesh);
    }

    std::vector<GfxTexture2d*> textures;
    for (auto texture : model.textures)
    {
        auto image = model.images[texture.source];
        auto sampler = model.samplers[texture.sampler];

        auto pixels = new Color32[image.image.size()/2];
        for (int i = 0; i < image.image.size(); i += 4)
        {

            auto r = image.image[i];
            auto g = image.image[i + 1];
            auto b = image.image[i + 2];
            auto a = image.image[i + 3];

            pixels[i / 4] = {r,g,b,a};
        }

        textures.push_back(new GfxTexture2d(pixels, image.width, image.height));
    }

    for (int i = 0; i < model.materials.size(); ++i)
    {
        tinygltf::Material aiMaterial = model.materials[i];

        std::string name = aiMaterial.name;

        auto desc = new GfxMaterialDescription;

        if (aiMaterial.pbrMetallicRoughness.baseColorTexture.index > -1)
            desc->baseColorTexture = textures[aiMaterial.pbrMetallicRoughness.baseColorTexture.index];


        {
            auto val = aiMaterial.pbrMetallicRoughness.baseColorFactor;
            desc->baseColorValue = {val[0], val[1], val[2], val[3]};
        }

        materialDescriptions.push_back(desc);
    }

    for (auto materialDescription : materialDescriptions)
        materials.push_back(materialDescription->createMaterial(GfxShaderProgram::GetDefaultShader()));

    rootNode = new GfxModelNode;

    rootNode->childCount = model.scenes[0].nodes.size();
    rootNode->children = new GfxModelNode*[rootNode->childCount];
    rootNode->model = this;
    for (int i = 0; i < model.scenes[0].nodes.size(); ++i)
    {
        rootNode->children[i] = processNode(model.nodes[model.scenes[0].nodes[i]], model);
    }
}

brl::GfxModel* brl::GfxModel::loadModel(std::string path)
{
    if (cachedModels.contains(path))
        return cachedModels[path];

    return new GfxModel(path);
}

brl::EcsEntity* brl::GfxModel::createEntity()
{
    return rootNode->createEntity();
}


brl::GfxModelNode* brl::GfxModel::processNode(tinygltf::Node aNode, tinygltf::Model scene)
{
    auto node = new GfxModelNode;


    node->name = aNode.name;

    if (aNode.scale.size() == 3)
    {
        node->position = {aNode.translation[0], aNode.translation[1], aNode.translation[2]};
    }
    if (aNode.rotation.size() == 4)
    {

        node->rotation = glm::quat{static_cast<float>(aNode.rotation[0]), static_cast<float>(aNode.rotation[1]),
                                   static_cast<float>(aNode.rotation[2]), static_cast<float>(aNode.rotation[3])};
    }
    if (aNode.scale.size() == 3)
    {
        node->scale = {aNode.scale[0], aNode.scale[1], aNode.scale[2]};
    }

    node->mesh = aNode.mesh;

    node->children = new GfxModelNode*[aNode.children.size()];
    for (int i = 0; i < aNode.children.size(); ++i)
    {
        node->children[i] = processNode(scene.nodes[aNode.children[i]], scene);
    }

    node->childCount = aNode.children.size();


    node->model = this;

    return node;
}

brl::GfxMeshRenderer::GfxMeshRenderer() { materials.push_back(new GfxMaterial(GfxShaderProgram::GetDefaultShader())); }


void brl::GfxMeshRenderer::lateUpdate()
{
    EcsEntity::lateUpdate();

    for (int i = 0; i < mesh->subMeshCount; ++i)
    {
        GfxSubMesh* subMesh = mesh->subMeshes[i];
        GfxEngine::instance->insertCall(materials[i], subMesh->buffer, calculateTransform(), instancingID);
    }
}
