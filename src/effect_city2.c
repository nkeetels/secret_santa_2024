#include "rendering.h"
#include "assets.h"
#include "sprites.h"
#include "game.h"
#include "meshgenerator.h"

int elmo_frame2 = 0;
model_t bbscene2, character2;

int cam_distances[] = {500, 330, 500, 300, 300, 200, 270, 300}; 
int new_dist = 0;

void effect_city2_init()
{
    set_palette(bbscene__bake256Pal);
    
    load_model(&bbscene2, bbscene_vertices, 160, SHADING_TEXTURED, 1, 4);
    load_model(&character2, cookiedance_vertices, 257, SHADING_TEXTURED, 1, 4);

    character2.position.z = -130;
    character2.position.x = 200;
    character2.euler_angles.y = 90;

     exposure_palette((uint16_t*)0x5000000, bbscene__bake256Pal, 200);
}

void effect_city2_destroy()
{
    clear_scene();
    clear_buffers(0x0000);
}

void IWRAM_CODE effect_city2_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
   // exposure_palette((uint16_t*)0x5000000, bbscene__bake256Pal, 0 + frame * 3);


    clear_screen(0x0000);

    int16_t *mesh = &cookiedance_vertices[257 * 6 * elmo_frame2 * 3];
    load_model(&character2, mesh, 257, SHADING_TEXTURED | MODEL_REPLACE, 1, 4);
    elmo_frame2 += 5;
    elmo_frame2 %= 105;  

    int dist = cam_distances[new_dist];

    if (((frame+1) & 7) == 0) {
        new_dist++;
        new_dist &= 7;
        dist = cam_distances[new_dist];
   invert_palette((uint16_t*)0x5000000, bbscene__bake256Pal, dist & 255);
    }
        

    int rotation = 90;
    camera.position.z = ((dist * cos(rotation)) >> 8) - 50 + (sin(frame << 1) >> 2) - (frame << 2) ;//+ 23;
    camera.position.x = ((dist * sin(rotation)) >> 8) - 50 + (frame << 1);
    camera.position.y = 230 + (frame >> 2);

    camera.angles.y = 160 + (sin(dist << 2) >> 4);
    camera.angles.z = dist;// cos(dist << 2) >> 5;

    model_t* models[] = {&bbscene2, &character2 };
    render(models, 2);

    fix_borders(target); 
}
