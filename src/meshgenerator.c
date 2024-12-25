#include "meshgenerator.h"
#include "rendering.h"
#include "fixedmath.h"
#include "memory.h"

int face_buffer_cursor = 0;
int vertex_buffer_cursor = 0;

void (*displacement_func)(vertex_t *, uint16_t);

const int16_t X = 34;
const int16_t Z = 64;
const int16_t N = 0;

const vector3_t ico_verts[] = {
  {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z}, {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X}, {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
};

const uint8_t ico_indices[] = {
  0,4,1,0,9,4,9,5,4,4,5,8,4,8,1,8,10,1,8,3,10,5,3,8,5,2,3,2,7,3,7,10,3,7,6,10,7,11,6,11,0,6,0,1,6,6,1,10,9,0,11,9,11,2,9,2,5,7,2,11
};

void finalize_mesh(model_t *model, const int num_faces)
{
  model->faces = &face_list[model->face_start];
  for (int i = 0; i < num_faces - 1; i++) {
    face_list[i].next = &face_list[model->face_start + i + 1];
  }
    
  model->faces[model->face_start + num_faces - 1].next = 0;
}

void new_mesh(model_t *model)
{
  model->face_start = face_buffer_cursor;
  model->vertex_start = vertex_buffer_cursor;

  model->faces[face_buffer_cursor].next = 0;
}

void clear_scene()
{
  face_buffer_cursor = 0;
  vertex_buffer_cursor = 0;
}

void displace_vertex(vertex_t *in, uint16_t value)
{
  int x = in->position.x;
  int y = in->position.y;
  int z = in->position.z;

  int rep = 2;
	int scale = sin(((x * rep) + (value >> 0))) + 
              sin(((y * rep) + (value << 1))) + 
              sin(((z * rep) + (value >> 1)));

  in->position.x += (in->position.x) * (scale >> 4) >> 5; 
  in->position.y += (in->position.y) * (scale >> 4) >> 5; 
  in->position.z += (in->position.z) * (scale >> 4) >> 5; 
}

#if 0

void IWRAM_CODE load_model(model_t *model, const int16_t *model_data, int16_t num_faces, uint8_t flags, uint8_t scale, uint8_t texture_index, uint8_t primitive)
{
  int16_t *src = model_data;

  model->flags = flags;
  model->vertex_source = &raw_vertex_list[model->vertex_start];
  model->faces = &face_list[model->face_start];

  if (!(flags & MODEL_REPLACE)) {
      model->face_start = face_buffer_cursor;
      model->vertex_start = vertex_buffer_cursor;
      model->face_count = num_faces;
      model->vertex_count = num_faces * primitive;
      
      for (uint16_t i = 0; i < num_faces; i++) {
          uint16_t face_index = model->face_start + i;
          face_list[face_index].vertex_count = primitive;
          face_list[face_index].vertex_start = i * primitive;
          face_list[face_index].texture_index = texture_index;
          
          face_list[face_index].next = &face_list[face_index + 1];
      }

      face_list[num_faces].next = 0;
      
      face_buffer_cursor += num_faces;
      vertex_buffer_cursor += (num_faces * primitive);
  }

  if (primitive == 3) {
    for (uint16_t i = 0; i < num_faces; i++) {
        uint16_t index =  model->vertex_start + i * primitive;

        raw_vertex_list[index + 2].position.x = *src++;
        raw_vertex_list[index + 2].position.y = *src++;
        raw_vertex_list[index + 2].position.z = *src++;
        raw_vertex_list[index + 2].coords.x = *src++;
        raw_vertex_list[index + 2].coords.y = *src++;
        raw_vertex_list[index + 2].coords.z = *src++;

        raw_vertex_list[index + 1].position.x = *src++;
        raw_vertex_list[index + 1].position.y = *src++;
        raw_vertex_list[index + 1].position.z = *src++;
        raw_vertex_list[index + 1].coords.x = *src++;
        raw_vertex_list[index + 1].coords.y = *src++;
        raw_vertex_list[index + 1].coords.z = *src++;

        raw_vertex_list[index + 0].position.x = *src++;
        raw_vertex_list[index + 0].position.y = *src++;
        raw_vertex_list[index + 0].position.z = *src++;
        raw_vertex_list[index + 0].coords.x = *src++;
        raw_vertex_list[index + 0].coords.y = *src++;
        raw_vertex_list[index + 0].coords.z = *src++;
    }
  }
  else
  if (primitive == 4) {
    for (uint16_t i = 0; i < num_faces; i++) {
        uint16_t index =  model->vertex_start + i * primitive;

        raw_vertex_list[index + 3].position.x = *src++;
        raw_vertex_list[index + 3].position.y = *src++;
        raw_vertex_list[index + 3].position.z = *src++;
        raw_vertex_list[index + 3].coords.x = *src++;
        raw_vertex_list[index + 3].coords.y = *src++;
        raw_vertex_list[index + 3].coords.z = *src++;

        raw_vertex_list[index + 2].position.x = *src++;
        raw_vertex_list[index + 2].position.y = *src++;
        raw_vertex_list[index + 2].position.z = *src++;
        raw_vertex_list[index + 2].coords.x = *src++;
        raw_vertex_list[index + 2].coords.y = *src++;
        raw_vertex_list[index + 2].coords.z = *src++;

        raw_vertex_list[index + 1].position.x = *src++;
        raw_vertex_list[index + 1].position.y = *src++;
        raw_vertex_list[index + 1].position.z = *src++;
        raw_vertex_list[index + 1].coords.x = *src++;
        raw_vertex_list[index + 1].coords.y = *src++;
        raw_vertex_list[index + 1].coords.z = *src++;

        raw_vertex_list[index + 0].position.x = *src++;
        raw_vertex_list[index + 0].position.y = *src++;
        raw_vertex_list[index + 0].position.z = *src++;
        raw_vertex_list[index + 0].coords.x = *src++;
        raw_vertex_list[index + 0].coords.y = *src++;
        raw_vertex_list[index + 0].coords.z = *src++;
    }
  }
}

#endif

void IWRAM_CODE load_model(model_t *model, const int16_t *model_data, int16_t num_faces, uint8_t flags, uint8_t scale, uint8_t texture_index)
{
  int i;

  model->flags = flags;
  model->vertex_source = &raw_vertex_list[model->vertex_start];
  model->faces = &face_list[model->face_start];

  if (!(flags & MODEL_REPLACE)) {
    model->face_start = face_buffer_cursor;
    model->vertex_start = vertex_buffer_cursor;
    model->face_count = num_faces;
    model->vertex_count = num_faces * 3;

    face_t* current_face = &face_list[model->face_start];
    const int vertex_start_base = model->vertex_start;

    for (i = 0; i < num_faces; i++, current_face++) {
      current_face->vertex_count = 3;
      current_face->vertex_start = i * 3;
      current_face->texture_index = texture_index;
      current_face->next = current_face + 1;
    }
    current_face[-1].next = 0; 

    face_buffer_cursor += num_faces;
    vertex_buffer_cursor += num_faces * 3;
  }

  vertex_t* dest = &raw_vertex_list[model->vertex_start];
  const int16_t* src = model_data;
  const int total_vertices = num_faces * 3;
    
  for (i = 0; i < num_faces; i++) {
    int dest_i = i * 3;

    vertex_t* v = &dest[dest_i + 2];
    v->position.x = *src++;
    v->position.y = *src++;
    v->position.z = *src++;
    v->coords.x = *src++;
    v->coords.y = *src++;
    v->coords.z = *src++;

    v = &dest[dest_i + 1];
    v->position.x = *src++;
    v->position.y = *src++;
    v->position.z = *src++;
    v->coords.x = *src++;
    v->coords.y = *src++;
    v->coords.z = *src++;

    v = &dest[dest_i];
    v->position.x = *src++;
    v->position.y = *src++;
    v->position.z = *src++;
    v->coords.x = *src++;
    v->coords.y = *src++;
    v->coords.z = *src++;
  }
}

/*
  if (flags & SHADING_TEXTURED) {
    for (uint16_t i = 0; i < num_faces; i++) {
        // Set up triangle info
        face_list[i].vertex_count = primitive;
        face_list[i].vertex_start = vertex_index;  // Use running vertex index
        face_list[i].texture_index = texture_index;

#if 0 // 1024 x 1024
        // Load vertices in reverse order (2,1,0) as in original
        raw_vertex_list[vertex_index + 2].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 2].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 2].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 2].coords.x = ((*model_data++) >> 4);
        raw_vertex_list[vertex_index + 2].coords.y = -((*model_data++) >> 5);
        raw_vertex_list[vertex_index + 2].coords.z = ((*model_data++) >> 6);

        raw_vertex_list[vertex_index + 1].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 1].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 1].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 1].coords.x = ((*model_data++) >> 4);
        raw_vertex_list[vertex_index + 1].coords.y = -((*model_data++) >> 5);
        raw_vertex_list[vertex_index + 1].coords.z = ((*model_data++) >> 6);

        raw_vertex_list[vertex_index + 0].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 0].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 0].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 0].coords.x = ((*model_data++) >> 4);
        raw_vertex_list[vertex_index + 0].coords.y = -((*model_data++) >> 5);
        raw_vertex_list[vertex_index + 0].coords.z = ((*model_data++) >> 6);
#else // 256 x 256

        raw_vertex_list[vertex_index + 2].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 2].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 2].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 2].coords.x = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 2].coords.y = -((*model_data++) >> 7);
        raw_vertex_list[vertex_index + 2].coords.z = ((*model_data++) >> 6);

        raw_vertex_list[vertex_index + 1].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 1].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 1].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 1].coords.x = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 1].coords.y = -((*model_data++) >> 7);
        raw_vertex_list[vertex_index + 1].coords.z = ((*model_data++) >> 6);

        raw_vertex_list[vertex_index + 0].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 0].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 0].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 0].coords.x = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 0].coords.y = -((*model_data++) >> 7);
        raw_vertex_list[vertex_index + 0].coords.z = ((*model_data++) >> 6);
        */
        /*
        // Load vertices in reverse order (2,1,0) as in original
        raw_vertex_list[vertex_index + 2].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 2].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 2].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 2].coords.x = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 2].coords.y = -((*model_data++) >> 7);
        *model_data++;

        raw_vertex_list[vertex_index + 1].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 1].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 1].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 1].coords.x = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 1].coords.y = -((*model_data++) >> 7);
        *model_data++;

        raw_vertex_list[vertex_index + 0].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 0].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 0].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 0].coords.x = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 0].coords.y = -((*model_data++) >> 7);
        *model_data++;
#endif

        vertex_index += 3;  // Move to next set of vertices
    }
  }
  else if (flags & SHADING_ENVMAPPED || flags & SHADING_FLAT) {
    for (uint16_t i = 0; i < num_faces; i++) {
        face_list[i].vertex_count = 3;
        face_list[i].vertex_start = vertex_index;  // Use running vertex index
        face_list[i].texture_index = texture_index;

        raw_vertex_list[vertex_index + 2].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 2].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 2].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 2].coords.x = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 2].coords.y = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 2].coords.z = ((*model_data++) >> 6);

        raw_vertex_list[vertex_index + 1].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 1].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 1].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 1].coords.x = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 1].coords.y = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 1].coords.z = ((*model_data++) >> 6);

        raw_vertex_list[vertex_index + 0].position.x = -(((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 0].position.y = (((*model_data++) * scale) >> shift_minus_one);
        raw_vertex_list[vertex_index + 0].position.z = (((*model_data++) * scale) >> shift);
        raw_vertex_list[vertex_index + 0].coords.x = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 0].coords.y = ((*model_data++) >> 6);
        raw_vertex_list[vertex_index + 0].coords.z = ((*model_data++) >> 6);

        vertex_index += 3;  // Move to next set of vertices
    }
  }
 */ 


void IWRAM_CODE _normalize_vector(vector3_t *vector) {
  int max_component = 0;
  if (abs(vector->x) > max_component) {
    max_component = abs(vector->x);
  }
  if (abs(vector->y) > max_component) {
    max_component = abs(vector->y);
  }
  if (abs(vector->z) > max_component) {
    max_component = abs(vector->z);
  }

  if (max_component != 0) {
    int inv = divide(1 << 16, max_component);
    vector->x = (vector->x * inv * 255) >> 16;
    vector->y = (vector->y * inv * 255) >> 16;
    vector->z = (vector->z * inv * 255) >> 16;
  }
}

vector3_t IWRAM_CODE _cross_product(vector3_t v1, vector3_t v2) 
{
  vector3_t result;
  result.x = (v1.y * v2.z - v1.z * v2.y);
  result.y = (v1.z * v2.x - v1.x * v2.z);
  result.z = (v1.x * v2.y - v1.y * v2.x);
  return result;
}

/*
void generate_normals(model_t *model) {
  triangle_t *face = model->triangles;
  while (face != NULL) {
    for (int i = 0; i < face->vertex_count; ++i) {
      face->vertices[i].coords.x = 0;
      face->vertices[i].coords.y = 0;
      face->vertices[i].coords.z = 0;
    }
    face = face->next;
  }

  face = model->triangles;
  while (face != NULL) {
    vector3_t v1, v2;
    v1.x = face->vertices[1].position.x - face->vertices[0].position.x;
    v1.y = face->vertices[1].position.y - face->vertices[0].position.y;
    v1.z = face->vertices[1].position.z - face->vertices[0].position.z;
    v2.x = face->vertices[2].position.x - face->vertices[0].position.x;
    v2.y = face->vertices[2].position.y - face->vertices[0].position.y;
    v2.z = face->vertices[2].position.z - face->vertices[0].position.z;
    face->normal = _cross_product(v1, v2);
    _normalize_vector(&face->normal);

    for (int i = 0; i < 3; ++i) {
      face->vertices[i].coords.x += (face->normal.x >> 8);
      face->vertices[i].coords.y += (face->normal.y >> 8);
      face->vertices[i].coords.z += (face->normal.z >> 8);
    }

    face = face->next;
  }

  face = model->triangles;
  vector3_t temp_normal;
  while (face != NULL) {
    for (int i = 0; i < 3; ++i) {
      temp_normal.x = face->vertices[i].coords.x;
      temp_normal.y = face->vertices[i].coords.y;
      temp_normal.z = face->vertices[i].coords.z;   

      _normalize_vector(&temp_normal);
      face->vertices[i].coords.x = temp_normal.x;
      face->vertices[i].coords.y = temp_normal.y;
      face->vertices[i].coords.z = temp_normal.z;
    }
    face = face->next;
  }
}

void displace_none(vertex_t *in, uint16_t value)
{
}

void subdivide_icosahedron(uint16_t offset, uint8_t mode, uint8_t texture_index)
{
  int i = 0, j = 0;
  vector3_t edge1, edge2, edge3;
  int16_t index = 0;

  switch (mode) {
    case MESH_VERTEXDISPLACE:
      displacement_func = displace_vertex;
      break;
    case MESH_STATIC:
      displacement_func = displace_none;
    default:
    break;
  }

  for (i = 0; i < 20; i++) {
    int16_t t0x0 = ico_verts[ico_indices[index + 0]].x;
    int16_t t0y0 = ico_verts[ico_indices[index + 0]].y;
    int16_t t0z0 = ico_verts[ico_indices[index + 0]].z;

    int16_t t0x1 = ico_verts[ico_indices[index + 1]].x;
    int16_t t0y1 = ico_verts[ico_indices[index + 1]].y;
    int16_t t0z1 = ico_verts[ico_indices[index + 1]].z;

    int16_t t0x2 = ico_verts[ico_indices[index + 2]].x;
    int16_t t0y2 = ico_verts[ico_indices[index + 2]].y;
    int16_t t0z2 = ico_verts[ico_indices[index + 2]].z;
  
    edge1.x = t0x0 + ((t0x1 - t0x0) >> 1);
    edge1.y = t0y0 + ((t0y1 - t0y0) >> 1);
    edge1.z = t0z0 + ((t0z1 - t0z0) >> 1);

    edge2.x = t0x1 + ((t0x2 - t0x1) >> 1);
    edge2.y = t0y1 + ((t0y2 - t0y1) >> 1);
    edge2.z = t0z1 + ((t0z2 - t0z1) >> 1);

    edge3.x = t0x2 + ((t0x0 - t0x2) >> 1);
    edge3.y = t0y2 + ((t0y0 - t0y2) >> 1);
    edge3.z = t0z2 + ((t0z0 - t0z2) >> 1);

    triangle_list[j].texture_index = texture_index;
    triangle_list[j].vertices[0].position.x = edge3.x;
    triangle_list[j].vertices[0].position.y = edge3.y;
    triangle_list[j].vertices[0].position.z = edge3.z;
    triangle_list[j].vertices[1].position.x = t0x0;
    triangle_list[j].vertices[1].position.y = t0y0;
    triangle_list[j].vertices[1].position.z = t0z0;
    triangle_list[j].vertices[2].position.x = edge1.x;
    triangle_list[j].vertices[2].position.y = edge1.y;
    triangle_list[j].vertices[2].position.z = edge1.z;
    displacement_func(&triangle_list[j].vertices[0], offset);
    displacement_func(&triangle_list[j].vertices[1], offset);
    displacement_func(&triangle_list[j].vertices[2], offset);

    j++;

    triangle_list[j].texture_index = texture_index;
    triangle_list[j].vertices[0].position.x = edge1.x;
    triangle_list[j].vertices[0].position.y = edge1.y;
    triangle_list[j].vertices[0].position.z = edge1.z;
    triangle_list[j].vertices[1].position.x = t0x1;
    triangle_list[j].vertices[1].position.y = t0y1;
    triangle_list[j].vertices[1].position.z = t0z1;
    triangle_list[j].vertices[2].position.x = edge2.x;
    triangle_list[j].vertices[2].position.y = edge2.y;
    triangle_list[j].vertices[2].position.z = edge2.z;
    displacement_func(&triangle_list[j].vertices[0], offset);
    displacement_func(&triangle_list[j].vertices[1], offset);
    displacement_func(&triangle_list[j].vertices[2], offset);

    j++;

    triangle_list[j].texture_index = texture_index;
    triangle_list[j].vertices[0].position.x = edge2.x;
    triangle_list[j].vertices[0].position.y = edge2.y;
    triangle_list[j].vertices[0].position.z = edge2.z;
    triangle_list[j].vertices[1].position.x = t0x2;
    triangle_list[j].vertices[1].position.y = t0y2;
    triangle_list[j].vertices[1].position.z = t0z2;
    triangle_list[j].vertices[2].position.x = edge3.x;
    triangle_list[j].vertices[2].position.y = edge3.y;
    triangle_list[j].vertices[2].position.z = edge3.z;
    displacement_func(&triangle_list[j].vertices[0], offset);
    displacement_func(&triangle_list[j].vertices[1], offset);
    displacement_func(&triangle_list[j].vertices[2], offset);

    j++;

    triangle_list[j].texture_index = texture_index;
    triangle_list[j].vertices[0].position.x = edge1.x;
    triangle_list[j].vertices[0].position.y = edge1.y;
    triangle_list[j].vertices[0].position.z = edge1.z;
    triangle_list[j].vertices[1].position.x = edge2.x;
    triangle_list[j].vertices[1].position.y = edge2.y;
    triangle_list[j].vertices[1].position.z = edge2.z;
    triangle_list[j].vertices[2].position.x = edge3.x;
    triangle_list[j].vertices[2].position.y = edge3.y;
    triangle_list[j].vertices[2].position.z = edge3.z;
    displacement_func(&triangle_list[j].vertices[0], offset);
    displacement_func(&triangle_list[j].vertices[1], offset);
    displacement_func(&triangle_list[j].vertices[2], offset);

    j++;
    
    index += 3;
  }

  finalize_mesh(79+1);
}
*/