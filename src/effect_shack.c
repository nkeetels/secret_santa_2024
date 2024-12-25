#include "rendering.h"
#include "assets.h"
#include "sprites.h"
#include "game.h"
#include "meshgenerator.h"


model_t shack_model, cookie_shack;
int cookiemove = 1;

void effect_shack_init()
{
    set_palette(shack256Pal);
    load_model(&shack_model, shack_vertices, 45, SHADING_TEXTURED, 1, 5);
    load_model(&cookie_shack, koekieshack_vertices, 332, SHADING_TEXTURED, 1, 5);

    exposure_palette((uint16_t*)0x5000000, shack256Pal, 196);
 }



void effect_shack_destroy()
{
    clear_scene();
    clear_buffers(0x0000);
}

void IWRAM_CODE effect_shack_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    clear_screen(0x0000);


    int16_t *mesh = &koekieshack_vertices[332 * 6 * cookiemove * 3];
    load_model(&cookie_shack, mesh, 332, SHADING_TEXTURED | MODEL_REPLACE, 1, 5);
    cookiemove += 5;
    cookiemove %= 198;  

    int rotation = 100 + (sin(frame << 1) >> 4);
    int dist = 200 - (frame << 1);
    camera.position.z = ((dist * cos(rotation)) >> 8);
    camera.position.x = ((dist * sin(rotation)) >> 8) ;
    camera.position.y = 110 + (frame << 2);// + (frame >> 2);

    camera.angles.y = -rotation;// 160 + (sin(20 + frame << 2) >> 4);
    camera.angles.z = 0;// rotation << 2;//cos(20 + frame << 2) >> 5;

   // render(&(model_t*){&shack_model}, 1);
    model_t* models[] = {&shack_model, &cookie_shack };
    render(models, 2);

 //   fix_borders(target); 
}
