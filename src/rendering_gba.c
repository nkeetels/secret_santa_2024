#include <gba_video.h>
#include "rendering.h"
#include "assets.h"

const uint8_t *texture_list[] = { shoppingbakeBitmap, GBA_piano256Bitmap, bake256Bitmap, city256Bitmap, bbscene__bake256Bitmap, shack256Bitmap };

extern uint16_t *framebuffer;
extern uint16_t *backbuffer;
extern uint32_t effect_frame;

camera_t camera;

EWRAM_DATA vertex_t raw_vertex_list[MAX_VERTICES];
EWRAM_DATA vertex_t vertex_buffer[MAX_VERTICES];
EWRAM_DATA face_t face_list[MAX_FACES];
EWRAM_DATA face_t transformed_list[MAX_FACES]; 

EWRAM_DATA model_t models[MAX_MODELS];

//int draw_order[MAX_FACES];  // legacy: for quicksort

IWRAM_DATA bin_t depth_bins[MAX_BINS];

IWRAM_DATA vertex_t scratch_vertices[4];
IWRAM_DATA vertex_t clip_buffer1[30];
IWRAM_DATA vertex_t clip_buffer2[30];

matrix3x3_t model_matrix;
matrix3x3_t camera_matrix;

void set_palette(const uint16_t *palette)
{
  fast_copy(((unsigned short*)0x5000000), palette, 256);
}

void IWRAM_CODE clear_screen(uint8_t color)
{
  fast_set(framebuffer, (color << 8)|(color), (VIEWPORT_WIDTH * VIEWPORT_HEIGHT) >> 1); 
}

void clear_buffers(uint8_t color)
{
  fast_set(framebuffer, (color << 8)|(color), (VIEWPORT_WIDTH * VIEWPORT_HEIGHT) >> 1); 
  fast_set(backbuffer, (color << 8)|(color), (VIEWPORT_WIDTH * VIEWPORT_HEIGHT) >> 1); 
  fast_set((uint16_t*)0x5000200, 0x0000, 128);
  fast_set((uint16_t*)0x5000000, 0x0000, 128);

  clear_sprites();
  commit_sprites();
}

void set_camera_position(int16_t x, int16_t y, int16_t z)
{
  camera.position.x = x;
  camera.position.y = y;
  camera.position.z = z;
}

void set_euler_angles(int16_t x, int16_t y, int16_t z)
{
  camera.angles.x = x;
  camera.angles.y = y;
  camera.angles.z = z; 
}

void init_camera()
{
  camera.position.x = 0;
  camera.position.y = 0;
  camera.position.z = 1034;

  camera.angles.x = 0;
  camera.angles.y = 0;
  camera.angles.z = 0;
}

void init_renderer()
{
  set_camera_position(0, 0, 0);
  set_euler_angles(0, 0, 0);

}

void IWRAM_CODE put_pixel(uint8_t x, uint8_t y, uint8_t color)
{
  if ((x < 0) | (x > 240) | (y < 0) | (y > 160))
    return;

  uint16_t *target = (uint16_t*)framebuffer + ((y * 240 + x) >> 1);

  if (x & 1) {
    *target = (*target & 0xFF) | (color << 8);
  }
  else {
    *target = (*target & ~0xFF) | color;
  }
}

