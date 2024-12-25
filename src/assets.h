#ifndef __ASSETS_H
#define __ASSETS_H

/* music */

extern const uint8_t koekie_data[];

/* animations */

//extern const int16_t elmo_animation_vertices[];
extern const int16_t koekiemonster_dance[]; // 105 frames, 784 vertices per frame, 261 triangles per frame
extern const int16_t cookiedance_vertices[]; // 105 frames

extern const int16_t mario64_castle_vertices[];

extern const int16_t city_vertices[]; // 256 tris
extern const unsigned short city256Pal[];
extern const unsigned short city256Bitmap[];

extern const int16_t bbscene_vertices[]; // 160
extern const unsigned short bbscene__bake256Pal[];
extern const unsigned short bbscene__bake256Bitmap[];

extern const unsigned short shoppingbakePal[];
extern const unsigned short shoppingbakeBitmap[];
extern const int16_t shopping_vertices[]; // 121 tris


extern const int16_t piano_vertices[]; // 116 tris
extern const unsigned short GBA_piano256Pal[];
extern const unsigned short GBA_piano256Bitmap[];
extern const int16_t elmo_piano_vertices[];

extern const int16_t shack_vertices[]; // 51 tris
extern const unsigned short shack256Bitmap[];
extern const unsigned short shack256Pal[];
extern const int16_t koekieshack_vertices[];

/* environment */

extern const unsigned short bake256Pal[];
extern const unsigned short bake256Bitmap[];
extern const int16_t lamp_vertices[]; // 462 vertices

/* movetable LUTs */
extern const unsigned short lut_flower_fb[];


/* overlay */
extern const unsigned short spr_globe_Pal[];
extern const unsigned short spr_globe_1Tiles[];
extern const unsigned short spr_globe_2Tiles[];
extern const unsigned short spr_globe_3Tiles[];
extern const unsigned short spr_globe_4Tiles[];

extern const unsigned short mc_spr_1Pal[];
extern const unsigned short mc_spr_1Tiles[];
extern const unsigned short mc_spr_2Tiles[];
extern const unsigned short mc_spr_3Tiles[];
extern const unsigned short mc_spr_4Tiles[];
#endif /* __ASSETS_H */