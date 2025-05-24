#ifndef PCC_OCTREE_H
#define PCC_OCTREE_H

#include <pcc/def.h>
#include <pcc/pcc_export.h>
#include <pcc/point_cloud.h>
#include <pcc/vec3f.h>
#include <pcc/vec3u.h>
#include <stdlib.h>

struct pcc_octree_node_t
{
  pcc_vec3u_t        color;
  pcc_octree_node_t *children[PCC_INT_OCT];
  pcc_vec3f_t        min_bound;
  pcc_vec3f_t        max_bound;
};

PCC_EXPORT void pcc_octree_node_init(pcc_octree_node_t *self);
PCC_EXPORT void pcc_octree_node_destroy(pcc_octree_node_t *self);

struct pcc_octree_t
{
  pcc_octree_node_t root;
  uint32_t          count;
  void (*count_leaf_nodes)(pcc_octree_t *);
  void (*read_from_point_cloud)(pcc_octree_t *,
                                const pcc_point_cloud_t,
                                const float);
  void (*write_to_buff)(pcc_octree_t *, char **, uint64_t *);
  void (*read_from_buff)(pcc_octree_t *,
                         const char *,
                         const uint64_t);
};

PCC_EXPORT void pcc_octree_init(pcc_octree_t *self);
PCC_EXPORT void pcc_octree_destroy(pcc_octree_t *self);

void            pcc_octree_count_leaf_nodes(pcc_octree_t *self);
void            pcc_octree_read_from_point_cloud(pcc_octree_t           *self,
                                                 const pcc_point_cloud_t pcd,
                                                 const float             error);
void            pcc_octree_write_to_buff(pcc_octree_t *self,
                                         char        **data,
                                         uint64_t     *size);
void            pcc_octree_read_from_buff(pcc_octree_t  *self,
                                          const char    *data,
                                          const uint64_t size);

#endif
