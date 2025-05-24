#include <float.h>
#include <pcc/octree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void pcc_append_to_buffer_s(char      **data,
                                   uint64_t   *size,
                                   const void *src,
                                   uint64_t    len)
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
                                            uint64_t          *size)
{
  if (!curr)
  {
    return;
  }

  unsigned char child_info = 0x00;
  for (int i = 0; i < PCC_INT_OCT; i++)
  {
    if (curr->children[i])
    {
      child_info |= (unsigned char)(1 << i);
    }
  }
  pcc_append_to_buffer_s(
      data, size, &child_info, sizeof(child_info));

  if (!child_info)
  {
    pcc_append_to_buffer_s(
        data, size, &curr->color, sizeof(curr->color));
  }
  for (int i = 0; i < PCC_INT_OCT; i++)
  {
    if (curr->children[i])
    {
      pcc_octree_node_write_to_buff_s(curr->children[i], data, size);
    }
  }
}

static void
pcc_octree_node_insert_point_s(pcc_octree_node_t *curr,
                               const pcc_vec3f_t  position,
                               const pcc_vec3u_t  color,
                               const float        error2)
{
  if (!curr)
  {
    return;
  }
  pcc_vec3f_t center = pcc_vec3f_scale(
      pcc_vec3f_add(curr->min_bound, curr->max_bound),
      PCC_FLOAT_HALF);
  if (pcc_vec3f_distance2(position, center) < error2)
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
    {
      new_min.x = center.x, new_max.x = curr->max_bound.x;
    }
    if (octant & 2)
    {
      new_min.y = center.y, new_max.y = curr->max_bound.y;
    }
    if (octant & 1)
    {
      new_min.z = center.z, new_max.z = curr->max_bound.z;
    }
    curr->children[octant] =
        (pcc_octree_node_t *)malloc(sizeof(pcc_octree_node_t));
    pcc_octree_node_init(curr->children[octant]);
    curr->children[octant]->min_bound = new_min;
    curr->children[octant]->max_bound = new_max;
  }
  pcc_octree_node_insert_point_s(
      curr->children[octant], position, color, error2);
}

static void
pcc_octree_node_count_leaf_nodes_s(pcc_octree_node_t *curr,
                                   uint32_t          *count)
{
  if (!curr)
  {
    return;
  }
  int is_leaf = 1;
  for (int i = 0; i < PCC_INT_OCT; i++)
  {
    if (curr->children[i])
    {
      is_leaf = 0;
      pcc_octree_node_count_leaf_nodes_s(curr->children[i], count);
    }
  }
  if (is_leaf)
  {
    (*count)++;
  }
}

static void pcc_octree_node_read_from_buff_s(pcc_octree_node_t *curr,
                                             const char       **data,
                                             const char        *end)
{
  pcc_vec3f_t   center     = {0};
  unsigned char child_info = 0x00;

  center                   = pcc_vec3f_scale(
      pcc_vec3f_add(curr->min_bound, curr->max_bound),
      PCC_FLOAT_HALF);

  memcpy(&child_info, *data, sizeof(unsigned char));
  *data += sizeof(unsigned char);

  if (!child_info)
  {
    memcpy(&curr->color, *data, sizeof(pcc_vec3u_t));
    *data += sizeof(pcc_vec3u_t);
  }

  for (int octant = 0; octant < PCC_INT_OCT; octant++)
  {
    if (child_info & (1 << octant))
    {
      pcc_vec3f_t new_min = curr->min_bound;
      pcc_vec3f_t new_max = center;

      if (octant & 4)
      {
        new_min.x = center.x, new_max.x = curr->max_bound.x;
      }
      if (octant & 2)
      {
        new_min.y = center.y, new_max.y = curr->max_bound.y;
      }
      if (octant & 1)
      {
        new_min.z = center.z, new_max.z = curr->max_bound.z;
      }

      curr->children[octant] =
          (pcc_octree_node_t *)malloc(sizeof(pcc_octree_node_t));
      pcc_octree_node_init(curr->children[octant]);

      curr->children[octant]->min_bound = new_min;
      curr->children[octant]->max_bound = new_max;

      pcc_octree_node_read_from_buff_s(
          curr->children[octant], data, end);
    }
  }
}

