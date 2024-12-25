#include "rendering.h"
#include "assets.h"
#include "sprites.h"
#include "game.h"

extern uint16_t *framebuffer;

model_t globe;
/*
uint16_t animation_frame = 24, animation_frame2 = 0;

#define ANIMATION_FRAMES 105
#define ANIMATION_VERTICES 784
#define ANIMATION_TRIANGLES 261

#define NUM_STARS 100
vertex_t EWRAM_DATA stars[NUM_STARS];
vertex_t stars_copy[NUM_STARS];
*/
void effect_globe_init()
{
    set_palette(bake256Pal);
    load_model(&globe, lamp_vertices, 798/3, SHADING_TEXTURED, 1, 2);
/*
    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i].position.x = (rand() % 500) - 250;
        stars[i].position.y = (rand() & 1023);
        stars[i].position.z = (rand() % 500) - 250;

     
        int dist = (stars[i].position.x * stars[i].position.x + (stars[i].position.y - 300) * (stars[i].position.y - 300) + stars[i].position.z * stars[i].position.z) >> 10;
        if (dist < 150) stars[i].coords.z = (33 << 8) | 33;
        else if (dist < 300) stars[i].coords.z = (254 << 8) | 254;
        else stars[i].coords.z = (243 << 8) | 243;
    }

*/
    set_overlay(mc_spr_1Tiles, mc_spr_2Tiles, mc_spr_3Tiles, mc_spr_4Tiles, mc_spr_1Pal);
    fade_palette((unsigned short*)0x5000200, mc_spr_1Pal, 0, 0, 0, 100);
  //  fast_set((uint16_t*)0x5000200, 0, 128);
}

void effect_globe_destroy()
{
    clear_scene();
    clear_buffers(0x0000);
}

void IWRAM_CODE effect_globe_update(uint16_t *target, uint32_t frame, uint16_t sync)
{   
    /*
    if (frame < 128) {
        int fader = min(frame << 2, 255);
        fader = fader > 250 ? 255 : fader;
        fade_palette((unsigned short*)0x5000000, bake256Pal, 0, 0, 0, 255 - fader);
        fade_palette((unsigned short*)0x5000200, spr_globe_Pal, 0, 0, 0, 255 - fader);
    }
*/
    clear_screen(0x00);
 
    int rotation = frame << 1;

    camera.position.z = 900;// ((800 * cos(rotation)) >> 8);
    camera.position.y = 300;
  //  camera.position.x = (800 * sin(rotation)) >> 8;
   // camera.position.y = 300;

  //  camera.angles.y = rotation;

  globe.euler_angles.y = -rotation;

    render(&(model_t*){&globe}, 1);
/*
    fast_copy(stars_copy, stars, sizeof(vertex_t) * NUM_STARS >> 1);

    for (int i = 0; i < NUM_STARS; ++i) {
        transform(&globe, &stars_copy[i]);
        project(&stars_copy[i]);

        put_pixel(stars_copy[i].position.x, stars_copy[i].position.y, stars_copy[i].coords.z);

        stars[i].position.y -= 16;
        if (stars[i].position.y < 0) {
            stars[i].position.y = 650;
        }
    }
*/
}
