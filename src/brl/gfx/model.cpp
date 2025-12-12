#include "borealis/gfx/model.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include "borealis/gfx/engine.hpp"
#include "borealis/gfx/shader.hpp"
#include "glm/gtx/matrix_decompose.hpp"

static brl::GfxShaderProgram* defaultSkinningShader = nullptr;
brl::GfxShaderProgram* GetSkinningShader()
{
    if (!defaultSkinningShader)
    {

        auto shaderBins = new brl::GfxShader*[2];

        auto vertexShaderSource = "#version 330 core\n"
                                  "layout (location = 0) in vec3 aPos;\n"
                                  "layout (location = 1) in vec3 aNorm;\n"
                                  "layout (location = 2) in vec2 aUV;\n"
                                  "layout (location = 3) in ivec4 aBoneIds;\n"
                                  "layout (location = 4) in vec4 aWeights;\n"
                                  "\n"
                                  "const int MAX_BONES = 100;\n"
                                  "const int MAX_BONE_INFLUENCE = 4;\n"
                                  "uniform mat4 _internalJoints[MAX_BONES];\n"
                                  "\n"
                                  "uniform mat4 _internalModel;\n"
                                  "uniform mat4 _internalView;\n"
                                  "uniform mat4 _internalProj;\n"
                                  "\n"
                                  "\n"
                                  "out vec2 texCoords;\n"
                                  "out vec3 normal;\n"
                                  "out vec3 pos;\n"
                                  "void main()\n"
                                  "{\n"
                                  "    vec4 totalPosition = vec4(0.0f);\n"
                                  "    vec3 totalNormal = vec3(0.0f);\n"
                                  "    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)\n"
                                  "    {\n"
                                  "        if(aBoneIds[i] == -1 || aWeights[i] == 0.0) \n"
                                  "            continue;\n"
                                  "        if(aBoneIds[i] >= MAX_BONES) \n"
                                  "        {\n"
                                  "            totalPosition = vec4(aPos,1.0f);\n"
                                  "            totalNormal = vec3(aNorm);\n"
                                  "            break;\n"
                                  "        }\n"
                                  "        vec4 localPosition = _internalJoints[aBoneIds[i]] * vec4(aPos,1.0f);\n"
                                  "        totalPosition += localPosition * aWeights[i];\n"
                                  "\n"
                                  "        vec3 localNormal = mat3(_internalJoints[aBoneIds[i]]) * aNorm;\n"
                                  "        totalNormal += localNormal * aWeights[i];\n"
                                  "    }\n"
                                  "\n"
                                  "    texCoords = aUV;\n"
                                  "\n"
                                  "    normal = mat3(transpose(inverse(_internalModel))) * totalNormal;\n"
                                  "    pos = totalPosition.xyz;\n"
                                  "\n"
                                  "    mat4 viewModel = _internalView * _internalModel;\n"
                                  "    gl_Position =  _internalProj * viewModel * totalPosition;\n"
                                  "}";

        auto fragmentShaderSource = "#version 330 core\n"
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
                                    "   FragColor = vec4(texture(tex,texCoords).rgb,1.0);\n"
                                    "}\n\0";

        shaderBins[0] = new brl::GfxShader(GL_VERTEX_SHADER, vertexShaderSource);
        shaderBins[1] = new brl::GfxShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        defaultSkinningShader = new brl::GfxShaderProgram(shaderBins, 2, true);
    }

    return defaultSkinningShader;
}

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

        GfxMeshRenderer* renderer = nullptr;

        if (skin > -1)
        {
            renderer = new GfxSkinnedMeshRenderer();
            ((GfxSkinnedMeshRenderer*)renderer)->skeletonIndex = skin;
        }
        else
        {
            renderer = new GfxMeshRenderer();
        }

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


    for (int i = 0; i < children.size(); ++i)
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

brl::GfxModel* brl::GfxModel::loadModel(std::string path)
{
    if (cachedModels.contains(path))
        return cachedModels[path];

    return new GfxModel(path);
}


