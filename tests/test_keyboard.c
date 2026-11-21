#include "port_keyboard.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void press(PortKeyboard *s,uint32_t buttons) { PortInput i={0}; i.pressed=buttons; port_keyboard_update(s,&i); }
int main(int argc,char **argv) {
    PortKeyboard s;
    assert(!port_keyboard_init(NULL,"",14)); assert(!port_keyboard_init(&s,"abc",2));
    assert(!port_keyboard_init(&s,"",32)); assert(!port_keyboard_init(&s,"",0));
    assert(port_keyboard_init(&s,"",14));
    press(&s,PORT_LEFT); assert(s.selected==5); press(&s,PORT_UP); assert(s.selected==41);
    press(&s,PORT_RIGHT); assert(s.selected==36); press(&s,PORT_DOWN); assert(s.selected==0);
    for(unsigned i=0;i<20;++i) press(&s,PORT_FIRE);
    assert(s.length==14 && !strcmp(s.text,"AAAAAAAAAAAAAA"));
    press(&s,PORT_ACTION3); assert(s.length==13 && !strcmp(s.text,"AAAAAAAAAAAAA"));
    press(&s,PORT_START); assert(s.result==PORT_KEYBOARD_ACCEPTED);
    press(&s,PORT_ALT); assert(s.result==PORT_KEYBOARD_ACCEPTED);
    assert(port_keyboard_init(&s,"KEEP",14)); press(&s,PORT_ALT);
    assert(s.result==PORT_KEYBOARD_CANCELLED && !strcmp(s.text,"KEEP"));
    assert(port_keyboard_init(&s,"\xff",14)); press(&s,PORT_START);
    assert(s.result==PORT_KEYBOARD_EDITING && s.invalid_ascii);
    press(&s,PORT_ACTION3); press(&s,PORT_START); assert(s.result==PORT_KEYBOARD_ACCEPTED && s.length==0);
    static uint8_t guarded[200*328+2]; guarded[0]=0x55; guarded[sizeof(guarded)-1]=0xaa;
    uint8_t *frame=guarded+1;
    for(unsigned cell=0;cell<42;++cell) {
        memset(frame,0x7f,200*328); assert(port_keyboard_init(&s,"NATIVE2",14)); s.selected=cell;
        assert(port_keyboard_draw(&s,frame,328));
        for(unsigned y=0;y<200;++y) { for(unsigned x=0;x<320;++x) assert(frame[y*328+x]<4);
            for(unsigned x=320;x<328;++x) assert(frame[y*328+x]==0x7f); }
    }
    assert(guarded[0]==0x55 && guarded[sizeof(guarded)-1]==0xaa);
    memset(frame,0x7f,200*328);
    assert(port_keyboard_draw_text(frame,328,314,193,"A",9));
    assert(frame[193*328+315]==9 && frame[193*328+314]==0x7f);
    assert(frame[192*328+315]==0x7f && frame[199*328+319]==0x7f);
    assert(!port_keyboard_draw_text(frame,328,-1,0,"A",9));
    assert(!port_keyboard_draw_text(frame,328,315,0,"A",9));
    assert(!port_keyboard_draw_text(frame,328,0,194,"A",9));
    assert(!port_keyboard_draw_text(frame,319,0,0,"A",9));
    assert(!port_keyboard_draw_text(frame,328,0,0,NULL,9));
    assert(guarded[0]==0x55 && guarded[sizeof(guarded)-1]==0xaa);
    assert(!port_keyboard_draw(&s,frame,319));
    assert(!port_keyboard_draw_title(&s,frame,328,NULL));
    assert(port_keyboard_init(&s,"ABCDEFGHIJKLMNOPQRSTUVWX1234",28));
    press(&s,PORT_FIRE); assert(s.length==28); press(&s,PORT_START);
    assert(s.result==PORT_KEYBOARD_ACCEPTED);
    assert(port_keyboard_draw_title(&s,frame,328,"HIGH SCORE"));
    for(unsigned y=0;y<200;++y) for(unsigned x=320;x<328;++x) assert(frame[y*328+x]==0x7f);
    assert(guarded[0]==0x55 && guarded[sizeof(guarded)-1]==0xaa);
    s.selected=99; assert(!port_keyboard_draw(&s,frame,328));
    assert(port_keyboard_init(&s,"",14)); uint32_t random=19;
    for(unsigned i=0;i<10000;++i) {
        random=random*1664525u+1013904223u; press(&s,1u<<(random%7));
        assert(s.selected<42 && s.length<=14 && s.text[s.length]==0);
        if(s.result!=PORT_KEYBOARD_EDITING) assert(port_keyboard_init(&s,"",14));
    }
    if(argc==2) {
        assert(port_keyboard_init(&s,"NATIVE EDIT 12",14)); s.selected=40;
        assert(port_keyboard_draw(&s,frame,328));
        FILE *preview=fopen(argv[1],"wb"); if(!preview) return 1;
        fprintf(preview,"P6\n320 200\n255\n"); const uint32_t *colors=port_keyboard_palette();
        for(unsigned y=0;y<200;++y) for(unsigned x=0;x<320;++x) {
            uint32_t rgba=colors[frame[y*328+x]];
            fputc((rgba>>24)&255,preview); fputc((rgba>>16)&255,preview); fputc((rgba>>8)&255,preview);
        }
        if(fclose(preview)) return 1;
    }
    puts("keyboard PASS: navigation, bounds, cancel, unsupported ASCII, renderer guards, 10000 transitions");
    return 0;
}
