/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#include <world.hpp>
#include <random>
#include <algorithm>

World::World(const std::string& name, unsigned int numPoints)
  :name(name)
  ,numPoints(numPoints)
  ,entities()
  ,points()
  ,triangles()
  ,edges()
{
  std::random_device randomDevice;
  std::mt19937 generator(randomDevice()); // Create a standard Mersenne Twister PRNG seeded with randomDevice
  std::uniform_real_distribution<> distribution(0, 1);

  for (unsigned int i = 0u;
       i < numPoints;
       i++)
  {
    points.push_back(Vec<2u>(distribution(generator) * 1920.0f, distribution(generator) * 1080.0f));
  }

  glGenVertexArrays(1, &pointsVAO);
  glBindVertexArray(pointsVAO);
  glGenBuffers(1, &pointsVBO);
  glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
  glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(Vec<2u>), &(points[0u]), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec<2u>), (void*)0);
  glEnableVertexAttribArray(0);

  DelaunayTriangulate();

  std::vector<GLfloat> edgeVertices;
  for (Edge& edge : edges)
  {
    edgeVertices.push_back(edge.a.x());
    edgeVertices.push_back(edge.a.y());
    edgeVertices.push_back(edge.b.x());
    edgeVertices.push_back(edge.b.y());
  }

  glGenVertexArrays(1, &triangleEdgeVAO);
  glBindVertexArray(triangleEdgeVAO);
  glGenBuffers(1, &triangleEdgeVBO);
  glBindBuffer(GL_ARRAY_BUFFER, triangleEdgeVBO);
  glBufferData(GL_ARRAY_BUFFER, edgeVertices.size() * sizeof(GLfloat), &(edgeVertices[0u]), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec<2u>), (void*)0);
  glEnableVertexAttribArray(0);

  std::vector<Vec<2u>> centroids;
  for (const Triangle& triangle : triangles)
  {
    centroids.push_back(triangle.centroid);
  }

  glGenVertexArrays(1, &centroidVAO);
  glBindVertexArray(centroidVAO);
  glGenBuffers(1, &centroidVBO);
  glBindBuffer(GL_ARRAY_BUFFER, centroidVBO);
  glBufferData(GL_ARRAY_BUFFER, centroids.size() * sizeof(Vec<2u>), &(centroids[0u]), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec<2u>), (void*)0);
  glEnableVertexAttribArray(0);
}

World::~World()
{
  for (Entity* entity : entities)
  {
    delete entity;
  }

  glDeleteBuffers(1, &pointsVBO);
  glDeleteVertexArrays(1, &pointsVAO);
}

void World::Render(Renderer& renderer)
{
  SetUniform(renderer.shader, "color", Vec<4u>(1.0, 0.0, 1.0, 1.0));
  glBindVertexArray(pointsVAO);
  glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
  glDrawArrays(GL_POINTS, 0, numPoints);

  glBindVertexArray(triangleEdgeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, triangleEdgeVBO);
  glDrawArrays(GL_LINES, 0, edges.size() * 4u);

  SetUniform(renderer.shader, "color", Vec<4u>(1.0, 1.0, 1.0, 1.0));
  glBindVertexArray(centroidVAO);
  glBindBuffer(GL_ARRAY_BUFFER, centroidVBO);
  glDrawArrays(GL_POINTS, 0, triangles.size());

  for (Entity* entity : entities)
  {
    if (entity->GetComponent<Renderable>())
    {
      renderer.RenderEntity(entity);
    }
  }
}

// Uses the Bowyer-Watson algorithm to find the Delaunay triangulation of a set of points
// TODO: Sometimes this randomly doesn't actually create a triangulation or doesn't include all of the points
void World::DelaunayTriangulate()
{
  // Create the super triangle
  Vec<2u> minPoint = points[0u];
  Vec<2u> maxPoint = minPoint;

  for (const Vec<2u>& point : points)
  {
    if (point.x() < minPoint.x()) minPoint.x() = point.x();
    if (point.y() < minPoint.y()) minPoint.y() = point.y();
    if (point.x() < maxPoint.x()) maxPoint.x() = point.x();
    if (point.y() < minPoint.y()) maxPoint.y() = point.y();
  }

  float dX = maxPoint.x() - minPoint.x();
  float dY = maxPoint.y() - minPoint.y();
  float dMax = std::max(dX, dY);
  float midX = (minPoint.x() + maxPoint.x()) / 2.0f;
  float midY = (minPoint.y() + maxPoint.y()) / 2.0f;

  Vec<2u> p1(midX - 20.0f * dMax, midY - dMax);
  Vec<2u> p2(midX,                midY + 20.0f * dMax);
  Vec<2u> p3(midX + 20.0f * dMax, midY - dMax);

  triangles.push_back(Triangle(p1, p2, p3));

  for (const Vec<2u>& point : points)
  {
    std::vector<Triangle> badTriangles;
    std::vector<Edge> polygonEdges;

    // Find the triangles that this new point disrupts
    for (Triangle triangle : triangles)
    {
      if (triangle.DoesCircumCircleContain(point))
      {
        badTriangles.push_back(triangle);
        polygonEdges.push_back(triangle.edge1);
        polygonEdges.push_back(triangle.edge2);
        polygonEdges.push_back(triangle.edge3);
      }
    }

    // Remove bad triangles from the main list
    triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
      [badTriangles](Triangle& triangle)
      {
        for (const Triangle& badTriangle : badTriangles)
        {
          if (badTriangle == triangle)
          {
            return true;
          }
        }
        return false;
      }), triangles.end());

    std::vector<Edge> badEdges;
    for (auto itA = polygonEdges.begin();
         itA != polygonEdges.end();
         itA++)
    {
      for (auto itB = polygonEdges.begin();
           itB != polygonEdges.end();
           itB++)
      {
        if (itA == itB)
        {
          continue;
        }

        if (*itA == *itB)
        {
          badEdges.push_back(*itA);
          badEdges.push_back(*itB);
        }
      }
    }

    polygonEdges.erase(std::remove_if(polygonEdges.begin(), polygonEdges.end(),
      [badEdges](const Edge& edge)
      {
        for (const Edge& badEdge : badEdges)
        {
          if (edge == badEdge)
          {
            return true;
          }
        }
        return false;
      }), polygonEdges.end());

    for (const Edge& polygonEdge : polygonEdges)
    {
      triangles.push_back(Triangle(polygonEdge.a, polygonEdge.b, point));
    }
  }

  triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
    [p1,p2,p3](const Triangle& triangle)
    {
      return triangle.ContainsVertex(p1) || triangle.ContainsVertex(p2) || triangle.ContainsVertex(p3);
    }), triangles.end());

  for (const Triangle& triangle : triangles)
  {
    edges.push_back(triangle.edge1);
    edges.push_back(triangle.edge2);
    edges.push_back(triangle.edge3);
  }
}
