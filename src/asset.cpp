/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#include <asset.hpp>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <platform.hpp>
#include <maths.hpp>

MeshData::MeshData(unsigned int numVertices, unsigned int numIndices)
  :numVertices(numVertices)
  ,numIndices(numIndices)
{
  vertices = new Vertex[numVertices];
  indices = new unsigned int[numIndices];
}

MeshData::~MeshData()
{
  delete[] vertices;
  delete[] indices;
}

MeshData ParseMeshData(const std::string& path, bool convertToRightHanded)
{
  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile(path,  aiProcess_GenSmoothNormals      |
                                                  aiProcess_FlipUVs               |
                                                  aiProcess_CalcTangentSpace      |
                                                  aiProcess_Triangulate           |
                                                  aiProcess_JoinIdenticalVertices |
                                                  aiProcess_SortByPType);

  if (!scene)
  {
    std::cerr << "Failed to load scene from path: " << path << std::endl;
    exit(1);
  }

  assert(scene->HasMeshes());
  assert(scene->mNumMeshes == 1u);
  aiMesh* mesh = scene->mMeshes[0u];

  assert(mesh->HasNormals());
  assert(mesh->HasTangentsAndBitangents());
  assert(mesh->HasTextureCoords(0u));

  // We know that there are 3 indices per face, because the mesh has been triangulated
  MeshData meshData(mesh->mNumVertices, mesh->mNumFaces * 3u);

  for (unsigned int i = 0u;
       i < meshData.numVertices;
       i++)
  {
    meshData.vertices[i] = Vertex(Vec<3u>(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
                                  Vec<2u>(mesh->mTextureCoords[0u][i].x, mesh->mTextureCoords[0u][i].y)/*,
                                  Vec<3u>(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
                                  Vec<3u>(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z)*/);
  }

  /*
   * Some tools use a left-handed coordinate system (i.e. Blender), and we can't read this using Assimp (even
   * tho it's in the Collada file), so we can flip it into RH here.
   */
  if (convertToRightHanded)
  {
    static const Mat<4u> TO_RH_MATRIX = Rotation<4u>(Quaternion(Vec<3u>(1.0f, 0.0f, 0.0f), RADIANS(-90.0f)));
/*    Print("TO_RH_MATRIX: %m4\n", TO_RH_MATRIX);
    Print("Test vec: %v4    Transformed vec: %v4\n", Vec<4u>(Vec<3u>(5.0f, 2.0f, 3.0f), 1.0f),
                                                     TO_RH_MATRIX * Vec<4u>(Vec<3u>(5.0f, 2.0f, 3.0f), 1.0f));*/

    for (unsigned int i = 0u;
         i < meshData.numVertices;
         i++)
    {
      Vec<4u> transformedPos = TO_RH_MATRIX * Vec<4u>(meshData.vertices[i].position, 0.0f);
//      Print("%v4 -> %v4\n", meshData.vertices[i].position, transformedPos);
      meshData.vertices[i].position = Vec<3u>(transformedPos[0u], transformedPos[1u], transformedPos[2u]);
    }
  }

  for (unsigned int i = 0u;
       i < mesh->mNumFaces;
       i++)
  {
    const aiFace& face = mesh->mFaces[i];
    assert(face.mNumIndices == 3u);

    meshData.indices[i*3u+0u] = face.mIndices[0u];
    meshData.indices[i*3u+1u] = face.mIndices[1u];
    meshData.indices[i*3u+2u] = face.mIndices[2u];
  }

  return meshData;
}