void IWRAM_CODE line(int x0, int y0, int x1, int y1, uint8_t color)
{
  if (y0 > y1) 
  {
    swap(x0, x1);
    swap(y0, y1);
  }

  int dx = x1 - x0;
  int dy = y1 - y0;

  int gradient = dx < 0 ? -1 : 1;
  dx *= gradient;

  put_pixel(x0, y0, color);

  if (dy == 0) 
  {
    while (dx--) put_pixel(x0 += gradient, y0, color);
    return;
  }

  if (dx == 0) 
  { 
    while (dy--) put_pixel(x0, y0++, color);
    return;
  }

  if (dx == dy) 
  {
    while (dy--) put_pixel(x0 += gradient, y0++, color);
    return;
  }
  
  uint16_t currentError;
  uint16_t error = 0;
  uint16_t intensity = 0;
   
  if (dy > dx) 
  {
    uint16_t fract = div(dx << 16, dy);

    while (--dy) 
    {
      currentError = error;   
      x0 += ((error += fract) <= currentError) ? gradient : 0;

      intensity = error >> 8;

      put_pixel(x0, ++y0, color/* + intensity*/);
      put_pixel(x0 + gradient, y0, color/* + (intensity ^ 255)*/);
    }
  }
  else
  {
    uint16_t fract = div(dy << 16, dx);

    while (--dx) 
    {
      currentError = error; 
      y0 += ((error += fract) <= currentError) ? 1 : 0;

      intensity = error >> 8;

      put_pixel(x0 += gradient, y0, color/* + intensity*/);
      put_pixel(x0, y0 + 1, color/* + (intensity ^ 255)*/);
    }
  }
  put_pixel(x1, y1, color);
}
/*
void IWRAM_CODE transform(model_t *model, vertex_t *v)
{  
    int x1, y1, z1, x2, y2, z2, x3, y3, z3;

    int x = v->position.x - camera.position.x;
    int y = v->position.y - camera.position.y;
    int z = v->position.z - camera.position.z;
    
    x1 = x;
    y1 = (y * cos(camera.angles.x) >> 8) - (z * sin(camera.angles.x) >> 8);
    z1 = (y * sin(camera.angles.x) >> 8) + (z * cos(camera.angles.x) >> 8);
    
    x2 = (x1 * cos(camera.angles.y) >> 8) - (z1 * sin(camera.angles.y) >> 8);
    y2 = y1;
    z2 = (x1 * sin(camera.angles.y) >> 8) + (z1 * cos(camera.angles.y) >> 8);
    
    x3 = (x2 * cos(camera.angles.z) >> 8) - (y2 * sin(camera.angles.z) >> 8);
    y3 = (x2 * sin(camera.angles.z) >> 8) + (y2 * cos(camera.angles.z) >> 8);
    z3 = z2;
    
    v->position.x = x3 + model->position.x;
    v->position.y = y3 + model->position.y;
    v->position.z = z3 + model->position.z;
}
*/

void IWRAM_CODE fix_borders(uint16_t* target) 
{
  int i;
  for (i = 0; i < 120; ++i) {
    target[i] = 0x000;
    target[120 * 157 + i] = 0x000;
    target[120 * 156 + i] = 0x000;
  }

  for (uint16_t *start = target; start < target + 120 * 159; start += 120) {
    start[0] = 0x000;
    start[1] = 0x000;
    start[117] = 0x000;
    start[118] = 0x000;
  }
}

IWRAM_CODE void transform(model_t *model, vertex_t *v)
{
  vector3_t temp;

  temp.x = (model_matrix[0][0] * v->position.x + model_matrix[0][1] * v->position.y + model_matrix[0][2] * v->position.z) >> 8;
  temp.y = (model_matrix[1][0] * v->position.x + model_matrix[1][1] * v->position.y + model_matrix[1][2] * v->position.z) >> 8;
  temp.z = (model_matrix[2][0] * v->position.x + model_matrix[2][1] * v->position.y + model_matrix[2][2] * v->position.z) >> 8;

  temp.x += model->position.x - camera.position.x;
  temp.y += model->position.y - camera.position.y;
  temp.z += model->position.z - camera.position.z;

  v->position.x = (camera_matrix[0][0] * temp.x + camera_matrix[0][1] * temp.y + camera_matrix[0][2] * temp.z) >> 8;
  v->position.y = (camera_matrix[1][0] * temp.x + camera_matrix[1][1] * temp.y + camera_matrix[1][2] * temp.z) >> 8;
  v->position.z = (camera_matrix[2][0] * temp.x + camera_matrix[2][1] * temp.y + camera_matrix[2][2] * temp.z) >> 8;
}