brl::GfxModelEntity* brl::GfxModel::createEntity()
{
    auto rootNodeEntity = rootNode->createEntity();

    
    brl::GfxModelEntity* root = new brl::GfxModelEntity;
    root->model = this;

    if (skins.size() == 0)
    {
        int childCount = 0;
        auto children = rootNodeEntity->getChildren(childCount);

        for (int i = 0; i < childCount; ++i)
        {
            auto child = children[i];
            child->setParent(root);
        }
        return root;
    }


    // force all meshes under root entity
    {

        int childCount = 0;
        auto children = rootNodeEntity->getChild(0)->getChildren(childCount);

        for (int i = 0; i < childCount; ++i)
        {
            auto child = children[i];
            child->setParent(root);
        }

        delete[] children;
    }

    for (auto skin : skins)
    {
        GfxSkeleton* skeleton = new GfxSkeleton;
        skeleton->bones = skin->bones;

        skeleton->name = skin->name;
        skeleton->setParent(root);

        root->skeletons.push_back(skeleton);
    }

    return root;
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
            const tinygltf::Accessor& jnt_accessor = model.accessors[prim.attributes["JOINTS_0"]];
            const tinygltf::Accessor& wgt_accessor = model.accessors[prim.attributes["WEIGHTS_0"]];

            const tinygltf::BufferView& pos_bufferView = model.bufferViews[pos_accessor.bufferView];
            const tinygltf::BufferView& nrm_bufferView = model.bufferViews[nrm_accessor.bufferView];
            const tinygltf::BufferView& tx0_bufferView = model.bufferViews[tx0_accessor.bufferView];
            const tinygltf::BufferView& jnt_bufferView = model.bufferViews[jnt_accessor.bufferView];
            const tinygltf::BufferView& wgt_bufferView = model.bufferViews[wgt_accessor.bufferView];

            const tinygltf::Buffer& pos_buffer = model.buffers[pos_bufferView.buffer];
            const tinygltf::Buffer& nrm_buffer = model.buffers[nrm_bufferView.buffer];
            const tinygltf::Buffer& tx0_buffer = model.buffers[tx0_bufferView.buffer];
            const tinygltf::Buffer& jnt_buffer = model.buffers[jnt_bufferView.buffer];
            const tinygltf::Buffer& wgt_buffer = model.buffers[wgt_bufferView.buffer];

            auto positions = reinterpret_cast<const float*>(&pos_buffer.data[pos_bufferView.byteOffset + pos_accessor.
                byteOffset]);
            auto normals = reinterpret_cast<const float*>(&nrm_buffer.data[nrm_bufferView.byteOffset + nrm_accessor.
                byteOffset]);
            auto tx0s = reinterpret_cast<const float*>(&tx0_buffer.data[tx0_bufferView.byteOffset + tx0_accessor.
                byteOffset]);
            auto wght0s =
                reinterpret_cast<const float*>(&wgt_buffer.data[wgt_bufferView.byteOffset + wgt_accessor.byteOffset]);

            
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


            std::vector<GfxSkinnedVertex> vertices;

            for (size_t h = 0; h < pos_accessor.count; ++h)
            {

                GfxSkinnedVertex vtx{};

                vtx.position = {positions[h * 3 + 0], positions[h * 3 + 1], positions[h * 3 + 2]};
                vtx.normal = {normals[h * 3 + 0], normals[h * 3 + 1], normals[h * 3 + 2]};
                vtx.uv = {tx0s[h * 2 + 0], 1.0 - tx0s[h * 2 + 1]};
                vtx.weights = {wght0s[h * 4 + 0], wght0s[h * 4 + 1], wght0s[h * 4 + 2], wght0s[h * 4 + 3]};
                vertices.push_back(vtx);
            }

            // Joint IDs can be stored as different types
            if (jnt_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
            {
                // 8-bit unsigned integers
                for (size_t i = 0; i < jnt_accessor.count; ++i)
                {
                    const uint8_t* data = reinterpret_cast<const uint8_t*>(
                        &jnt_buffer.data[jnt_bufferView.byteOffset + i * jnt_accessor.byteOffset]);
                    vertices[i].boneIds = glm::uvec4(data[0], data[1], data[2], data[3]);
                }
            }
            else if (jnt_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                // 16-bit unsigned integers (most common)
                for (size_t i = 0; i < jnt_accessor.count; ++i)
                {
                    const uint16_t* data = reinterpret_cast<const uint16_t*>(
                        &jnt_buffer.data[jnt_bufferView.byteOffset + i * jnt_accessor.byteOffset]);
                    vertices[i].boneIds = glm::uvec4(data[0], data[1], data[2], data[3]);
                }
            }
            else
            {
                // no joints
            }

            auto attribBuffer = new GfxAttribBuffer();


            attribBuffer->use();

            auto buffer = new GfxBuffer(GL_ARRAY_BUFFER);
            buffer->use();
            buffer->updateData(GL_STATIC_DRAW, vertices.data(), sizeof(GfxSkinnedVertex) * vertices.size());

            auto elementBuffer = new GfxBuffer(GL_ELEMENT_ARRAY_BUFFER);
            elementBuffer->use();
            elementBuffer->updateData(GL_STATIC_DRAW, indices.data(), sizeof(unsigned) * indices.size());


            attribBuffer->assignBuffer(buffer);
            attribBuffer->assignElementBuffer(elementBuffer, GL_UNSIGNED_INT);


            attribBuffer->insertAttribute(GfxAttribute{3, sizeof(GfxSkinnedVertex), (void*)offsetof(GfxSkinnedVertex,position) });
            attribBuffer->insertAttribute(GfxAttribute{3, sizeof(GfxSkinnedVertex), (void*)offsetof(GfxSkinnedVertex, normal)});
            attribBuffer->insertAttribute(GfxAttribute{2, sizeof(GfxSkinnedVertex), (void*)offsetof(GfxSkinnedVertex, uv)});
            if (model.skins.size() > 0)
            {
                attribBuffer->insertAttribute(
                    GfxAttribute{4, sizeof(GfxSkinnedVertex), (void*)offsetof(GfxSkinnedVertex, boneIds), GL_INT});
                attribBuffer->insertAttribute(
                    GfxAttribute{4, sizeof(GfxSkinnedVertex), (void*)offsetof(GfxSkinnedVertex, weights)});
            }

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

    if (model.skins.size() > 0)
    {

        for (auto materialDescription : materialDescriptions)
            materials.push_back(materialDescription->createMaterial(GetSkinningShader()));
    }
    else
    {
        for (auto materialDescription : materialDescriptions)
            materials.push_back(materialDescription->createMaterial(GfxShaderProgram::GetDefaultShader()));
    }
    rootNode = new GfxModelNode;
    rootNode->model = this;
    for (int i = 0; i < model.scenes[0].nodes.size(); ++i)
    {
        auto child = processNode(model.nodes[model.scenes[0].nodes[i]], model, model.scenes[0].nodes[i]);
        if (child)
            rootNode->children.push_back(child);
    }

    for (const auto& skin : model.skins)
    {
        auto s = new GfxSkin;

        s->name = skin.name;

        for (int joint : skin.joints)
        {
            auto bone = new GfxBone;
            auto node = model.nodes[joint];

            bone->name = node.name;

            if (node.translation.size() == 3)
            {
                bone->position = {node.translation[0], node.translation[1], node.translation[2]};
            }
            if (node.rotation.size() == 4)
            {

                bone->rotation = glm::make_quat(node.rotation.data());
            }
            if (node.scale.size() == 3)
            {
                bone->scale = {node.scale[0], node.scale[1], node.scale[2]};
            }


            for (int child : node.children)
            {
                for (int j = 0; j < skin.joints.size(); ++j)
                {
                    if (skin.joints[j] == child)
                    {
                        bone->children.push_back(j);
                    }
                }
            }

            s->bones.push_back(bone);
        }

        for (int j = 0; j < s->bones.size(); ++j)
        {
            auto gfx_bone = s->bones[j];
            for (int i = 0; i < gfx_bone->children.size(); ++i)
            {
                s->bones[gfx_bone->children[i]]->parent = j;
            }
        }

        const tinygltf::Accessor& ibmAccessor = model.accessors[skin.inverseBindMatrices];
        const tinygltf::BufferView& ibmBufferView = model.bufferViews[ibmAccessor.bufferView];
        const tinygltf::Buffer& ibmBuffer = model.buffers[ibmBufferView.buffer];
        int ibmAccessorIndex = skin.inverseBindMatrices;

        if (ibmAccessorIndex < 0)
        {
            // No inverse bind matrices specified - use identity matrices
            // This is rare but valid in glTF
            std::vector<glm::mat4> inverseBindMatrices(skin.joints.size(), glm::mat4(1.0f));
        }
        else
        {
            // Get the accessor
            const tinygltf::Accessor& accessor = model.accessors[ibmAccessorIndex];

            // Get the buffer view
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];

            // Get the buffer
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            // Calculate the actual byte offset
            size_t byteOffset = bufferView.byteOffset + accessor.byteOffset;

            // The data is stored as an array of 4x4 matrices (floats)
            const float* matrices = reinterpret_cast<const float*>(&buffer.data[byteOffset]);

            // There should be one matrix per joint
            size_t numJoints = skin.joints.size();
            assert(accessor.count == numJoints);

            for (size_t i = 0; i < numJoints; ++i)
            {

                s->bones[i]->inverseBindMatrix = glm::make_mat4(&matrices[i * 16]);
            }
        }

        s->initialize();

        skins.push_back(s);
    }

    //https://github.com/mikelma/craftium/blob/7594e8af5637cd77da18bd33520a4907b9a19c6d/irr/src/CGLTFMeshFileLoader.cpp#L646
    //https://github.khronos.org/glTF-Tutorials/gltfTutorial/gltfTutorial_007_Animations.html
    // https://github.khronos.org/glTF-Tutorials/gltfTutorial/gltfTutorial_006_SimpleAnimation.html
    // https://github.com/raysan5/raylib/blob/8fa5f1fe2cf7efeda59a5d935a259ccb1cb97f1c/src/rmodels.c#L6347
    //https://raw.githubusercontent.com/KhronosGroup/glTF/refs/heads/main/specification/2.0/figures/gltfOverview-2.0.0d.png

    for (tinygltf::Animation anim : model.animations) {

        GfxAnimation* animation = new GfxAnimation();

        for (tinygltf::AnimationChannel channel : anim.channels) {

            GfxAnimation::Channel animChannel{};

            tinygltf::AnimationSampler sampler = anim.samplers[channel.sampler];

            int targetSkin = -1;
            int targetBone = -1;

            for (int j = 0; j < model.skins.size(); j++)
            {
                for (int i = 0; i <model.skins[j].joints.size(); i++) {
                    if (model.skins[j].joints[i] == channel.target_node) {
                        targetBone = i;
                        targetSkin = j;
                        break;
                    }
                }

                if (targetBone != -1) {
                    break;
                }
            }
            

            if (targetBone == -1 || targetSkin == -1)
                continue;

            if (sampler.interpolation == "LINEAR")
                animChannel.interpolation = GfxAnimation::LINEAR;
            if (sampler.interpolation == "STEP")
                animChannel.interpolation = GfxAnimation::STEP;
            if (sampler.interpolation == "CUBICSPLINE")
                animChannel.interpolation = GfxAnimation::CUBICSPLINE;

            const tinygltf::Accessor& inputAccessor = model.accessors[sampler.input];
            const tinygltf::Accessor& outputAccessor = model.accessors[sampler.output];

            const tinygltf::BufferView& input_bufferView = model.bufferViews[inputAccessor.bufferView];
            const tinygltf::Buffer& input_buffer = model.buffers[input_bufferView.buffer];

            const tinygltf::BufferView& output_bufferView = model.bufferViews[outputAccessor.bufferView];
            const tinygltf::Buffer& output_buffer = model.buffers[output_bufferView.buffer];


            auto times = reinterpret_cast<const float*>(&input_buffer.data[input_bufferView.byteOffset + inputAccessor.byteOffset]);
            int frameCount = inputAccessor.count;

            auto values = reinterpret_cast<const float*>(&output_buffer.data[output_bufferView.byteOffset + outputAccessor.byteOffset]);


            if (animChannel.type == GfxAnimation::TRANSLATION || animChannel.type == GfxAnimation::SCALE) {

                for (std::size_t i = 0; i < frameCount; ++i) {
                    float frame = times[i];
                    glm::vec3 value = {values[i * 3 + 0], values[i * 3 + 1], values[i * 3 + 2]};
                    
                    animChannel.frames.push_back(GfxAnimation::Vec3AnimationFrame{frame,value});
                }
            } else if (animChannel.type == GfxAnimation::ROTATION) 
            {
                for (std::size_t i = 0; i < frameCount; ++i) {
                    float frame = times[i];
                    glm::quat value = {values[i * 4 + 0], values[i * 4 + 1], values[i * 4 + 2], values[i*4+3]};
                    
                    animChannel.frames.push_back(GfxAnimation::QuatAnimationFrame{frame,value});
                }
            }

            animation->channels.push_back(animChannel);
        }

        animations.push_back(animation);
    }
}

