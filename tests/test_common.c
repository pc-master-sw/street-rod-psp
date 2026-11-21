#include "port_common.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    uint32_t pal[256]={0}; uint8_t frame[12]={0};
    assert(port_frame_valid(frame,3,3,4,pal,256));
    assert(!port_frame_valid(frame,3,3,2,pal,256));
    assert(!port_frame_valid(frame,513,1,513,pal,256));
    assert(!port_frame_valid(NULL,3,3,4,pal,256));
    frame[5]=3; assert(!port_frame_valid(frame,3,3,4,pal,3));
    assert(port_rgba_to_abgr(0x12345678)==0x78563412);
    PortViewport v=port_viewport(480,272,320,200,true); assert(v.x==59 && v.y==0 && v.width==362 && v.height==272);
    v=port_viewport(480,272,320,200,false); assert(v.width==435 && v.height==272 && v.x==22);
    PortInput i; uint32_t prev=0; port_input_update(&i,PORT_FIRE|PORT_LEFT,&prev);
    assert(i.pressed==(PORT_FIRE|PORT_LEFT)); port_input_update(&i,PORT_FIRE,&prev);
    assert(i.released==PORT_LEFT && i.pressed==0); port_input_update(&i,0,&prev); assert(i.released==PORT_FIRE);
    uint32_t blocked=PORT_START|PORT_ALT;
    assert(port_input_after_modal(PORT_START|PORT_ALT,&blocked)==0);
    assert(port_input_after_modal(PORT_START|PORT_ALT|PORT_LEFT,&blocked)==PORT_LEFT);
    assert(port_input_after_modal(PORT_ALT|PORT_LEFT,&blocked)==PORT_LEFT);
    assert(port_input_after_modal(PORT_START|PORT_ALT,&blocked)==PORT_START);
    assert(port_input_after_modal(0,&blocked)==0 && blocked==0);
    assert(port_input_after_modal(PORT_ALT,&blocked)==PORT_ALT);
    static PortAudioRing r; static int16_t data[PORT_AUDIO_CAPACITY*2], out[PORT_AUDIO_CAPACITY*2];
    for(size_t j=0;j<PORT_AUDIO_CAPACITY*2;++j) data[j]=(int16_t)j;
    assert(port_ring_push(&r,data,PORT_AUDIO_CAPACITY)==PORT_AUDIO_CAPACITY);
    assert(port_ring_push(&r,data,1)==0); assert(port_ring_pop(&r,out,7000)==7000);
    assert(!memcmp(out,data,7000*4)); assert(port_ring_push(&r,data,7000)==7000);
    assert(port_ring_pop(&r,out,PORT_AUDIO_CAPACITY)==PORT_AUDIO_CAPACITY);
    assert(!memcmp(out,data+14000,1192*4)); assert(!memcmp(out+2384,data,7000*4));
    memset(out,1,sizeof(out)); assert(port_ring_pop(&r,out,1024)==0);
    for(int j=0;j<2048;++j) assert(out[j]==0);
    char path[32]; assert(port_asset_path(path,sizeof(path),"assets","level/data"));
    const char *bad[]={"../save","level/../save","/abs","ms0:/foo","a\\b","a//b","a/","./a"};
    for(size_t j=0;j<sizeof(bad)/sizeof(*bad);++j) assert(!port_asset_path(path,sizeof(path),"assets",bad[j]));
    assert(!port_asset_path(path,3,"assets","data"));
    puts("common PASS: framebuffer, palette, viewport, input, ring wrap/backpressure, paths"); return 0;
}
