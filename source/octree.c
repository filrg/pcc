#include <pcc/octree.h>
#include <stdio.h>
#include <stdlib.h>
static void pcc_append_to_buffer_s(char      **data,
                                   size_t     *size,
                                   const void *src,
                                   size_t      len)
{
  *data = realloc(*data, *size + len);
  if (!*data)
  {
    fprintf(stderr, "Memory allocation failed\n");
  }
  memcpy(*data + *size, src, len);
  *size += len;
}

static void pcc_octree_node_write_to_buff_s(pcc_octree_node_t *curr,
                                            char             **data,
                                            size_t            *size)
{
  if (!curr)
    return;

  unsigned char child_info = 0x00;
  for (int i = 0; i < 8; ++i)
    if (curr->children[i])
      child_info |= (1 << i);

  pcc_append_to_buffer_s(
      data, size, &child_info, sizeof(unsigned char));

  if (!child_info)
    pcc_append_to_buffer_s(
        data, size, &curr->colors, sizeof(pcc_vec3u_t));

  for (int i = 0; i < 8; ++i)
    if (curr->children[i])
      pcc_octree_node_write_to_buff_s(curr->children[i], data, size);
}

static void
pcc_octree_node_insert_point_s(pcc_octree_node_t *curr,
                               const pcc_vec3f_t  position,
                               const pcc_vec3u_t  color,
                               const float        error2)
{
  if (!curr)
    return;
  pcc_vec3f_t center = pcc_vec3f_scale(
      pcc_vec3f_add(curr->min_bound, curr->max_bound), 0.5f);
  if (pcc_vec3f_distance(position, center) < error2)
  {
    curr->color = color;
    return;
  }

  unsigned char octant = pcc_vec3f_octant(position, center);
  if (!curr->children[octant])
  {
    pcc_vec3f_t new_min = curr->min_bound;
    pcc_vec3f_t new_max = center;
    if (octant & 4)
      new_min.x = center.x, new_max.x = node->max_bound.x;
    if (octant & 2)
      new_min.y = center.y, new_max.y = node->max_bound.y;
    if (octant & 1)
      new_min.z = center.z, new_max.z = node->max_bound.z;
    curr->children[octant] =
        (pcc_octree_node_t)malloc(sizeof(pcc_octree_node_t));
    pcc_octree_node_init(curr->children[octant]);
    curr->children[octant].min_bound = new_min;
    curr->children[octant].max_bound = new_max;
  }
  insert_point(curr->children[octant], position, color, error2);
}

void pcc_octree_node_init(pcc_octree_node_t *self)
{
  *self = (pcc_octree_node_init){0};
  return;
}

void pcc_octree_node_destroy(pcc_octree_node_t *self)
{
  for (int i = 0; i < 8; i++)
  {
    if (self->children[i] != NULL)
    {
      pcc_octree_node_destroy(self->children[i]);
      free(self->children[i]);
    }
  }
  *self = (pcc_octree_t){0};
  return;
}

void pcc_octree_init(pcc_octree_t *self)
{
  *self = (pcc_octree_t){0};
  pcc_octree_node_init(&(self->root));
  return;
}
void pcc_octree_destroy(pcc_octree_t *self)
{
  pcc_octree_node_destroy(&(self->root));
  *self = (pcc_octree_t){0};
  return;
}

void pcc_octree_insert_point_cloud(pcc_octree_t           *self,
                                   const pcc_point_cloud_t pc,
                                   const float             error)
{
  float error2 = error * error;
  if (pc.size == 0 || self == NULL)
    return;

  for (int i = 0; i < pc.size; i++)
  {
    pcc_octree_node_insert_point_s(&(self->root),
                                   ((pcc_vec3f_t *)pc.positions)[i],
                                   ((pcc_vec3u_t *)pc.colors)[i],
                                   error2);
  }
  return;
}

void pcc_octree_write_to_buff(pcc_octree_t *self,
                              char        **data,
                              size_t       *size)
{

    pcc_octree_node_write_to_buff_s(self->root, data, size);
}
