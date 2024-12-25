#include "rendering.h"
#include "assets.h"
#include "sprites.h"
#include "game.h"
#include "meshgenerator.h"


model_t shopping_model;

void effect_shopping_init()
{
    set_palette(shoppingbakePal);
    load_model(&shopping_model, shopping_vertices, 121, SHADING_TEXTURED, 1, 0);
    //load_model(&shopping_model, shopping_vertices, 137, SHADING_TEXTURED, 1, 0);
 }

void effect_shopping_destroy()
{
    clear_scene();
}

void effect_shopping_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    clear_screen(0x0000);

    int rotation = frame << 1;
    camera.position.z = ((350 * cos(rotation)) >> 8);
    camera.position.x = ((350 * sin(rotation)) >> 8) ;
    camera.position.y = 550;// + (frame >> 2);

    camera.angles.y = -rotation;// 160 + (sin(20 + frame << 2) >> 4);
    camera.angles.z = 0;// rotation << 2;//cos(20 + frame << 2) >> 5;

    render(&(model_t*){&shopping_model}, 1);

 //   fix_borders(target); 
}