void IWRAM_CODE project(vertex_t *v)
{ 
  int z = rcp((v->position.z >> 1));

  v->position.x = (58 * v->position.x * z >> 16) + (VIEWPORT_WIDTH >> 1);
  v->position.y = (39 * v->position.y * z >> 16) + (VIEWPORT_HEIGHT >> 1);
}

bool IWRAM_CODE is_clockwise(vertex_t *vertices) 
{
  int dx1 = vertices[1].position.x - vertices[0].position.x;
  int dy1 = vertices[1].position.y - vertices[0].position.y;
  int dx2 = vertices[2].position.x - vertices[0].position.x;
  int dy2 = vertices[2].position.y - vertices[0].position.y;
  
  return (dx1 * dy2 - dy1 * dx2) < 0;
}
/*
void IWRAM_CODE swap_indices(uint16_t *a, uint16_t *b, uint16_t l, uint16_t r)
{
  int t = draw_order[l];
  draw_order[l] = draw_order[r];
  draw_order[r] = t;
}

void IWRAM_CODE quick_sort(uint16_t *list, uint16_t start, uint16_t end)
{
  int z, l, r;

  if (end > start + 1)
  {
    z = transformed_list[list[start]].depth;
    l = start + 1;
    r = end;

    while (l < r)
    {
      if (transformed_list[list[l]].depth < z)
      { 
        l++;
      }
      else
      {
        r--;
        swap_indices(&list[l], &list[r], l, r);
      }
    }
    l--;
    swap_indices(&list[l], &list[start], l, start);
    quick_sort(list, start, l);
    quick_sort(list, r, end);
  }
} 
*/

