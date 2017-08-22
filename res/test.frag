/*
 * Copyright (C) 2016, Isaac Woods. All rights reserved.
 */

#version 330 core

layout (location = 0) out vec4 fragColor;

//in vec2 _texCoord;

//uniform sampler2D texture;

void main()
{
  //fragColor = texture2D(texture, _texCoord);
  fragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
