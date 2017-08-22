/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#pragma once

#include <gl3w.hpp>
#include <maths.hpp>
#include <asset.hpp>
#include <entity.hpp>

// --- Mesh ---
struct Mesh
{
  Mesh(const MeshData& meshData);
  ~Mesh();

  unsigned int numElements;
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
};

void DrawMesh(Mesh& mesh);

// --- Shaders ---
struct Shader
{
  Shader(const char* basePath);
  ~Shader();

  GLuint        handle;
  unsigned int  textureCount;
};

void UseShader(Shader& shader);
template<typename T> void SetUniform(Shader& shader, const char* name, const T& value);

// --- Textures ---
struct Texture
{
  Texture(const char* path);
  ~Texture();

  GLuint handle;
  unsigned int width;
  unsigned int height;
};

// --- Rendering ---
struct Renderer
{
  Renderer(unsigned int width, unsigned int height);

  void StartFrame();
  void RenderEntity(Entity* entity);
  void EndFrame();

  unsigned int  width;
  unsigned int  height;
  Shader        shader;
  Mat<4u>       projection;
};