brl::GfxModelNode* brl::GfxModel::processNode(tinygltf::Node aNode, tinygltf::Model scene, int index)
{
    for (const auto& skin : scene.skins)
    {
        if (std::find(skin.joints.begin(), skin.joints.end(), index) != skin.joints.end())
        {
            return nullptr;
        }
    }

    auto node = new GfxModelNode;

    node->name = aNode.name;

    if (aNode.translation.size() == 3)
    {
        node->position = {aNode.translation[0], aNode.translation[1], aNode.translation[2]};
    }
    if (aNode.rotation.size() == 4)
    {

        node->rotation = glm::make_quat(aNode.rotation.data());
    }
    if (aNode.scale.size() == 3)
    {
        node->scale = {aNode.scale[0], aNode.scale[1], aNode.scale[2]};
    }

    node->mesh = aNode.mesh;
    node->skin = aNode.skin;

    for (int i = 0; i < aNode.children.size(); ++i)
    {
        auto child = processNode(scene.nodes[aNode.children[i]], scene, aNode.children[i]);
        if (child)
            node->children.push_back(child);
    }



    node->model = this;

    return node;
}


std::map<std::string, brl::GfxModel*> brl::GfxModel::cachedModels;

brl::GfxMeshRenderer::GfxMeshRenderer() { materials.push_back(new GfxMaterial(GfxShaderProgram::GetDefaultShader())); }

    // Fix: create a named lvalue for the empty map to satisfy non-const reference requirement
