// ARGB 8888 Format

#define PATACHE_SHADOW_BOTTOM_HEIGHT     12
#define PATACHE_SHADOW_BOTTOM_WIDTH      1
#define PATACHE_SHADOW_BOTTOM_SIZE       12 // W * H
#define PATACHE_SHADOW_BOTTOM_PIXELCOUNT 48 // W * H * 4

// array size is 576
static constexpr std::uint32_t sShadowBottom[]{ 0xa4000000, 0x9f000000, 0x91000000, 0x82000000,
                                                0x74000000, 0x65000000, 0x57000000, 0x48000000,
                                                0x3a000000, 0x27000000, 0x19000000, 0x0b000000,
                                                0x00000000 };
