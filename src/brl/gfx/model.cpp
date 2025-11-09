#include "borealis/gfx/model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

brl::GfxModel::GfxModel(std::string path)
{
    Assimp::Importer importer;

    IoFile file = readFileBinary(path);

    const aiScene* scene = importer.ReadFileFromMemory(file.data, file.dataSize,
                                                       aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene)
    {
        std::cerr << importer.GetErrorString() << std::endl;
        return;
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

        meshes.push_back(mesh);

    }
}
