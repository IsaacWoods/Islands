/*
 * Copyright (C) 2016, Isaac Woods. All rights reserved.
 */

#version 330 core

layout (location = 0) out vec4 fragColor;

uniform vec4 color;

void main()
{
  fragColor = color;
}
