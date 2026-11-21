#include "port_common.h"
#include <SDL.h>
#include <stdlib.h>
#include <string.h>
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_AudioDeviceID audio;
static PortAudioRing ring;
static PortStats stats;
static PortConfig cfg;
static char asset_root[768];
static uint32_t previous, held, *rgba;
static bool initialized, quit;
static PortViewport viewport;
static void audio_callback(void *ctx,Uint8 *stream,int bytes) {
    (void)ctx; size_t frames=(size_t)bytes/4;
    size_t got=port_ring_pop(&ring,(int16_t*)stream,frames);
    stats.audio_played+=got; if(got<frames) ++stats.audio_underruns;
}
void port_platform_shutdown(void) {
    if(audio) SDL_CloseAudioDevice(audio);
    audio=0; if(texture) SDL_DestroyTexture(texture); texture=NULL;
    if(renderer) SDL_DestroyRenderer(renderer); renderer=NULL;
    if(window) SDL_DestroyWindow(window); window=NULL;
    free(rgba); rgba=NULL; SDL_Quit(); initialized=false;
}
bool port_platform_init(const PortConfig *c) {
    if(initialized || !c || c->width<1 || c->height<1 || c->width>512 || c->height>512 ||
       !c->asset_root || strlen(c->asset_root)>=sizeof(asset_root)) return false;
    cfg=*c; strcpy(asset_root,c->asset_root); cfg.asset_root=asset_root;
    memset(&ring,0,sizeof(ring)); memset(&stats,0,sizeof(stats)); previous=held=0; quit=false;
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS|SDL_INIT_TIMER)<0) goto fail;
    window=SDL_CreateWindow(c->title,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,480,272,0);
    if(!window) goto fail;
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    if(!renderer) goto fail;
    texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_STREAMING,c->width,c->height);
    rgba=malloc((size_t)c->width*c->height*4);
    if(!texture || !rgba) goto fail;
    SDL_AudioSpec want={0}, have; want.freq=44100; want.format=AUDIO_S16SYS; want.channels=2;
    want.samples=1024; want.callback=audio_callback;
    if(c->enable_audio) { audio=SDL_OpenAudioDevice(NULL,0,&want,&have,0); if(!audio) goto fail; }
    viewport=port_viewport(480,272,c->width,c->height,c->aspect_4_3);
    initialized=true; if(audio) SDL_PauseAudioDevice(audio,0); return true;
fail: port_platform_shutdown(); return false;
}
static uint32_t key(SDL_Keycode k) {
    switch(k) { case SDLK_UP:return PORT_UP; case SDLK_DOWN:return PORT_DOWN;
      case SDLK_LEFT:return PORT_LEFT; case SDLK_RIGHT:return PORT_RIGHT;
      case SDLK_z:return PORT_FIRE; case SDLK_x:return PORT_ALT;
      case SDLK_RETURN:return PORT_START; case SDLK_BACKSPACE:return PORT_SELECT;
      case SDLK_c:return PORT_ACTION3; case SDLK_v:return PORT_ACTION4;
      case SDLK_a:return PORT_LEFT_TRIGGER; case SDLK_s:return PORT_RIGHT_TRIGGER; default:return 0; }
}
bool port_platform_poll_input(PortInput *o) {
    if(!initialized || !o) return false;
    SDL_Event e; while(SDL_PollEvent(&e)) {
        if(e.type==SDL_QUIT) quit=true;
        if(e.type==SDL_KEYDOWN) { held|=key(e.key.keysym.sym); if(e.key.keysym.sym==SDLK_ESCAPE) quit=true; }
        if(e.type==SDL_KEYUP) held&=~key(e.key.keysym.sym);
        if(e.type==SDL_WINDOWEVENT && e.window.event==SDL_WINDOWEVENT_FOCUS_LOST) held=0;
    }
    memset(o,0,sizeof(*o)); port_input_update(o,held,&previous); o->quit=quit; return true;
}
bool port_platform_present_indexed(const uint8_t *p,int w,int h,int pitch,const uint32_t *pal,int n) {
    if(!initialized || w!=cfg.width || h!=cfg.height || !port_frame_valid(p,w,h,pitch,pal,n)) return false;
    uint32_t colors[256]; for(int i=0;i<n;++i) colors[i]=port_rgba_to_abgr(pal[i]);
    for(int y=0;y<h;++y) for(int x=0;x<w;++x) rgba[(size_t)y*w+x]=colors[p[(size_t)y*pitch+x]];
    if(SDL_UpdateTexture(texture,NULL,rgba,w*4)<0) return false;
    SDL_SetRenderDrawColor(renderer,0,0,0,255); SDL_RenderClear(renderer);
    SDL_Rect dst={viewport.x,viewport.y,viewport.width,viewport.height};
    if(SDL_RenderCopy(renderer,texture,NULL,&dst)<0) return false;
    SDL_RenderPresent(renderer); ++stats.presented; return true;
}
size_t port_platform_audio_push_s16(const int16_t *s,size_t n) {
    if(!initialized || !audio) return 0;
    SDL_LockAudioDevice(audio); size_t got=port_ring_push(&ring,s,n);
    stats.audio_accepted+=got; SDL_UnlockAudioDevice(audio); return got;
}
void port_platform_get_stats(PortStats *o) {
    if(!o) return;
    if(audio) SDL_LockAudioDevice(audio);
    *o=stats; o->audio_queued=ring.count;
    if(audio) SDL_UnlockAudioDevice(audio);
}
uint64_t port_platform_ticks_us(void) {
    uint64_t t=SDL_GetPerformanceCounter(), f=SDL_GetPerformanceFrequency();
    return (t/f)*1000000+(t%f)*1000000/f;
}
void port_platform_sleep_us(uint32_t us) { SDL_Delay((us+999)/1000); }
bool port_platform_read_asset(const char *r,void *d,size_t c,size_t *n) { return initialized && port_asset_read(asset_root,r,d,c,n); }
bool port_platform_read_pixel(int x,int y,uint32_t *rgb) {
    if(!initialized || !rgb || x<0 || x>=480 || y<0 || y>=272) return false;
    uint32_t c; SDL_Rect r={x,y,1,1};
    if(SDL_RenderReadPixels(renderer,&r,SDL_PIXELFORMAT_ABGR8888,&c,4)<0) return false;
    *rgb=((c&255)<<16)|(c&0xff00)|((c>>16)&255); return true;
}
