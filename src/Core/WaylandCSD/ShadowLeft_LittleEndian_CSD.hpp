// ARGB 8888 Format

#define PATACHE_SHADOW_LEFT_HEIGHT     12
#define PATACHE_SHADOW_LEFT_WIDTH      1
#define PATACHE_SHADOW_LEFT_SIZE       12 // W * H
#define PATACHE_SHADOW_LEFT_PIXELCOUNT 48 // W * H * 4

// array size is 576
static constexpr std::uint32_t sShadowLeft[]{ 0x00000000, 0x0a000000, 0x1a000000, 0x28000000,
                                              0x3a000000, 0x49000000, 0x56000000, 0x65000000,
                                              0x73000000, 0x82000000, 0x91000000, 0x9f000000,
                                              0xA4000000 };
