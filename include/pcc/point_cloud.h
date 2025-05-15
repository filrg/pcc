#ifndef PCC_POINT_CLOUD_H
#define PCC_POINT_CLOUD_H

#include <pcc/def.h>
#include <pcc/octree.h>
#include <stdint.h>

struct pcc_point_cloud_t
{
  float   *positions;
  uint8_t *colors;
  uint32_t size;
  void (*read_from_file)(pcc_point_cloud_t *, const char *);
  void (*write_to_file)(pcc_point_cloud_t *, const char *, int);
  void (*read_from_octree)(pcc_point_cloud_t *, const pcc_octree_t);
};

PCC_EXPORT void pcc_point_cloud_init(pcc_point_cloud_t *self);
PCC_EXPORT void pcc_point_cloud_destroy(pcc_point_cloud_t *self);

void pcc_point_cloud_read_from_file(pcc_point_cloud_t *self,
                                    const char        *filepath);
void pcc_point_cloud_write_to_file(pcc_point_cloud_t *self,
                                   const char        *filepath,
                                   int                is_binary);
void pcc_point_cloud_read_from_octree(pcc_point_cloud_t *self,
                                      const pcc_octree_t oct);

#endif
