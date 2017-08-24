/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#pragma once

#include <ostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <cmath>

/*
 * NOTE(Isaac): as far as I can work out, this is as much of pi as can be accurately represented by IEEE-754 SP
 */
#define PI 3.14159265358979323846264338327950288f
#define RADIANS(degrees) (degrees * PI / 180.0f)
#define DEGREES(radians) (radians * 180.0f / PI)

template<unsigned int N>
struct Vec
{
  float v[N];

  Vec()
    :v{}
  {
  }

  explicit Vec(float x, ...)
  {
    va_list args;
    va_start(args, x);

    v[0u] = x;

    for (unsigned int i = 1u;
         i < N;
         i++)
    {
      v[i] = static_cast<float>(va_arg(args, double));
    }

    va_end(args);
  }

  Vec(const Vec<N-1u>& r, float last)
  {
    for (unsigned int i = 0u;
         i < N-1u;
         i++)
    {
      v[i] = r[i];
    }

    v[N-1u] = last;
  }

  // NOTE: I was hoping that this was possible with SFINAE and std::enable_if or something, but idk
  float& x() { static_assert(N>0u, "Component 'x' does not exist");  return v[0u]; }
  float& y() { static_assert(N>1u, "Component 'y' does not exist");  return v[1u]; }
  float& z() { static_assert(N>2u, "Component 'z' does not exist");  return v[2u]; }
  float& w() { static_assert(N>3u, "Component 'w' does not exist");  return v[3u]; }

  float x() const { static_assert(N>0u, "Component 'x' does not exist");  return v[0u]; }
  float y() const { static_assert(N>1u, "Component 'y' does not exist");  return v[1u]; }
  float z() const { static_assert(N>2u, "Component 'z' does not exist");  return v[2u]; }
  float w() const { static_assert(N>3u, "Component 'w' does not exist");  return v[3u]; }

  float& operator[](unsigned int i)
  {
    assert(i < N);
    return v[i];
  }

  const float& operator[](unsigned int i) const
  {
    assert(i < N);
    return v[i];
  }

  Vec<N> operator+(const Vec<N>& v) const
  {
    Vec<N> r;
    
    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      r[i] = (*this)[i] + v[i];
    }

    return r;
  }

  Vec<N> operator-(const Vec<N>& v) const
  {
    Vec<N> r;
    
    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      r[i] = (*this)[i] - v[i];
    }

    return r;
  }

  Vec<N> operator*(const Vec<N>& v) const
  {
    Vec<N> r;
    
    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      r[i] = (*this)[i] * v[i];
    }

    return r;
  }

  Vec<N> operator*(float f) const
  {
    Vec<N> r;

    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      r[i] = (*this)[i] * f;
    }

    return r;
  }

  Vec<N> operator/(const Vec<N>& v) const
  {
    Vec<N> r;
    
    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      r[i] = (*this)[i] / v[i];
    }

    return r;
  }

  Vec<N> operator/(float f) const
  {
    Vec<N> r;

    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      r[i] = (*this)[i] / f;
    }

    return r;
  }

  void operator+=(const Vec<N>& v)
  {
    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      (*this)[i] += v[i];
    }
  }

  void operator-=(const Vec<N>& v)
  {
    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      (*this)[i] -= v[i];
    }
  }

  void operator*=(const Vec<N>& v)
  {
    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      (*this)[i] *= v[i];
    }
  }

  void operator/=(const Vec<N>& v)
  {
    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      (*this)[i] /= v[i];
    }
  }
};

template<unsigned int N>
std::ostream& operator<<(std::ostream& stream, const Vec<N>& v)
{
  stream << "(";
  for (unsigned int i = 0u;
       i < N-1u;
       i++)
  {
    stream << v[i] << ", ";
  }
  stream << v[N-1u] << ")";
  return stream;
}

template<unsigned int N>
Vec<N> operator*(float f, const Vec<N>& v)
{
  Vec<N> r;

  for (unsigned int i = 0u;
       i < N;
       i++)
  {
    r[i] = v[i] * f;
  }

  return r;
}

template<unsigned int N>
Vec<N>& operator*=(Vec<N>& v, float f)
{
  for (unsigned int i = 0u;
       i < N;
       i++)
  {
    v[i] *= f;
  }

  return v;
}

