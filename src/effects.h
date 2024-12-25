#ifndef __EFFECT_H
#define __EFFECT_H

#include <stdint.h>
#include "demo.h"

void move_table(uint16_t *target, uint8_t *texture, uint16_t *table, int16_t xoffset, int16_t yoffset, uint16_t texture_width, uint16_t t);
void move_table_sprites(uint16_t *target, uint8_t *texture, uint16_t *table, int16_t xoffset, int16_t yoffset, uint16_t t);
//void upscale_uv_table(uint16_t *target, uint8_t *texture, int *table, uint16_t t);

void bloom(uint16_t *target, const uint8_t bloom_bias, const uint8_t bloom_threshold);
void motion_blur(uint16_t *target);
void zoom_blur(uint16_t *target);

void fade_palette(uint16_t *dest, const uint16_t *palette, uint8_t red, uint8_t green, uint8_t blue, uint8_t v);
void exposure_palette(uint16_t *dest, const uint16_t *palette, uint8_t v);
void invert_palette(uint16_t *dest, const uint16_t *palette, uint8_t v);
void scale_palette(uint16_t *dest, const uint16_t *palette, uint8_t v);
void add_palette(uint16_t *dest, const uint16_t *palette, int16_t v);
void cycle_palette();
void rotate_background(uint16_t angle, uint16_t zoom);

extern int uvtable[16 * 8 * 3];
extern unsigned short generated_palette[256];

void effect_globe_init();
void effect_globe_destroy();
void effect_globe_update(uint16_t *target, uint32_t frame, uint16_t sync);

static const struct scene_t effect_globe = {
	effect_globe_init,
	effect_globe_update,
	effect_globe_destroy,
};

void effect_city_init();
void effect_city_destroy();
void effect_city_update(uint16_t *target, uint32_t frame, uint16_t sync);

static const struct scene_t effect_city = {
	effect_city_init,
	effect_city_update,
	effect_city_destroy,
};

void effect_city2_init();
void effect_city2_destroy();
void effect_city2_update(uint16_t *target, uint32_t frame, uint16_t sync);

static const struct scene_t effect_city2 = {
	effect_city2_init,
	effect_city2_update,
	effect_city2_destroy,
};

void effect_shopping_init();
void effect_shopping_destroy();
void effect_shopping_update(uint16_t *target, uint32_t frame, uint16_t sync);

static const struct scene_t effect_shopping = {
	effect_shopping_init,
	effect_shopping_update,
	effect_shopping_destroy,
};

void effect_piano_init();
void effect_piano_destroy();
void effect_piano_update(uint16_t *target, uint32_t frame, uint16_t sync);

static const struct scene_t effect_piano = {
	effect_piano_init,
	effect_piano_update,
	effect_piano_destroy,
};

void effect_shack_init();
void effect_shack_destroy();
void effect_shack_update(uint16_t *target, uint32_t frame, uint16_t sync);

static const struct scene_t effect_shack = {
	effect_shack_init,
	effect_shack_update,
	effect_shack_destroy,
};

void effect_end_init();
void effect_end_destroy();
void effect_end_update(uint16_t *target, uint32_t frame, uint16_t sync);

static const struct scene_t effect_end = {
	effect_end_init,
	effect_end_update,
	effect_end_destroy,
};

#endif /* __EFFECT_H */