// ARGB 8888 Format
namespace Patache
{
static constexpr std::uint8_t sShadowBottomHeight{ 12U };
static constexpr std::uint8_t sShadowBottomWidth{ 1U };
static constexpr std::uint8_t sShadowBottomSize{ 12U };       // W * H
static constexpr std::uint8_t sShadowBottomPixelCount{ 48U }; // W * H * 4

static constexpr std::uint32_t sShadowBottom[]{ 0xa4000000, 0x9f000000, 0x91000000, 0x82000000,
                                                0x74000000, 0x65000000, 0x57000000, 0x48000000,
                                                0x3a000000, 0x27000000, 0x19000000, 0x0b000000,
                                                0x00000000 };
}