static brl::GfxUniformList emptyUniforms;

void brl::GfxMeshRenderer::lateUpdate()
{
    EcsEntity::lateUpdate();

    for (int i = 0; i < mesh->subMeshCount; ++i)
    {
        GfxSubMesh* subMesh = mesh->subMeshes[i];
        GfxEngine::instance->insertCall(materials[i], subMesh->buffer, calculateTransform(), emptyUniforms, instancingID);
    }
}



glm::mat4 brl::GfxBone::calculateLocalTransform()
{
    glm::mat4 t(1.0);

    t = translate(t, position);
    t *= glm::toMat4(rotation);
    t = glm::scale(t, scale);


    return t;
}

void brl::GfxSkin::initialize()
{
    for (int i = 0; i < bones.size(); ++i)
    {
        const auto& bone = bones[i];
        if (bone->parent == -1)
        {
            _calcTransform(bone, glm::mat4(1.0), i);
        }
    }
}

void brl::GfxSkin::_calcTransform(brl::GfxBone* bone, const glm::mat4& parentTransform, int index)
{
    glm::mat4 t = bone->calculateLocalTransform();

    t = parentTransform * t;


    bone->worldMatrix = t;
    //bone->inverseBindMatrix = glm::inverse(bone->worldMatrix);

    for (int child : bone->children)
    {
        _calcTransform(bones[child], t, child);
    }
}