void IWRAM_CODE texture_polygon(vertex_t *vtx, int vertices, uint16_t *texture)
{
  int c = 0;
  int i, top_index, bottom_index, left_index, right_index;
  int min_y = 99999, max_y = 0;
  for (i = 0; i < vertices; i++) {
    if ((vtx[i].position.y) < min_y) {min_y = (vtx[i].position.y); top_index = i;}
    if ((vtx[i].position.y) > max_y) {max_y = (vtx[i].position.y); bottom_index = i;}
  }

  left_index = right_index = top_index;
  int y = min_y * 120;
  int length = max_y - min_y;
  int right_x, right_dx, left_x, left_dx, left_u, left_du, left_v, left_dv, right_u, right_v, right_du, right_dv;
  int n, stride, prev_x, prev_u, prev_v, tu, tv, len, du, dv;
  
  int left_height = 0, right_height = 0;
  uint16_t *start, *line = (uint16_t*)(framebuffer + y);
  uint8_t *bitmap = (uint8_t*)texture;
  do {
    if (--left_height <= 0) {
      if (left_index == bottom_index) {
        return;
      }   

      prev_x = vtx[left_index].position.x;
      prev_u = vtx[left_index].coords.x;
      prev_v = vtx[left_index].coords.y;
      left_height = vtx[left_index].position.y;
      
      if (--left_index < 0) {
        left_index = vertices - 1;
      }
      
      left_height = (vtx[left_index].position.y + 1) - (left_height);
      
      left_dx = divide((vtx[left_index].position.x - prev_x) << 16, left_height);
      left_du = divide((vtx[left_index].coords.x - prev_u) << 16, left_height);
      left_dv = divide((vtx[left_index].coords.y - prev_v) << 16, left_height);

      left_x = prev_x << 16;
      left_u = prev_u << 16;
      left_v = prev_v << 16;
    }
      
    if (--right_height <= 0) {
      if (right_index == bottom_index) {
        return;
      }

      prev_x = vtx[right_index].position.x;
      prev_u = vtx[right_index].coords.x;
      prev_v = vtx[right_index].coords.y;
      right_height = vtx[right_index].position.y;
      
      if (++right_index >= vertices) {
        right_index = 0;
      }
      
      right_height = (vtx[right_index].position.y + 1) - (right_height);
      
      right_dx = divide((vtx[right_index].position.x - prev_x) << 16, right_height);
      right_du = divide((vtx[right_index].coords.x - prev_u) << 16, right_height);
      right_dv = divide((vtx[right_index].coords.y - prev_v) << 16, right_height);
      
      right_x = prev_x << 16;
      right_u = prev_u << 16;
      right_v = prev_v << 16;
    }

    left_x += left_dx;
    right_x += right_dx;
    left_u += left_du;
    right_u += right_du;
    left_v += left_dv;
    right_v += right_dv;
  
    tu = left_u;
    tv = left_v;

    // for closing gaps between adjacent edges, tuning is arbitrary
    const uint16_t push_left = 0x10000 * 64;
    const uint16_t push_right = 0x30000 * 64;

    int start_x = ((left_x - push_left) >> 16) & ~1;
    int end_x = ((right_x + push_right) >> 16) & ~1;
    
    len = (end_x - start_x) >> 1;
    start = (uint16_t*)(line + (start_x >> 1));

    // 256 x 256 texture
    #define BLIT \
    { \
      c = (bitmap[(((tu >> 16)) + (((tv >> 16)) << 9)) & 0xFFFF]); \
      tu += du; tv += dv; \
      c |= (bitmap[(((tu >> 16)) + (((tv >> 16)) << 9)) & 0xFFFF]) << 8; \
      tu += du; tv += dv; \
      *start++ = c; \
    }   

    if (len > 0) {
      du = divide((right_u - left_u), len << 1);
      dv = divide((right_v - left_v), len << 1);

      stride = len;
      n = (stride + 7) >> 3;

      switch (stride & 7) {
        case 0: do { BLIT;   
        case 7: BLIT;
        case 6: BLIT;
        case 5: BLIT; 
        case 4: BLIT;   
        case 3: BLIT;
        case 2: BLIT;
        case 1: BLIT;  
        } while (--n); 
      }
    }
    line += 120;
  } while (--length);
}

void IWRAM_CODE wire_polygon(vertex_t *vertices, int num_vertices, uint16_t *texture)
{
  uint16_t i, j;
  for (i = 0; i < num_vertices; ++i) {
    j = (i + 1) % num_vertices;

    line(vertices[i].position.x, vertices[i].position.y, vertices[j].position.x, vertices[j].position.y, 196);
  }  
}

typedef enum {
  //PLANE_NEAR,
  PLANE_LEFT,
  PLANE_RIGHT,
  PLANE_TOP,
  PLANE_BOTTOM,
  NUM_PLANES
} clip_plane_t;

IWRAM_CODE int32_t distance_to_plane(const vertex_t* v, clip_plane_t plane) 
{
  switch (plane) {
    //case PLANE_NEAR:
    //return -v->position.z; 
  case PLANE_LEFT:
    return v->position.x - v->position.z; 
  case PLANE_RIGHT:
    return -v->position.z - v->position.x;
  case PLANE_TOP:
    return -v->position.z - v->position.y;
  case PLANE_BOTTOM:
    return v->position.y - v->position.z; 
  default:
    return 0;
  }
}

