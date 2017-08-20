/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

#include <string>
#include <vector>
#include <entity.hpp>

struct Area
{
  Area(const std::string& name);

  std::string           name;
  std::vector<Entity*>  entities;
};
