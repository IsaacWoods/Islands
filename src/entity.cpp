/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#include <entity.hpp>
#include <cstdarg>
#include <limits>
#include <platform.hpp>

Component::Component()
  :parent(nullptr)
{
}

Renderable::Renderable(Mesh* mesh, Texture* texture)
  :mesh(mesh)
  ,texture(texture)
{
}

void Renderable::Update(float /*delta*/)
{
}

Controllable::Controllable(Controller* controller)
  :controller(controller)
{
}

// TODO: Make this stuff accessible from ImGui
void Controllable::Update(float delta)
{
  static const float SPEED = 0.5f;
  /*
   * This dead-zone seems quite high (probably my controller is just crap quality), so maybe allow the player to set this in the settings??
   */
  static const float DEADZONE = 0.21f;
  Vec<3u> d;

#if 1
  /*
   * NOTE(Isaac): The divide moves each coordinate into (0.0-1.0)
   */
  Vec<2u> stick = Vec<2u>(static_cast<float>(controller->axes[LEFT_STICK_X]),
                          static_cast<float>(controller->axes[LEFT_STICK_Y]))
                        / static_cast<float>(std::numeric_limits<int16_t>::max());;

  //static TermHandle lengthHandle = CreateTermHandle();
  //Print(lengthHandle, "Length: %f\n", Length(stick));

  //static TermHandle stickTermHandle = CreateTermHandle();
  //Print(stickTermHandle, "Stick: %v2\n", stick);

  //static TermHandle movingHandle = CreateTermHandle();

  if (Length(stick) < DEADZONE)
  {
    d = Vec<3u>(0.0f);
    //Print(movingHandle, "Moving: no\n");
  }
  else
  {
    //static TermHandle multiplierHandle = CreateTermHandle();
    //Print(multiplierHandle, "Multiplier: %f\n", (Length(stick) - DEADZONE) / (1.0f - DEADZONE));
    stick = Normalise(stick) * ((Length(stick) - DEADZONE) / (1.0f - DEADZONE));
    d = Vec<3u>(stick[0u], 0.0f, stick[1u]) * SPEED * delta;
    //Print(movingHandle, "Moving: yes\n");
  }
#else
  if (g_keys[KEY_W])  d -= Vec<3u>(0.0f, 0.0f, SPEED * delta);
  if (g_keys[KEY_S])  d += Vec<3u>(0.0f, 0.0f, SPEED * delta);
  if (g_keys[KEY_A])  d -= Vec<3u>(SPEED * delta, 0.0f, 0.0f);
  if (g_keys[KEY_D])  d += Vec<3u>(SPEED * delta, 0.0f, 0.0f);
#endif

  parent->transform.position += d;
  //static TermHandle positionTermHandle = CreateTermHandle();
  //Print(positionTermHandle, "Controllable position: %v3\n", parent->transform.position);
}

Entity::Entity(const std::string& name)
  :name(name)
  ,componentMap()
{
}

void Entity::Update(float delta)
{
  for (auto& mapping : componentMap)
  {
    (mapping.second)->Update(delta);
  }
}
