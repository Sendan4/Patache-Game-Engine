#pragma once

namespace Patata
{
enum ChangeConfigFlagBits : std::uint8_t
{
  None                     = 0b00000000,
  ShowFatalErrorMessagebox = 0b00000001,
  Vsync                    = 0b00000100,
  BitDepth10               = 0b00001000,
  AddImageCount            = 0b00010000
};

struct Config
{
  // General
  bool ShowFatalErrorMessagebox = true;

  // Raccoon Renderer
  bool         Vsync         = true;
  bool         BitDepth10    = false;
  std::uint8_t AddImageCount = 0;

  // Change
  std::uint8_t TriggeredChange = Patata::ChangeConfigFlagBits::None;
};
}
