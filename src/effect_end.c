#include "audio.h"
#include "effects.h"
#include "assets.h"
#include "rendering.h"
#include "meshgenerator.h"

void effect_end_init()
{
    clear_screen(0x00);

    fast_set((unsigned short*)0x5000000, 0, 128);

    set_overlay(mc_spr_1Tiles, mc_spr_2Tiles, mc_spr_3Tiles, mc_spr_4Tiles, mc_spr_1Pal);

    stop_audio();
}

void effect_end_destroy()
{
}

void effect_end_update(uint16_t *target, uint32_t frame, uint16_t sync)
{
    clear_screen(0x00);

    if (frame > 8) {
        while (1) {}
    }
}