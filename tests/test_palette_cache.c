#include "port_common.h"
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    PortPaletteCache c={0}, before; uint32_t source[256]={0}, copy[256];
    assert(port_palette_cache_rgb888(&c,source,256));
    assert(c.updates==1 && c.rgba[0]==255 && c.rgba[255]==255);
    memcpy(copy,source,sizeof(copy)); assert(port_palette_cache_rgb888(&c,copy,256) && c.updates==1);
    memcpy(&before,&c,sizeof(c));
    assert(!port_palette_cache_rgb888(NULL,source,256));
    assert(!port_palette_cache_rgb888(&c,NULL,256));
    assert(!port_palette_cache_rgb888(&c,source,0));
    assert(!port_palette_cache_rgb888(&c,source,257));
    assert(!memcmp(&before,&c,sizeof(c)));
    SDL_PixelFormat *format=SDL_AllocFormat(SDL_PIXELFORMAT_RGB888); assert(format);
    unsigned random=91;
    for(unsigned batch=0;batch<128;++batch) {
        for(unsigned i=0;i<256;++i) { random=random*1664525u+1013904223u; source[i]=random; }
        memcpy(copy,source,sizeof(copy));
        uint64_t updates=c.updates;
        assert(port_palette_cache_rgb888(&c,source,256) && c.updates==updates+1);
        for(unsigned i=0;i<256;++i) {
            Uint8 r,g,b; SDL_GetRGB(source[i],format,&r,&g,&b);
            assert(c.rgba[i]==(((uint32_t)r<<24)|((uint32_t)g<<16)|((uint32_t)b<<8)|255u));
        }
        assert(!memcmp(source,copy,sizeof(copy)));
        assert(port_palette_cache_rgb888(&c,source,256) && c.updates==updates+1);
    }
    source[255]^=0xffffffu; uint64_t updates=c.updates;
    assert(port_palette_cache_rgb888(&c,source,256) && c.updates==updates+1);
    assert(port_palette_cache_rgb888(&c,source,4) && c.count==4);
    updates=c.updates; source[255]^=0xffffffu;
    assert(port_palette_cache_rgb888(&c,source,4) && c.updates==updates);
    assert(port_palette_cache_rgb888(&c,source,256) && c.updates==updates+1);
    memset(&c,0,sizeof(c)); assert(port_palette_cache_rgb888(&c,source,256) && c.updates==1);
    SDL_FreeFormat(format);
    puts("palette cache PASS: 32768 differential SDL RGB888 colors, reuse, mutation, size changes, reinit, invalid input");
    return 0;
}