template<unsigned int N>
Vec<N> operator/(float f, const Vec<N>& v)
{
  Vec<N> r;

  for (unsigned int i = 0u;
       i < N;
       i++)
  {
    r[i] = v[i] / f;
  }

  return r;
}

template<unsigned int N>
Vec<N>& operator/=(Vec<N>& v, float f)
{
  for (unsigned int i = 0u;
       i < N;
       i++)
  {
    v[i] /= f;
  }

  return v;
}

template<unsigned int N>
bool operator==(const Vec<N>& a, const Vec<N>& b)
{
  for (unsigned int i = 0u;
       i < N;
       i++)
  {
    if (a[i] != b[i])
    {
      return false;
    }
  }

  return true;
}

template<unsigned int N>
bool operator!=(const Vec<N>& a, const Vec<N>& b)
{
  for (unsigned int i = 0u;
       i < N;
       i++)
  {
    if (a[i] == b[i])
    {
      return false;
    }
  }

  return true;
}

template<unsigned int N>
float LengthSq(const Vec<N>& v)
{
  float length = 0u;

  for (unsigned int i = 0u;
       i < N;
       i++)
  {
    length += v[i] * v[i];
  }

  return length;
}

template<unsigned int N>
float Length(const Vec<N>& v)
{
  return sqrt(LengthSq<N>(v));
}

template<unsigned int N>
Vec<N> Normalise(const Vec<N>& v)
{
  return v / Length<N>(v);
}

template<unsigned int N>
float Dot(const Vec<N>& a, const Vec<N>& b)
{
  float result = 0.0f;

  for (unsigned int i = 0u;
       i < N;
       i++)
  {
    result += a[i] * b[i];
  }

  return result;
}

template<unsigned int N>
Vec<N> Cross(const Vec<N>& a, const Vec<N>& b);

template<unsigned int N>
struct Mat
{
  float m[N][N];

  Mat()
    :m{0.0f}
  {
    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      this->m[i][i] = 1.0f;
    }
  }

  Mat(float f)
    :m{}
  {
    for (unsigned int y = 0u;
         y < N;
         y++)
    {
      for (unsigned int x = 0u;
           x < N;
           x++)
      {
        this->m[y][x] = f;
      }
    }
  }

  float* operator[](unsigned int i)
  {
    return this->m[i];
  }

  const float* operator[](unsigned int i) const
  {
    return this->m[i];
  }

  Mat<N> operator*(const Mat<N>& other) const
  {
    Mat<N> result(0.0f);

    for (unsigned int row = 0u;
         row < N;
         row++)
    {
      for (unsigned int column = 0u;
           column < N;
           column++)
      {
        for (unsigned int i = 0u;
             i < N;
             i++)
        {
          result[column][row] += (*this)[i][row] * other[column][i];
        }
      }
    }

    return result;
  }

  Vec<N> operator*(const Vec<N>& v) const
  {
    Vec<N> result(0.0f);

    for (unsigned int i = 0u;
         i < N;
         i++)
    {
      for (unsigned int j = 0u;
           j < N;
           j++)
      {
        result[i] += (*this)[j][i] * v[j];
      }
    }

    return result;
  }
};

struct Quaternion : Vec<4u>
{
  Quaternion()
    :Vec<4u>(0.0f, 0.0f, 0.0f, 1.0f)
  {
  }

  Quaternion(float x, float y, float z, float w)
    :Vec<4u>(x, y, z, w)
  {
  }

  Quaternion(const Vec<3u>& axis, float angle)
  {
    float sinHalfAngle = sin(angle / 2.0f);
    (*this)[0u] = axis[0u] * sinHalfAngle;
    (*this)[1u] = axis[1u] * sinHalfAngle;
    (*this)[2u] = axis[2u] * sinHalfAngle;
    (*this)[3u] = cos(angle / 2.0f);
  }

  /*
   * This implements the Hamilton produce, which is not the same as ordinary vector multiplication.
   * When two quaternions are multiplied together, it represents doing the LHS rotation, then the RHS.
   * It is not commutive (so ab!=ba)
   */
  Quaternion operator*(const Quaternion& other) const;
};

Quaternion& operator*=(Quaternion& q, const Quaternion& other);

