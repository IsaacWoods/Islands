/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#pragma once

#include <string>
#include <maths.hpp>

struct MeshData
{
  MeshData(unsigned int numVertices, unsigned int numIndices);
  ~MeshData();

  unsigned int  numVertices;
  Vertex*       vertices;
  unsigned int  numIndices;
  unsigned int* indices;
};

MeshData ParseMeshData(const std::string& path, bool convertToRightHanded=false);
