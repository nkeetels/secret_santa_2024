#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include "audio.h"
#include "assets.h"
//#include "script.h"
#include "demo.h"
#include "rendering.h"
#include "vcount.h"
#include "game.h"


#define FB (uint16_t*)0x6000000
#define BB (uint16_t*)0x600A000

uint16_t *framebuffer = BB;
uint16_t *backbuffer = FB;

uint32_t current_frame = 0;

void vblank()
{
  update_audio();
  ++current_frame;
}

void vcount()
{
}

void present()
{
  if (REG_DISPCNT & BACKBUFFER) {
    REG_DISPCNT &= ~BACKBUFFER;
    framebuffer = BB;
    backbuffer = FB;
  }
  else {
    REG_DISPCNT |= BACKBUFFER;
    framebuffer = FB;
    backbuffer = BB;
  } 
}

void run_demo()
{
  irqInit();
  irqEnable(IRQ_VBLANK | IRQ_HBLANK);
  irqSet(IRQ_VBLANK, vblank);
  irqSet(IRQ_HBLANK, vcount);

  SetMode(MODE_4 | BG2_ON | OBJ_ENABLE | OBJ_1D_MAP | BG_MOSAIC );

  play_audio((uint8_t*)koekie_data, (1269678 >> 4) - 1024 * 16, 124);

  //init_game();
  start_script();

  while (1) {
    VBlankIntrWait();

    update_camera(&camera);
    
    //update_game(current_frame);
    
    tick_script(current_frame);

    present();    
  }
}
