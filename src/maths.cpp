/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#include <maths.hpp>

template<>
Vec<3u> Cross<3u>(const Vec<3u>& a, const Vec<3u>& b)
{
  return Vec<3u>( a[1u]*b[2u]-a[2u]*b[1u],
                  a[2u]*b[0u]-a[0u]*b[2u],
                  a[0u]*b[1u]-a[1u]*b[0u]);
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
  float w = (*this)[3u]*other[3u] - (*this)[0u]*other[0u] - (*this)[1u]*other[1u] - (*this)[2u]*other[2u];
  float x = (*this)[3u]*other[0u] + (*this)[0u]*other[3u] + (*this)[1u]*other[2u] - (*this)[2u]*other[1u];
  float y = (*this)[3u]*other[1u] - (*this)[0u]*other[2u] + (*this)[1u]*other[3u] + (*this)[2u]*other[0u];
  float z = (*this)[3u]*other[2u] + (*this)[0u]*other[1u] - (*this)[1u]*other[0u] + (*this)[2u]*other[3u];
  return Quaternion(x, y, z, w);
}

Quaternion& operator*=(Quaternion& q, const Quaternion& other)
{
  q = q * other;
  return q;
}

Mat<4u> CreateTransformation(const Transform& transform)
{
  return  Translation<4u>(transform.position) *
          Rotation<4u>(transform.rotation) *
          Scale<4u>(transform.scale);
}