IWRAM_CODE int clip_polygon(vertex_t* dest, const vertex_t* source, int num_vertices, clip_plane_t plane) 
{
  vertex_t* previous_vertex = &source[num_vertices - 1];
  vertex_t* temp = dest;

  int32_t previous_dist = distance_to_plane(previous_vertex, plane);

  for (int i = 0; i < num_vertices; i++) {
    vertex_t* current_vertex = &source[i];
    int32_t current_dist = distance_to_plane(current_vertex, plane);

    if (previous_dist >= 0) {
      *temp++ = *previous_vertex;
    }

    if ((previous_dist ^ current_dist) < 0) {
      int t = previous_dist * rcp(previous_dist - current_dist);
      
      temp->position.x = previous_vertex->position.x + (((current_vertex->position.x - previous_vertex->position.x) * t) >> 16);
      temp->position.y = previous_vertex->position.y + (((current_vertex->position.y - previous_vertex->position.y) * t) >> 16);
      temp->position.z = previous_vertex->position.z + (((current_vertex->position.z - previous_vertex->position.z) * t) >> 16);
      temp->coords.x = previous_vertex->coords.x + (((current_vertex->coords.x - previous_vertex->coords.x) * t) >> 16);
      temp->coords.y = previous_vertex->coords.y + (((current_vertex->coords.y - previous_vertex->coords.y) * t) >> 16);
      
      temp++;
    }

    previous_vertex = current_vertex;
    previous_dist = current_dist;
  }

  return temp - dest;
}

inline void copy_face_attributes(face_t *dest, const face_t *source)
{
  dest->texture_index = source->texture_index;
}

IWRAM_CODE int clip_against_frustrum(vertex_t* dest, const vertex_t* source, int num_vertices) 
{
  vertex_t *temp_src = source;
  vertex_t *temp_dst = clip_buffer1;

  int count = num_vertices;

  for (int plane = 0; plane < NUM_PLANES; plane++) {
    count = clip_polygon(temp_dst, temp_src, count, plane);

    temp_src = temp_dst;
    temp_dst = (temp_dst == clip_buffer1) ? clip_buffer2 : clip_buffer1;
  }

  if (temp_src != dest) {
    fast_copy(dest, temp_src, count * sizeof(vertex_t) >> 1);
  }

  return count;
}

void IWRAM_CODE add_to_bin(face_t* face, int depth) {
  int index = min(MAX_BINS - 1, depth >> 2);

  face->next = depth_bins[index].first;
  depth_bins[index].first = face;
}

IWRAM_CODE void render(model_t** models, int model_count) {
  int num_faces = 0, index = 0, depth = 0, i , j;
  shader_t polygon_filler = texture_polygon;
    
  for (i = 0; i < MAX_BINS; i++) {
    depth_bins[i].first = 0;
  }

  for (j = 0; j < model_count; j++) {
    model_t* model = models[j];

    rotate(&model_matrix, model->euler_angles);
    rotate(&camera_matrix, camera.angles);

    for (face_t* f = model->faces; f != 0; f = f->next) {
      for (int i = 0; i < f->vertex_count; i++) {
        scratch_vertices[i] = model->vertex_source[f->vertex_start + i];
        transform(model, &scratch_vertices[i]);
      }

      face_t* face = &transformed_list[num_faces];
      face->vertex_start = index;
      face->vertex_count = clip_against_frustrum(&vertex_buffer[index], scratch_vertices, f->vertex_count);

      if (face->vertex_count >= 3) {
        for (i = 0; i < face->vertex_count; i++) {
          project(&vertex_buffer[index + i]);
        }

        if (!is_clockwise(&vertex_buffer[index])) {
          //depth = min(min(vertex_buffer[index].position.z, vertex_buffer[index + 1].position.z), vertex_buffer[index + 2].position.z);
          depth = (vertex_buffer[index].position.z + vertex_buffer[index + 1].position.z + vertex_buffer[index + 2].position.z) >> 1;

          //current_face->depth = -depth; // legacy: for quicksort

          copy_face_attributes(face, f);
          add_to_bin(face, -depth);
          
          num_faces++;
          index += face->vertex_count;
        }
      }
    }
  }
    
  for (i = MAX_BINS - 1; i >= 0; i--) {
    face_t* face = depth_bins[i].first;
    while (face) {
      polygon_filler(&vertex_buffer[face->vertex_start], face->vertex_count, texture_list[face->texture_index]);
      face = face->next;
    }
  }
}
