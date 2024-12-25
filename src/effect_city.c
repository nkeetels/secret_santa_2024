#include "rendering.h"
#include "assets.h"
#include "sprites.h"
#include "game.h"
#include "meshgenerator.h"

int elmo_frame = 0;
model_t bbscene, character;

void effect_city_init()
{
    set_palette(bbscene__bake256Pal);
    
    load_model(&bbscene, bbscene_vertices, 160, SHADING_TEXTURED, 1, 4);
    load_model(&character, cookiedance_vertices, 257, SHADING_TEXTURED, 1, 4);

    character.position.z = -130;
    character.position.x = 200;
    character.euler_angles.y = 90;
}

void effect_city_destroy()
{
    clear_scene();
    clear_buffers(0x0000);
}

void IWRAM_CODE effect_city_update(uint16_t *target, uint32_t frame, uint16_t sync)
{;
    fast_set(target, 0x0000, 120 * 160 >> 2); 

    int16_t *mesh = &cookiedance_vertices[257 * 6 * elmo_frame * 3];
    load_model(&character, mesh, 257, SHADING_TEXTURED | MODEL_REPLACE, 1, 4);
    elmo_frame += 5;
    elmo_frame %= 105;  

    int rotation = 90;
    camera.position.z = ((350 * cos(rotation)) >> 8) + (sin(frame << 1) >> 2) - (frame << 2) ;//+ 23;
    camera.position.x = ((450 * sin(rotation)) >> 8) + (frame << 1);
    camera.position.y = 230 + (frame >> 2);

    camera.angles.y = 160 + (sin(20 + frame << 2) >> 4);
    camera.angles.z = cos(20 + frame << 2) >> 5;

    model_t* models[] = {&bbscene, &character };
    render(models, 2);

    fix_borders(target); 
}
