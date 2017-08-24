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
    ,centroid((a+b+c)/3.0f)
  { }

  Vec<2u> a;
  Vec<2u> b;
  Vec<2u> c;
  Edge    edge1;
  Edge    edge2;
  Edge    edge3;
  Vec<2u> centroid;

  bool HasVertex(const Vec<2u>& v) const;
  bool DoesCircumCircleContain(const Vec<2u>& v) const;
  bool operator==(const Triangle& other) const;
};

struct DelaunayPoint
{
  DelaunayPoint(const Vec<2u>& position)
    :position(position)
  { }

  Vec<2u> position;
  std::vector<DelaunayPoint*> neighbors;
};

struct PolygonPoint
{
  PolygonPoint(const Vec<2u>& position)
    :position(position)
  { }

  Vec<2u> position;
};

struct Polygon
{
  Polygon()
    :vertices()
  { }

  /*
   * These should be wound anti-clockwise.
   */
  std::vector<PolygonPoint> vertices;
};

struct PointGenerator
{
  PointGenerator(float width, float height)
    :width(width)
    ,height(height)
  { }
  virtual ~PointGenerator() = default;

  virtual std::vector<Vec<2u>> Generate() = 0;

  float width;
  float height;
};

struct RandomPointGenerator : PointGenerator
{
  RandomPointGenerator(float width, float height, unsigned int numPoints)
    :PointGenerator(width, height)
    ,numPoints(numPoints)
  { }

  std::vector<Vec<2u>> Generate() override;

  unsigned int numPoints;
};

struct JitteredPointGenerator : PointGenerator
{
  JitteredPointGenerator(float width, float height, unsigned int numColumns, unsigned int numRows)
    :PointGenerator(width, height)
    ,numColumns(numColumns)
    ,numRows(numRows)
  { }

  std::vector<Vec<2u>> Generate() override;

  unsigned int numColumns;
  unsigned int numRows;
};

struct World
{
  World(const std::string& name, PointGenerator* pointGenerator, float width, float height);
  ~World();

  void Render(Renderer& renderer);

  std::string           name;
  float                 width;
  float                 height;
  std::vector<Entity*>  entities;
private:
  std::vector<Vec<2u>>  points;

  std::vector<Triangle> triangles;
  std::vector<Edge>     edges;

  std::vector<Polygon>  polygons;

  GLuint                pointsVAO;
  GLuint                pointsVBO;

  GLuint                triangleEdgeVAO;
  GLuint                triangleEdgeVBO;

  GLuint                centroidVAO;
  GLuint                centroidVBO;

  GLuint                polygonVAO;
  GLuint                polygonVBO;
  unsigned int          numPolygonIndices;

  bool renderPoints;
  bool renderDelaunay;
  bool renderCentroids;
  bool renderPolygons;

  void DelaunayTriangulate();
  void FindPolygons();
};
