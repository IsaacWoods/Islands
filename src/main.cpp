/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#include <thread>
#include <chrono>
#include <platform.hpp>
#include <asset.hpp>
#include <rendering.hpp>

static inline float GetTime()
{
  static std::chrono::system_clock::time_point EPOCH = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - EPOCH).count() / 1e9f;
}

int main()
{
  const unsigned int width = 1920u;
  const unsigned int height = 1080u;
  InitPlatform(width, height, true, "Suku");
  Controller controller;

  Renderer renderer(width, height);
  Texture texture("./res/house.png");
  Mesh mesh(ParseMeshData("./res/house.dae", false));

  Entity* test = new Entity("test");
//  test->transform.rotation = Quaternion(Vec<3u>(0.5f, 1.0f, 0.0f), RADIANS(-55));
//  test->transform.rotation = Quaternion(Vec<3u>(0.0f, 1.0f, 0.0f), RADIANS(-45));
  test->transform.rotation *= Quaternion(Vec<3u>(1.0f, 0.0f, 0.0f), RADIANS(-90));
  //Print("Rotation of test entity: %v4\n", test->transform.rotation);
  test->AddComponent(new Renderable(&mesh, &texture));
  test->AddComponent(new Controllable(&controller));

  Area area("test");
  area.entities.push_back(test);

  const float FRAME_TIME = 1.0f / 60.0f;
  const float PROFILE_TIME = 1.0f;   // NOTE(Isaac): how often the FPS and frame time should be profiled (seconds)

  //TermHandle fpsCounterHandle = CreateTermHandle();
  float lastTime = GetTime();
  float unprocessedTime = 0.0f;
  float frameCounter = 0.0f;
  unsigned int frames = 0u;

  while (true)
  {
    float startTime = GetTime();
    float elapsedTime = startTime - lastTime;
    lastTime = startTime;
    bool shouldRender = false;

    unprocessedTime += elapsedTime;
    frameCounter += elapsedTime;

    if (frameCounter >= PROFILE_TIME)
    {
      float totalTime = (1000.0f * frameCounter) / static_cast<float>(frames);
      //Print(fpsCounterHandle, "Frame time: %f ms (%u fps)\n", totalTime, static_cast<unsigned int>(frames / PROFILE_TIME));
      frameCounter = 0.0f;
      frames = 0u;
    }

    while (unprocessedTime > FRAME_TIME)
    {
      PollWindowEvents(controller);

      if (controller.buttons[ControllerButton::CENTRAL] || g_keys[KEY_ESCAPE])
      {
        DestroyPlatform();
        return 0;
      }

      // --- Run a tick ---
      test->Update(FRAME_TIME);

      shouldRender = true;
      unprocessedTime -= FRAME_TIME;
    }

    if (shouldRender)
    {
      RenderFrame(renderer, area);
      frames++;
    }
    else
    {
      // NOTE(Isaac): we sleep for a little bit, allowing the OS to schedule other tasks
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
  }

  DestroyPlatform();
  return 0;
}
