#include "rendering.h"
#include "assets.h"
#include "sprites.h"
#include "game.h"
#include "meshgenerator.h"


model_t piano_model, elmo_piano;
int piano_frame = 0;

#define NUM_STARS 50
vertex_t EWRAM_DATA stars[NUM_STARS];
vertex_t stars_copy[NUM_STARS];

void effect_piano_init()
{
    set_palette(GBA_piano256Pal);
    load_model(&piano_model, piano_vertices, 116, SHADING_TEXTURED, 1, 1);
    load_model(&elmo_piano, elmo_piano_vertices, 264, SHADING_TEXTURED, 1, 1);

    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i].position.x = (rand() % 400) - 200;
        stars[i].position.y = (rand() & 511);
        stars[i].position.z = (rand() % 400) - 200;

        int dist = (stars[i].position.x * stars[i].position.x + (stars[i].position.y - 300) * (stars[i].position.y - 300) + stars[i].position.z * stars[i].position.z) >> 10;
        if (dist < 50) stars[i].coords.z = (72 << 8) | 72;
        else if (dist < 100) stars[i].coords.z = (40 << 8) | 40;
        else stars[i].coords.z = (24 << 8) | 24;
    }
 }

void effect_piano_destroy()
{
    clear_scene();
    clear_buffers(0x0000);
}

void IWRAM_CODE effect_piano_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    clear_screen(0x0000);

    if (frame < 128) {
        int fader = min(frame << 2, 255);
        fader = fader > 250 ? 255 : fader;
        fade_palette((unsigned short*)0x5000000, GBA_piano256Pal, 0, 0, 0, 255 - fader);
       // fade_palette((unsigned short*)0x5000200, spr_globe_Pal, 0, 0, 0, 255 - fader);
    }

    int16_t *mesh = &elmo_piano_vertices[264 * 6 * piano_frame * 3];
    load_model(&elmo_piano, mesh, 264, SHADING_TEXTURED | MODEL_REPLACE, 1, 4);
    piano_frame += 15;
    piano_frame %= 360;  

    int rotation = (frame << 1) + -16;
    camera.position.z = ((175 * cos(rotation)) >> 8);
    camera.position.x = ((175 * sin(rotation)) >> 8);
    camera.position.y = 180;// + (frame >> 2);

    camera.angles.y = -rotation;// 160 + (sin(20 + frame << 2) >> 4);
    camera.angles.z = 0;// rotation << 2;//cos(20 + frame << 2) >> 5;

    model_t* models[] = {&piano_model, &elmo_piano };
    render(models, 2);

    fast_copy(stars_copy, stars, sizeof(vertex_t) * NUM_STARS >> 1);

    for (int i = 0; i < NUM_STARS; ++i) {
        transform(&piano_model, &stars_copy[i]);
        project(&stars_copy[i]);

        if (stars_copy[i].position.x > 0 && stars_copy[i].position.x < 240 && stars_copy[i].position.y > 0 && stars_copy[i].position.y < 160)
          put_pixel(stars_copy[i].position.x, stars_copy[i].position.y, stars_copy[i].coords.z);

        stars[i].position.y -= 16;
        if (stars[i].position.y < 0) {
            stars[i].position.y = 400;
        }

    }

 //   fix_borders(target); 
}