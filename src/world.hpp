/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

#include <string>
#include <vector>
#include <gl3w.hpp>
#include <maths.hpp>
#include <entity.hpp>
#include <rendering.hpp>

struct Edge
{
  Edge(const Vec<2u>& a, const Vec<2u>& b)
    :a(a)
    ,b(b)
  { }

  Vec<2u> a;
  Vec<2u> b;

  bool operator==(const Edge& other) const
  {
    return (a == other.a && b == other.b) ||
           (a == other.b && b == other.a);
  }
};

struct Triangle
{
  Triangle(const Vec<2u>& a, const Vec<2u>& b, const Vec<2u>& c)
    :a(a)
    ,b(b)
    ,c(c)
    ,edge1(Edge(a, b))
    ,edge2(Edge(b, c))
    ,edge3(Edge(c, a))
  { }

  Vec<2u> a;
  Vec<2u> b;
  Vec<2u> c;
  Edge    edge1;
  Edge    edge2;
  Edge    edge3;

  bool ContainsVertex(const Vec<2u>& v) const
  {
    return a == v || b == v || c == v;
  }

  bool DoesCircumCircleContain(const Vec<2u>& v) const
  {
    float aLengthSq = a.x()*a.x() + a.y()*a.y();
    float bLengthSq = b.x()*b.x() + b.y()*b.y();
    float cLengthSq = c.x()*c.x() + c.y()*c.y();

    float circumX = (aLengthSq * (c.y() - b.y()) + bLengthSq * (a.y() - c.y()) + cLengthSq * (b.y() - a.y())) /
                    (a.x() * (c.y() - b.y()) + b.x() * (a.y() - c.y()) + c.x() * (b.y() - a.y())) / 2.0f;
    float circumY = (aLengthSq * (c.x() - b.x()) + bLengthSq * (a.x() - c.x()) + cLengthSq * (b.x() - a.x())) /
                    (a.y() * (c.x() - b.x()) + b.y() * (a.x() - c.x()) + c.y() * (b.x() - a.x())) / 2.0f;
    float circumRadius = sqrtf((a.x() - circumX) * (a.x() - circumX) + (a.y() - circumY) * (a.y() - circumY));

    float dist = sqrt((v.x() - circumX) * (v.x() - circumX) + (v.y() - circumY) * (v.y() - circumY));
    return dist <= circumRadius;
  }

  bool operator==(const Triangle& other) const
  {
    return (a == other.a || a == other.b || a == other.c) &&
			     (b == other.a || b == other.b || b == other.c) && 
			     (c == other.a || c == other.b || c == other.c);
  }
};

struct World
{
  World(const std::string& name, unsigned int numPoints);
  ~World();

  void Render(Renderer& renderer);

  std::string           name;
  unsigned int          numPoints;
  std::vector<Entity*>  entities;
private:
  std::vector<Vec<2u>>  points;

  std::vector<Triangle> triangles;
  std::vector<Edge>     edges;
  std::vector<Vec<2u>>  centroids;

  GLuint                pointsVAO;
  GLuint                pointsVBO;
  GLuint                triangleEdgeVAO;
  GLuint                triangleEdgeVBO;
  GLuint                centroidVAO;
  GLuint                centroidVBO;

  void DelaunayTriangulate();
};
