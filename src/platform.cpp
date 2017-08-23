/*
 * Copyright (C) 2017, Isaac Woods.
 * See LICENCE.md
 */

// Enable this flag to not enable Curses (so we can look properly at stuff emitted by the driver)
//#define DISABLE_CURSES_LOGGING

#include <platform.hpp>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <cinttypes>
#include <SDL2/SDL.h>
#include <gl3w.hpp>
#include <maths.hpp>
#include <imgui/imgui.hpp>
#include <imgui/imgui_impl_sdl_gl3.hpp>

static SDL_Window*          g_window;
static SDL_GLContext        g_context;
static SDL_GameController*  g_controller;
static SDL_Haptic*          g_haptic;

void DestroyPlatform()
{
  // Destroy ImGui
  ImGui_ImplSdlGL3_Shutdown();

  // Destroy SDL
  SDL_GL_DeleteContext(g_context);
  SDL_DestroyWindow(g_window);
  SDL_Quit();
}

bool    g_keys[NUM_KEYBOARD_KEYS]         = {};
bool    g_mouseButtons[NUM_MOUSE_BUTTONS] = {};
Vec<2u> g_mousePosition                   = Vec<2u>(0.0f, 0.0f);

void InitPlatform(unsigned int width, unsigned int height, bool fullscreen, const char* windowTitle)
{
  // Install signal handler to clean up the platform stuff correctly
  signal(SIGTERM,
    [](int /*signal*/)
    {
      DestroyPlatform();
    });

  signal(SIGKILL,
    [](int /*signal*/)
    {
      DestroyPlatform();
    });

  // Create the window using SDL and load the OpenGL context
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,       8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,     8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,      8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,     8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,   32);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,    16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,   1);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  g_window = SDL_CreateWindow(windowTitle,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width,
                              height,
                              SDL_WINDOW_OPENGL);
  // NOTE(Isaac): "real" fullscreen screws everything up, so use "fake" windowed fullscreen instead
  SDL_SetWindowFullscreen(g_window, (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
  g_context = SDL_GL_CreateContext(g_window);
  SDL_GL_SetSwapInterval(1);

  // Load OpenGL functions
  if (gl3wInit())
  {
    fprintf(stderr, "FATAL: Failed to load OpenGL functions from context!\n");
    exit(1);
  }

  // Initialise ImGui
  ImGui_ImplSdlGL3_Init(g_window);
}

// Utility functions
void itoa(char* buffer, unsigned long int n, int base)
{
  unsigned long int temp = n;
  int i = 0;

  do
  {
    temp = n % base;
    buffer[i++] = (temp < 10u) ? ('0' + temp) : ('a' - 10u + temp);
  } while (n /= base);

  buffer[i--] = '\0';

  for (int j = 0u;
       j < i;
       i--)
  {
    temp = buffer[j];
    buffer[j] = buffer[i];
    buffer[i] = temp;
  }
}

char* LoadFileAsString(const char* path)
{
  FILE* file = fopen(path, "rb");

  if (!file)
  {
    fprintf(stderr, "Failed to read source file: %s\n", path);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  unsigned long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* contents = static_cast<char*>(malloc(length + 1));

  if (!contents)
  {
    fprintf(stderr, "Failed to allocate space for source file!\n");
    exit(1);
  }

  if (fread(contents, 1, length, file) != length)
  {
    fprintf(stderr, "Failed to read source file: %s\n", path);
    exit(1);
  }

  contents[length] = '\0';
  fclose(file);

  return contents;
}

const char* GetButtonName(ControllerButton button)
{
  switch (button)
  {
    case ControllerButton::CENTRAL:       return "CENTRAL";
    case ControllerButton::LEFT_CONTROL:  return "LEFT_CONTROL";
    case ControllerButton::RIGHT_CONTROL: return "RIGHT_CONTROL";
    case ControllerButton::Y:             return "Y";
    case ControllerButton::X:             return "X";
    case ControllerButton::A:             return "A";
    case ControllerButton::B:             return "B";
    case ControllerButton::LEFT_STICK:    return "LEFT_STICK";
    case ControllerButton::RIGHT_STICK:   return "RIGHT_STICK";
    case ControllerButton::D_LEFT:        return "D_LEFT";
    case ControllerButton::D_RIGHT:       return "D_RIGHT";
    case ControllerButton::D_UP:          return "D_UP";
    case ControllerButton::D_DOWN:        return "D_DOWN";
    case ControllerButton::LEFT_BUMPER:   return "LEFT_BUMPER";
    case ControllerButton::RIGHT_BUMPER:  return "RIGHT_BUMPER";

    default:
    {
      return "--INVALID_BUTTON--";
    } break;
  }
}

const char* GetAxisName(ControllerAxis axis)
{
  switch (axis)
  {
    case ControllerAxis::LEFT_STICK_X:  return "LEFT_STICK_X";
    case ControllerAxis::LEFT_STICK_Y:  return "LEFT_STICK_Y";
    case ControllerAxis::RIGHT_STICK_X: return "RIGHT_STICK_X";
    case ControllerAxis::RIGHT_STICK_Y: return "RIGHT_STICK_Y";
    case ControllerAxis::LEFT_TRIGGER:  return "LEFT_TRIGGER";
    case ControllerAxis::RIGHT_TRIGGER: return "RIGHT_TRIGGER";

    default:
    {
      return "--INVALID_AXIS--";
    } break;
  }
}

// Platform stuff
void PollWindowEvents(Controller& controller)
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSdlGL3_ProcessEvent(&event);

    switch (event.type)
    {
      case SDL_QUIT:
      {
        exit(0);
      } break;

      case SDL_KEYDOWN:
      {
        g_keys[static_cast<unsigned int>(event.key.keysym.scancode)] = true;
      } break;

      case SDL_KEYUP:
      {
        g_keys[static_cast<unsigned int>(event.key.keysym.scancode)] = false;
      } break;

      case SDL_MOUSEMOTION:
      {
        g_mousePosition = Vec<2u>(event.motion.x, event.motion.y);
      } break;

      case SDL_MOUSEBUTTONDOWN:
      {
        g_mouseButtons[event.button.button] = true;
      } break;

      case SDL_MOUSEBUTTONUP:
      {
        g_mouseButtons[event.button.button] = false;
      } break;

      case SDL_CONTROLLERDEVICEADDED:
      {
        int id = event.cdevice.which;

        if (SDL_IsGameController(id))
        {
          g_controller = SDL_GameControllerOpen(id);

          if (g_controller)
          {
            SDL_Joystick* joystick = SDL_GameControllerGetJoystick(g_controller);
            g_haptic = SDL_HapticOpenFromJoystick(joystick);

            if (SDL_HapticRumbleInit(g_haptic))
            {
              fprintf(stderr, "WARNING: Failed to initializise haptic feedback on the controller!\n");
            }
          }
        }
      } break;

      case SDL_CONTROLLERDEVICEREMOVED:
      {
        SDL_GameControllerClose(g_controller);
        g_controller = nullptr;
      } break;

      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
      {
        static const ControllerButton mapping[] =
        {
          ControllerButton::A,
          ControllerButton::B,
          ControllerButton::X,
          ControllerButton::Y,
          ControllerButton::LEFT_CONTROL,
          ControllerButton::CENTRAL,
          ControllerButton::RIGHT_CONTROL,
          ControllerButton::LEFT_STICK,
          ControllerButton::RIGHT_STICK,
          ControllerButton::LEFT_BUMPER,
          ControllerButton::RIGHT_BUMPER,
          ControllerButton::D_UP,
          ControllerButton::D_DOWN,
          ControllerButton::D_LEFT,
          ControllerButton::D_RIGHT,
        };

        controller.buttons[mapping[event.cbutton.button]] = (event.cbutton.state == SDL_PRESSED);
      } break;

      case SDL_CONTROLLERAXISMOTION:
      {
        static const ControllerAxis mapping[] =
        {
          ControllerAxis::LEFT_STICK_X,
          ControllerAxis::LEFT_STICK_Y,
          ControllerAxis::RIGHT_STICK_X,
          ControllerAxis::RIGHT_STICK_Y,
          ControllerAxis::LEFT_TRIGGER,
          ControllerAxis::RIGHT_TRIGGER,
        };

        controller.axes[mapping[event.caxis.axis]] = event.caxis.value;
      } break;

      default:
      {
      } break;
    }
  }
}

void RumbleController(float strength, unsigned int length)
{
  SDL_HapticRumblePlay(g_haptic, strength, length);
}

void PrepareFrame()
{
  ImGui_ImplSdlGL3_NewFrame(g_window);
}

void SwapWindowBuffer()
{
  SDL_GL_SwapWindow(g_window);
}

Controller::Controller()
  :buttons{false}
  ,axes{}
{
}