void brl::GfxSkeleton::earlyUpdate()
{
    EcsEntity::earlyUpdate();

    if (jointMatrices.size() < bones.size())
    {
        jointMatrices.clear();
        jointMatrices.reserve(bones.size());
        for (int i = 0; i < bones.size(); i++)
            jointMatrices.push_back(glm::mat4(1.0f));
    }
}

std::vector<glm::mat4> brl::GfxSkeleton::calculateTransforms()
{

    for (int i = 0; i < bones.size(); ++i)
    {
        const auto& bone = bones[i];
        if (bone->parent == -1)
        {
            _calcTransform(bone, glm::mat4(1.0), i, false);
        }
    }

    return jointMatrices;
}

void brl::GfxSkeleton::_calcTransform(brl::GfxBone* bone, const glm::mat4& parentTransform, int index, bool parentChanged)
{
    bool _changed = bone->changed || parentChanged;

    if (_changed)
    {

        glm::mat4 t = bone->calculateLocalTransform();

        t = parentTransform * t;
        
        bone->worldMatrix = t;
        jointMatrices[index] = t * bone->inverseBindMatrix;
    }

    for (int child : bone->children)
    {
        _calcTransform(bones[child], bone->worldMatrix, child, _changed);
    }

    bone->changed = false;
}

brl::GfxSkinnedMeshRenderer::GfxSkinnedMeshRenderer()
{


    materials[0] = new GfxMaterial(defaultSkinningShader);
}

void brl::GfxSkinnedMeshRenderer::start()
{
    GfxMeshRenderer::start();

    model = getEntityInParent<GfxModelEntity>();
}

void brl::GfxSkinnedMeshRenderer::lateUpdate()
{
    EcsEntity::lateUpdate();

    auto matrices = model->skeletons[skeletonIndex]->calculateTransforms();


    for (int i = 0; i < mesh->subMeshCount; ++i)
    {

        GfxUniformList overrides;
        auto jointValue = std::make_shared<GfxShaderValue>();
        jointValue->m4value = CONVERT_UNIFORM_MAT4(matrices);


        overrides.push_back({materials[i]->getShader()->getUniform("_internalJoints[0]"), jointValue});

        GfxSubMesh* subMesh = mesh->subMeshes[i];
        GfxEngine::instance->insertCall(materials[i], subMesh->buffer, calculateTransform(), overrides, instancingID);
    }
}

void brl::GfxAnimator::update()
{
    const auto& skeleton = model->skeletons[0];


    
}