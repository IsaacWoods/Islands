/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

#include <thread>
#include <chrono>
#include <platform.hpp>
#include <asset.hpp>
#include <world.hpp>
#include <rendering.hpp>
#include <imgui/imgui.hpp>

static inline float GetTime()
{
  static std::chrono::system_clock::time_point EPOCH = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - EPOCH).count() / 1e9f;
}

int main()
{
  const unsigned int WIDTH = 1920;
  const unsigned int HEIGHT = 1080;
  const float FRAME_TIME = 1.0f / 60.0f;
  const float PROFILE_TIME = 1.0f;   // NOTE(Isaac): how often the FPS and frame time should be profiled (seconds)

  InitPlatform(WIDTH, HEIGHT, true, "Suku");
  Controller controller;
  Renderer renderer(WIDTH, HEIGHT);

  World world("test", 700u);

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
      //test->Update(FRAME_TIME);

      shouldRender = true;
      unprocessedTime -= FRAME_TIME;
    }

    if (shouldRender)
    {
      renderer.StartFrame();
      world.Render(renderer);

      renderer.EndFrame();
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
