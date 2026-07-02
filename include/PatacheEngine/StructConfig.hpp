#ifndef STRUCTCONFIG_HPP
#define STRUCTCONFIG_HPP
namespace Patache
{
enum ChangeConfigFlagBits : std::uint8_t
{
  eNone                     = 0b00000000,
  eShowFatalErrorMessagebox = 0b00000001,
  eVsync                    = 0b00000100,
  eBitDepth10               = 0b00001000,
  eAddImageCount            = 0b00010000
};

struct Config
{
  // General
  bool showFatalErrorMessagebox{ true };

  // Raccoon Renderer
  bool         vsync{ true };
  bool         bitDepth10{ false };
  std::uint8_t addImageCount{ 0U };

  // Change
  std::uint8_t triggeredChange{ Patache::ChangeConfigFlagBits::eNone };
};
}
#endif
