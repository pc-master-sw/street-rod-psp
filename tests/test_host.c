#include "port_platform.h"
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
int main(void) {
    PortConfig cfg={"host API",320,200,true,".",true};
    assert(port_platform_init(&cfg)); assert(!port_platform_init(&cfg));
    PortInput i; assert(port_platform_poll_input(&i));
    SDL_Event e={0}; e.type=SDL_KEYDOWN; e.key.keysym.sym=SDLK_z; assert(SDL_PushEvent(&e)==1);
    assert(port_platform_poll_input(&i) && i.pressed==PORT_FIRE);
    assert(port_platform_poll_input(&i) && i.pressed==0 && i.held==PORT_FIRE);
    e.type=SDL_KEYUP; assert(SDL_PushEvent(&e)==1); assert(port_platform_poll_input(&i) && i.released==PORT_FIRE);
    const SDL_Keycode auxiliary[]={SDLK_c,SDLK_v,SDLK_a,SDLK_s};
    for(unsigned k=0;k<4;++k) {
        e.type=SDL_KEYDOWN; e.key.keysym.sym=auxiliary[k]; assert(SDL_PushEvent(&e)==1);
    }
    const uint32_t extras=PORT_ACTION3|PORT_ACTION4|PORT_LEFT_TRIGGER|PORT_RIGHT_TRIGGER;
    assert(port_platform_poll_input(&i) && i.pressed==extras && i.held==extras);
    e.type=SDL_WINDOWEVENT; e.window.event=SDL_WINDOWEVENT_FOCUS_LOST; assert(SDL_PushEvent(&e)==1);
    assert(port_platform_poll_input(&i) && i.released==extras && i.held==0);
    uint8_t pixels[320*200]; uint32_t pal[256];
    for(int j=0;j<320*200;++j) pixels[j]=1;
    for(int j=0;j<256;++j) pal[j]=0xff0000ff;
    assert(port_platform_present_indexed(pixels,320,200,320,pal,256));
    uint32_t rgb; assert(port_platform_read_pixel(240,136,&rgb) && rgb==0xff0000);
    assert(port_platform_read_pixel(0,136,&rgb) && rgb==0);
    uint64_t t=port_platform_ticks_us(); port_platform_sleep_us(20000); assert(port_platform_ticks_us()>t);
    int16_t samples[2048]={0}; assert(port_platform_audio_push_s16(samples,1024)==1024);
    port_platform_sleep_us(60000); PortStats stats; port_platform_get_stats(&stats);
    assert(stats.audio_played==1024 && stats.presented==1);
    port_platform_shutdown(); port_platform_shutdown(); assert(port_platform_init(&cfg)); port_platform_shutdown();
    puts("host API PASS: actual events, readback, clock, audio callback, reinit"); return 0;
}
