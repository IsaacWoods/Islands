/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#pragma once

#include <maths.hpp>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <platform.hpp>

struct Mesh;
struct Texture;
struct Entity;

struct Component
{
  Component();
  virtual ~Component() { }

  virtual void Update(float delta) = 0;

  Entity* parent;
};

struct Renderable : Component
{
  Renderable(Mesh* mesh, Texture* texture);

  void Update(float delta);

  Mesh*     mesh;
  Texture*  texture;
};

struct Controllable : Component
{
  Controllable(Controller* controller);

  void Update(float delta);

  Controller* controller;
};

struct Entity
{
  Entity(const std::string& name);

  void Update(float delta);

  template<typename T>
  T* GetComponent()
  {
    return dynamic_cast<T*>(componentMap[typeid(T)]);
  }

  template<typename T>
  const T* GetComponent() const
  {
    return dynamic_cast<T*>(componentMap.at(typeid(T)));
  }

  template<typename T>
  void AddComponent(T* component)
  {
    static_assert(std::is_base_of<Component, T>::value, "Can only add component if base class is Component");
    componentMap[typeid(T)] = component;
    component->parent = this;
  }

  std::string                                     name;
  Transform                                       transform;
  std::unordered_map<std::type_index, Component*> componentMap;
};
