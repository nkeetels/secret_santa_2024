#include "rendering.h"
#include "assets.h"
#include "sprites.h"
#include "game.h"
#if 0
extern uint16_t *framebuffer;

model_t elmo, street;

uint16_t animation_frame = 24, animation_frame2 = 0;

#define ANIMATION_FRAMES 105
#define ANIMATION_VERTICES 784
#define ANIMATION_TRIANGLES 261

#define NUM_STARS 100
vertex_t EWRAM_DATA stars[NUM_STARS];
vertex_t stars_copy[NUM_STARS];

void init_game()
{
    set_palette(bake256Pal);

//   load_model(&elmo, sesamstraat_mesh, 68, SHADING_TEXTURED, 1, 1, 3);
 
   load_model(&elmo, lamp_vertices, 798/3, SHADING_TEXTURED, 1, 2, 3); // deze
    //load_model(&elmo, tunnel_vertices, 256, SHADING_TEXTURED, 1, 2, 3);
    int16_t *mesh1 = &koekiemonster_dance[ANIMATION_TRIANGLES * 6 * animation_frame * 3];

  // load_model(&elmo, mesh1, koekiemonster_animation_vertex_count[0] / 3, SHADING_TEXTURED, 1, 0, 3);

    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i].position.x = (rand() % 500) - 250;
        stars[i].position.y = (rand() & 1023);
        stars[i].position.z = (rand() % 500) - 250;

        //stars[i].position.x <<= 3;
        //stars[i].position.y <<= 3;
        //stars[i].position.z <<= 3;

        //stars[i].position.x >>= 4;
        //stars[i].position.y >>= 4
        //stars[i].position.z >>= 4;

        int dist = (stars[i].position.x * stars[i].position.x + (stars[i].position.y - 300) * (stars[i].position.y - 300) + stars[i].position.z * stars[i].position.z) >> 10;
        if (dist < 150) stars[i].coords.z = (33 << 8) | 33;
        else if (dist < 300) stars[i].coords.z = (254 << 8) | 254;
        else stars[i].coords.z = (243 << 8) | 243;
    }

    set_sprites_transparency(10, 16);
    set_overlay(spr_globe_1Tiles, spr_globe_2Tiles, spr_globe_3Tiles, spr_globe_4Tiles, spr_globe_Pal);


    /*
extern const unsigned short spr_globe_Pal[];
extern const unsigned short spr_globe_1Bitmap[];
extern const unsigned short spr_globe_2Bitmap[];
extern const unsigned short spr_globe_3Bitmap[];
extern const unsigned short spr_globe_4Bitmap[];    
    */
}

void IWRAM_CODE update_game(uint16_t frame)
{   
    if (frame < 128) {
        int fader = min(frame << 1, 255);
        fader = fader > 250 ? 255 : fader;
        fade_palette((unsigned short*)0x5000000, bake256Pal, 0, 0, 0, 255 - fader);
        fade_palette((unsigned short*)0x5000200, spr_globe_Pal, 0, 0, 0, 255 - fader);
    }

    clear_screen(0x00);
 
    int time = frame << 2;
    int t = ((-frame & 0xFF) << 8) | ((frame) & 0xFF) & 0xFFFF;
   // move_table(framebuffer, huuu_koekieBitmap, lut_flower_fb, 40, 40, 256, t);      

    int16_t *mesh1 = &koekiemonster_dance[ANIMATION_TRIANGLES * 6 * animation_frame * 3];
   // load_model(&elmo, mesh1, koekiemonster_animation_vertex_count[animation_frame] / 3, SHADING_TEXTURED | MODEL_REPLACE, 1, 0, 3);
    animation_frame += 2;
    animation_frame %= ANIMATION_FRAMES;     


    camera.position.z = ((800 * cos(frame >> 2)) >> 8);
    camera.position.x = (800 * sin(frame >> 2)) >> 8;
    camera.position.y = 300;

    camera.angles.y = frame >> 2;

    model_t* models[] = {&elmo, &street};
    render(models, 1);

    fast_copy(stars_copy, stars, sizeof(vertex_t) * NUM_STARS >> 1);

    for (int i = 0; i < NUM_STARS; ++i) {
        transform(&elmo, &stars_copy[i]);
        project(&stars_copy[i]);

        put_pixel(stars_copy[i].position.x, stars_copy[i].position.y, stars_copy[i].coords.z);

        stars[i].position.y -= 16;
        if (stars[i].position.y < 0) {
            stars[i].position.y = 650;
        }
    }
}
#endif