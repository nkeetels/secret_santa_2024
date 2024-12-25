#ifndef __RENDERING_H
#define __RENDERING_H

#include "fixedmath.h"

#define VIEWPORT_WIDTH 240
#define VIEWPORT_HEIGHT 160
#define VIEWPORT_TOP 0
#define VIEWPORT_BOTTOM VIEWPORT_HEIGHT
#define VIEWPORT_LEFT 0
#define VIEWPORT_RIGHT VIEWPORT_WIDTH


#define MODEL_ENABLED 1 << 1
#define MODEL_DISABLED 1 << 2
#define MODEL_REPLACE 1 << 3

#define SHADING_TEXTURED 1 << 5

#define MAX_MODELS 5
#define MAX_FACES 1000
#define MAX_VERTICES 3 * MAX_FACES

#define MAX_BINS 1024 / 4

typedef struct vertex_t {
  vector3_t position;
  vector3_t coords;
} vertex_t;

typedef struct face_t {
  uint16_t vertex_start;
  uint16_t vertex_count;
  vector3_t normal;
  //uint16_t depth;
  uint8_t texture_index;
  struct face_t *next;
} face_t;

typedef struct model_t {
  uint16_t vertex_start;
  uint16_t vertex_count;
  uint16_t face_start;
  uint16_t face_count;
  struct face_t *faces;
  vertex_t *vertex_source;
  vector3_t position;
  vector3_t euler_angles;
  int8_t flags;
} model_t;

typedef struct camera_t {
  vector3_t position;
  vector3_t angles;
  uint8_t speed;
} camera_t;

typedef struct {
    face_t* first;
} bin_t;

typedef void (*shader_t)(vertex_t *vertices, int num_vertices, uint16_t *texture);

extern camera_t camera;

void clear_screen(uint8_t color);
void set_palette(const uint16_t *palette);
void set_camera_position(int16_t x, int16_t y, int16_t z);
void set_euler_angles(int16_t x, int16_t y, int16_t z);
void init_camera();
void put_pixel(uint8_t x, uint8_t y, uint8_t color);
void line(int x0, int y0, int x1, int y1, uint8_t color);
void render(model_t** models, int model_count);
void project(vertex_t *v);
void transform(model_t *model, vertex_t *v);
void fix_borders(uint16_t* target);
void set_slime_vectors(int16_t x, int16_t y, int16_t z);
int sutherland_hodgman_clipping(vertex_t *vertices, uint16_t num_vertices);

void texture_polygon(vertex_t *vtx, int vertices, uint16_t *texture);
void flat_polygon(vertex_t *vtx, int vertices, uint16_t *texture);
void wire_polygon(vertex_t *vertices, int num_vertices, uint16_t *texture);

extern face_t face_list[MAX_FACES]; 
extern vertex_t raw_vertex_list[MAX_VERTICES];
extern vertex_t vertex_buffer[MAX_VERTICES];
extern model_t current_model;
extern uint16_t face_count;

#endif /* __RENDERING_H */