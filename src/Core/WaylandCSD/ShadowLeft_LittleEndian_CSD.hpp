// ARGB 8888 Format

namespace Patache
{
static constexpr std::uint8_t sShadowLeftCSDHeight{ 12U };
static constexpr std::uint8_t sShadowLeftCSDWidth{ 1U };
static constexpr std::uint8_t sShadowLeftCSDSize{ 12U };       // W * H
static constexpr std::uint8_t sShadowLeftCSDPixelCount{ 48U }; // W * H * 4

// array size is 576
static constexpr std::uint32_t sShadowLeftCSD[]{ 0x00000000, 0x0a000000, 0x1a000000, 0x28000000,
                                                 0x3a000000, 0x49000000, 0x56000000, 0x65000000,
                                                 0x73000000, 0x82000000, 0x91000000, 0x9f000000,
                                                 0xA4000000 };
}