template<unsigned int N>
char* GetMatrixString(const Mat<N>& m)
{
  char buffer[1024u];
  int length = 0u;

  for (unsigned int y = 0u;
       y < N;
       y++)
  {
    length += sprintf(buffer + length, "[");

    for (unsigned int x = 0u;
         x < N;
         x++)
    {
      length += sprintf(buffer + length, "% .3f ", m.m[x][y]);
    }

    length += sprintf(buffer + length, "]\n");
  }

  char* str = static_cast<char*>(malloc(sizeof(char) * length));
  strcpy(str, buffer);
  return str;
}

template<unsigned int N>
Mat<N> Scale(float scale)
{
  Mat<N> m;

  for (unsigned int i = 0u;
       i < N-1u;
       i++)
  {
    m[i][i] = scale;
  }

  m[N-1u][N-1u] = 1.0f;
  return m;
}

template<unsigned int N>
Mat<N> Translation(const Vec<N-1u>& t)
{
  Mat<N> m;

  for (unsigned int i = 0u;
       i < N-1u;
       i++)
  {
    m[N-1u][i] = t[i];
  }

  return m;
}

template<unsigned int N>
Mat<N> Rotation(const Vec<N-1u>& forward, const Vec<N-1u>& up, const Vec<N-1u>& right)
{
  static_assert(N == 4u, "Can only create rotations with 4x4 matrices");
  Mat<N> m;

  m[0u][0u] = right[0u];
  m[1u][0u] = right[1u];
  m[2u][0u] = right[2u];

  m[0u][1u] = up[0u];
  m[1u][1u] = up[1u];
  m[2u][1u] = up[2u];

  m[0u][2u] = forward[0u];
  m[1u][2u] = forward[1u];
  m[2u][2u] = forward[2u];

  return m;
}

template<unsigned int N>
Mat<N> Rotation(const Quaternion& q)
{
  static_assert(N == 4u, "Can only convert quaternion to a 4x4 matrix");

  Vec<N-1u> forward(        2.0f * (q[0u] * q[2u] - q[3u] * q[1u]),
                            2.0f * (q[1u] * q[2u] + q[3u] * q[0u]),
                     1.0f - 2.0f * (q[0u] * q[0u] + q[1u] * q[1u]));
  Vec<N-1u> up(             2.0f * (q[0u] * q[1u] + q[3u] * q[2u]),
                     1.0f - 2.0f * (q[0u] * q[0u] + q[2u] * q[2u]),
                            2.0f * (q[1u] * q[2u] - q[3u] * q[0u]));
  Vec<N-1u> right(   1.0f - 2.0f * (q[1u] * q[1u] + q[2u] * q[2u]),
                            2.0f * (q[0u] * q[1u] - q[3u] * q[2u]),
                            2.0f * (q[0u] * q[2u] + q[3u] * q[1u]));

  return Rotation<N>(forward, up, right);
}

template<unsigned int N>
Mat<N> PerspectiveProjection(float fov, float aspectRatio, float near, float far)
{
  static_assert(N == 4u, "Perspective projection matrices can only be 4x4!");
  Mat<N> result(0.0f);

  float tanHalfFOV = tan(fov / 2.0f);
  float zRange = far - near;

  result[0u][0u] = 1.0f / (aspectRatio * tanHalfFOV);
  result[1u][1u] = 1.0f / tanHalfFOV;
  result[2u][2u] = -(near + far) / zRange;
  result[3u][2u] = -(2.0f * near * far) / zRange;
  result[2u][3u] = -1.0f;

  return result;
}

template<unsigned int N>
Mat<N> OrthographicProjection(unsigned int width, unsigned int height, float near, float far)
{
  static_assert(N == 4u, "Orthographic projection matrices must be 4x4!");
  Mat<N> result(0.0f);

  result[0u][0u] = 2.0f / static_cast<float>(width);
  result[1u][1u] = 2.0f / static_cast<float>(height);
  result[2u][2u] = -2.0f / (far - near);
  result[3u][0u] = -1.0f;
  result[3u][1u] = -1.0f;
  result[3u][2u] = -1.0f;
  result[3u][3u] = 1.0f;

  return result;
}

struct Vertex
{
  Vertex() = default;
  Vertex(const Vec<3u>& position, const Vec<2u>& texCoord)
    :position(position)
    ,texCoord(texCoord)
  { }

  Vec<3u> position;
  Vec<2u> texCoord;
};

struct Transform
{
  Transform()
    :position()
    ,scale(1.0f)
    ,rotation()
  {
  }

  Vec<3u>     position;
  float       scale;
  Quaternion  rotation;
};

Mat<4u> CreateTransformation(const Transform& transform);
