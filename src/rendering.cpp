/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#include <rendering.hpp>
#include <iostream>
#include <platform.hpp>
#include <gl3w.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.hpp>

// --- Meshes ---
Mesh::Mesh(const MeshData& meshData)
  :numElements(meshData.numIndices)
{
  glGenVertexArrays(1, &(this->vao));
  glBindVertexArray(this->vao);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  glGenBuffers(1, &(this->vbo));
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER, meshData.numVertices * sizeof(Vertex), meshData.vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, position)));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, texCoord)));

  glGenBuffers(1, &(this->ebo));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElements * sizeof(GLuint), meshData.indices, GL_STATIC_DRAW);

  glBindVertexArray(0);
}

Mesh::~Mesh()
{
  glDeleteBuffers(1, &(this->vbo));
  glDeleteBuffers(1, &(this->ebo));
  glDeleteVertexArrays(1, &(this->vao));
}

void DrawMesh(Mesh& mesh)
{
  glBindVertexArray(mesh.vao);
  glDrawElements(GL_TRIANGLES, mesh.numElements, GL_UNSIGNED_INT, 0);
}

// --- Shaders ---
enum ShaderPart
{
  VERTEX,
  FRAGMENT,
};

static GLuint LoadShaderPart(ShaderPart part, const char* path)
{
  GLuint handle = 0;

  switch (part)
  {
    case VERTEX:    handle = glCreateShader(GL_VERTEX_SHADER);    break;
    case FRAGMENT:  handle = glCreateShader(GL_FRAGMENT_SHADER);  break;
  }

  char* sourceString = LoadFileAsString(path);
  glShaderSource(handle, 1, &sourceString, nullptr);
  glCompileShader(handle);
  free(sourceString);

  GLint success = GL_FALSE;
  int messageLength;
  glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
  glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &messageLength);

  if (!success)
  {
    char message[messageLength + 1u];
    glGetShaderInfoLog(handle, messageLength, nullptr, message);
    std::cerr << "Failed to compile shader: " << message << std::endl;
  }

  return handle;
}

/*
 * Vertex shader should be located at {basePath}.vert
 * Fragment shader should be located at {basePath}.frag
 */
Shader::Shader(const char* basePath)
  :textureCount(0u)
{
  // Load the vertex shader
  char* vertexPath = static_cast<char*>(malloc(sizeof(char) * (strlen(basePath) + strlen(".vert") + 1u)));
  strcpy(vertexPath, basePath);
  strcat(vertexPath, ".vert");
  GLuint vertex = LoadShaderPart(ShaderPart::VERTEX, vertexPath);
  free(vertexPath);

  // Load the fragment shader
  char* fragmentPath = static_cast<char*>(malloc(sizeof(char) * (strlen(basePath) + strlen(".frag") + 1u)));
  strcpy(fragmentPath, basePath);
  strcat(fragmentPath, ".frag");
  GLuint fragment = LoadShaderPart(ShaderPart::FRAGMENT, fragmentPath);
  free(fragmentPath);

  // Create the shader program
  this->handle = glCreateProgram();
  glAttachShader(this->handle, vertex);
  glAttachShader(this->handle, fragment);
  glLinkProgram(this->handle);

  GLint success;
  int messageLength;
  glGetProgramiv(this->handle, GL_LINK_STATUS, &success);
  glGetProgramiv(this->handle, GL_INFO_LOG_LENGTH, &messageLength);

  if (!success)
  {
    char message[messageLength + 1u];
    glGetProgramInfoLog(this->handle, messageLength, nullptr, message);
    std::cerr << "Failed to link shader program: " << message << std::endl;
  }

  glDetachShader(this->handle, vertex);
  glDetachShader(this->handle, fragment);
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

Shader::~Shader()
{
  glDeleteProgram(this->handle);
}

void UseShader(Shader& shader)
{
  glUseProgram(shader.handle);
  shader.textureCount = 0u;
}

template<>
void SetUniform<Texture>(Shader& shader, const char* name, const Texture& value)
{
  glActiveTexture(GL_TEXTURE0 + shader.textureCount);
  glBindTexture(GL_TEXTURE_2D, value.handle);
  glUniform1i(glGetUniformLocation(shader.handle, name), shader.textureCount);
  shader.textureCount++;
}

template<>
void SetUniform<Vec<4u>>(Shader& shader, const char* name, const Vec<4u>& v)
{
  glUniform4f(glGetUniformLocation(shader.handle, name), v.x(), v.y(), v.z(), v.w());
}

template<>
void SetUniform<Mat<4u>>(Shader& shader, const char* name, const Mat<4u>& mat)
{
  glUniformMatrix4fv(glGetUniformLocation(shader.handle, name), 1, GL_FALSE, &(mat[0u][0u]));
}

// --- Textures ---
Texture::Texture(const char* path)
{
  glGenTextures(1, &(this->handle));
  glBindTexture(GL_TEXTURE_2D, this->handle);

  int iWidth, iHeight, componentsPerPixel;
  unsigned char* data = stbi_load(path, &iWidth, &iHeight, &componentsPerPixel, 0);
  this->width   = static_cast<unsigned int>(iWidth);
  this->height  = static_cast<unsigned int>(iHeight);

  switch (componentsPerPixel)
  {
    case 3u:
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } break;

    case 4u:
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } break;

    default:
    {
      std::cerr << "Failed to read texture: " << path << std::endl;
    } break;
  }

  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
  glDeleteTextures(1, &(this->handle));
}

// --- Renderer ---
Renderer::Renderer(unsigned int width, unsigned int height)
  :width(width)
  ,height(height)
  ,shader("./res/test")
//  ,projection(PerspectiveProjection<4u>(RADIANS(45.0f), (float)width / (float)height, 0.1f, 100.0f))
  ,projection(OrthographicProjection<4u>(1920.0f, 1080.0f, 0.1f, 100.0f))
{
  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//  glEnable(GL_DEPTH_TEST);

/*  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);*/
}

static Mat<4u> CalculateCameraViewMatrix(const Vec<3u>& cameraPos, const Vec<3u>& targetPos)
{
  const Vec<3u> up(0.0f, 1.0f, 0.0f);
  Mat<4u> result;

  Vec<3u> f = Normalise(targetPos - cameraPos);
  Vec<3u> s = Normalise(Cross(f, up));
  Vec<3u> u = Cross(s, f);

  result[0u][0u] =  s[0u];
  result[1u][0u] =  s[1u];
  result[2u][0u] =  s[2u];
  result[0u][1u] =  u[0u];
  result[1u][1u] =  u[1u];
  result[2u][1u] =  u[2u];
  result[0u][2u] = -f[0u];
  result[1u][2u] = -f[1u];
  result[2u][2u] = -f[2u];
  result[3u][0u] = -Dot(s, cameraPos);
  result[3u][1u] = -Dot(u, cameraPos);
  result[3u][2u] =  Dot(f, cameraPos);

  return result;
}

void Renderer::StartFrame()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  UseShader(shader);
  SetUniform(shader, "projection", projection);
}

void Renderer::RenderEntity(Entity* entity)
{
  const Renderable* renderable = entity->GetComponent<Renderable>();
  assert(renderable);

  if (renderable)
  {
    SetUniform(shader, "texture", *(renderable->texture));
    SetUniform(shader, "model", CreateTransformation(entity->transform));
    DrawMesh(*(renderable->mesh));
  }
}

void Renderer::EndFrame()
{
  SwapWindowBuffer();
}