void pcc_octree_node_init(pcc_octree_node_t *self)
{
  *self = (pcc_octree_node_t){0};
  return;
}

void pcc_octree_node_destroy(pcc_octree_node_t *self)
{
  for (int i = 0; i < PCC_INT_OCT; i++)
  {
    if (self->children[i] != NULL)
    {
      pcc_octree_node_destroy(self->children[i]);
      free(self->children[i]);
    }
  }
  *self = (pcc_octree_node_t){0};
  return;
}

void pcc_octree_init(pcc_octree_t *self)
{
  *self                       = (pcc_octree_t){0};
  self->count_leaf_nodes      = pcc_octree_count_leaf_nodes;
  self->read_from_point_cloud = pcc_octree_read_from_point_cloud;
  self->write_to_buff         = pcc_octree_write_to_buff;
  self->read_from_buff        = pcc_octree_read_from_buff;
  pcc_octree_node_init(&(self->root));
  return;
}
void pcc_octree_destroy(pcc_octree_t *self)
{
  pcc_octree_node_destroy(&(self->root));
  *self = (pcc_octree_t){0};
  return;
}

void pcc_octree_count_leaf_nodes(pcc_octree_t *self)
{
  self->count = 0;
  pcc_octree_node_count_leaf_nodes_s(&(self->root), &(self->count));
  return;
}

void pcc_octree_read_from_point_cloud(pcc_octree_t           *self,
                                      const pcc_point_cloud_t pcd,
                                      const float             error)
{
  float error2 = error * error;
  if (pcd.size == 0 || self == NULL)
  {
    return;
  }

  pcc_vec3f_t max_bound = {
      .x = -FLT_MAX, .y = -FLT_MAX, .z = -FLT_MAX};
  pcc_vec3f_t min_bound = {.x = FLT_MAX, .y = FLT_MAX, .z = FLT_MAX};

  for (uint32_t i = 0; i < pcd.size; i++)
  {
    pcc_vec3f_t pnt = ((pcc_vec3f_t *)pcd.positions)[i];
    if (max_bound.x < pnt.x)
    {
      max_bound.x = pnt.x;
    }
    if (max_bound.y < pnt.y)
    {
      max_bound.y = pnt.y;
    }
    if (max_bound.z < pnt.z)
    {
      max_bound.z = pnt.z;
    }
    if (min_bound.x > pnt.x)
    {
      min_bound.x = pnt.x;
    }
    if (min_bound.y > pnt.y)
    {
      min_bound.y = pnt.y;
    }
    if (min_bound.z > pnt.z)
    {
      min_bound.z = pnt.z;
    }
  }

  self->root.max_bound = max_bound;
  self->root.min_bound = min_bound;

  for (uint32_t i = 0; i < pcd.size; i++)
  {
    pcc_octree_node_insert_point_s(&(self->root),
                                   ((pcc_vec3f_t *)pcd.positions)[i],
                                   ((pcc_vec3u_t *)pcd.colors)[i],
                                   error2);
  }
  return;
}

void pcc_octree_write_to_buff(pcc_octree_t *self,
                              char        **data,
                              uint64_t     *size)
{
  self->count_leaf_nodes(self);
  pcc_append_to_buffer_s(
      data, size, &(self->count), sizeof(self->count));
  pcc_append_to_buffer_s(data,
                         size,
                         &(self->root.min_bound),
                         sizeof(self->root.min_bound));
  pcc_append_to_buffer_s(data,
                         size,
                         &(self->root.max_bound),
                         sizeof(self->root.max_bound));
  pcc_octree_node_write_to_buff_s(&(self->root), data, size);
}

void pcc_octree_read_from_buff(pcc_octree_t  *self,
                               const char    *data,
                               const uint64_t size)
{
  const char *ptr = data;
  const char *end = data + size;

  memcpy(&self->count, ptr, sizeof(self->count));
  ptr += sizeof(self->count);
  memcpy(&(self->root.min_bound), ptr, sizeof(pcc_vec3f_t));
  ptr += sizeof(pcc_vec3f_t);
  memcpy(&(self->root.max_bound), ptr, sizeof(pcc_vec3f_t));
  ptr += sizeof(pcc_vec3f_t);

  pcc_octree_node_read_from_buff_s(&self->root, &ptr, end);
}
