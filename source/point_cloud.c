#include <pcc/point_cloud.h>
#include <ply.h>
#include <stdio.h>
#include <stdlib.h>

void pcc_point_cloud_init(pcc_point_cloud_t *self)
{
  *self                  = (pcc_point_cloud_t){0};
  self->read_from_file   = pcc_point_cloud_read_from_file;
  self->write_to_file    = pcc_point_cloud_write_to_file;
  self->read_from_octree = pcc_point_cloud_read_from_octree;
  return;
}
void pcc_point_cloud_destroy(pcc_point_cloud_t *self)
{
  if (self->positions != NULL)
  {
    free(self->positions);
  }
  if (self->colors != NULL)
  {
    free(self->colors);
  }
  *self = (pcc_point_cloud_t){0};
  return;
}

void pcc_point_cloud_read_from_file(pcc_point_cloud_t *self,
                                    const char        *filepath)
{
  self->size = ply_count_vertex(filepath);

  if (self->size == 0)
  {
    return;
  }
  self->positions = (float *)malloc(sizeof(float) * 3 * self->size);
  self->colors = (uint8_t *)malloc(sizeof(uint8_t) * 3 * self->size);

  ply_pointcloud_loader(filepath, self->positions, self->colors);
  return;
}
void pcc_point_cloud_write_to_file(pcc_point_cloud_t *self,
                                   const char        *filepath,
                                   int                is_binary)
{
  FILE *file = fopen(filepath, "wb");
  if (!file)
  {
    perror("Error opening file");
    return;
  }

  if (is_binary)
  {
    // Binary PLY Header
    fprintf(file, "ply\n");
    fprintf(file, "format binary_little_endian 1.0\n");
    fprintf(file, "element vertex %u\n", self->size);
    fprintf(file, "property float x\n");
    fprintf(file, "property float y\n");
    fprintf(file, "property float z\n");
    fprintf(file, "property uchar red\n");
    fprintf(file, "property uchar green\n");
    fprintf(file, "property uchar blue\n");
    fprintf(file, "end_header\n");

    for (uint32_t i = 0; i < self->size; i++)
    {
      fwrite(&(self->positions[i * 3]),
             sizeof(float),
             3,
             file); // x, y, z
      fwrite(&(self->colors[i * 3]),
             sizeof(uint8_t),
             3,
             file); // r, g, b
    }
  }
  else
  {
    // ASCII PLY Header
    fprintf(file, "ply\n");
    fprintf(file, "format ascii 1.0\n");
    fprintf(file, "element vertex %u\n", self->size);
    fprintf(file, "property float x\n");
    fprintf(file, "property float y\n");
    fprintf(file, "property float z\n");
    fprintf(file, "property uchar red\n");
    fprintf(file, "property uchar green\n");
    fprintf(file, "property uchar blue\n");
    fprintf(file, "end_header\n");

    for (uint32_t i = 0; i < self->size; i++)
    {
      fprintf(file,
              "%f %f %f %u %u %u\n",
              (double)self->positions[i * 3],
              (double)self->positions[i * 3 + 1],
              (double)self->positions[i * 3 + 2],
              self->colors[i * 3],
              self->colors[i * 3 + 1],
              self->colors[i * 3 + 2]);
    }
  }

  fclose(file);
  return;
}

static void pcc_point_cloud_read_from_octree_node_s(
    pcc_point_cloud_t *self, const pcc_octree_node_t *curr)
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
      pcc_point_cloud_read_from_octree_node_s(self,
                                              curr->children[i]);
    }
  }
  if (is_leaf)
  {
    pcc_vec3f_t center = {0};
    center             = pcc_vec3f_scale(
        pcc_vec3f_add(curr->min_bound, curr->max_bound),
        PCC_FLOAT_HALF);

    ((pcc_vec3f_t *)self->positions)[self->size] = center;
    ((pcc_vec3u_t *)self->colors)[self->size]    = curr->color;
    self->size++;
  }
}

void pcc_point_cloud_read_from_octree(pcc_point_cloud_t *self,
                                      const pcc_octree_t oct)
{
  if (oct.count == 0)
  {
    return;
  }
  self->positions = (float *)malloc(sizeof(float) * 3 * oct.count);
  self->colors = (uint8_t *)malloc(sizeof(uint8_t) * 3 * oct.count);
  pcc_point_cloud_read_from_octree_node_s(self, &(oct.root));
}
