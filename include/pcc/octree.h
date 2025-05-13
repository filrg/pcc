#ifndef PCC_OCTREE_H
#define PCC_OCTREE_H

#include <pcc/stdint.h>
#include <pcc/vec3f.h>
#include <pcc/vec3u.h>

typedef struct pcc_octree_node_t pcc_octree_node_t;

struct pcc_octree_node_t
{
  pcc_vec3u_t        color;
  pcc_octree_node_t *children[8];
  pcc_vec3f_t        min_bound;
  pcc_vec3f_t        max_bound;
};

void pcc_octree_node_init(pcc_octree_node_t *self);
void pcc_octree_node_destroy(pcc_octree_node_t *self);

typedef struct pcc_octree_t pcc_octree_t;

struct pcc_octree_t
{
  pcc_octree_node_t root;
  int               count;

  void              insert_point_cloud(pcc_octree_t *,
                                       const pcc_point_cloud_t,
                                       const float);
  void              write_to_buff(pcc_octree_t *, char **, size_t *);
};

void pcc_octree_init(pcc_octree_t *self);
void pcc_octree_destroy(pcc_octree_t *self);

void pcc_octree_insert_point_cloud(pcc_octree_t           *self,
                                   const pcc_point_cloud_t pc,
                                   const float             error);
void pcc_octree_write_to_buff(pcc_octree_t *self,
                              char        **data,
                              size_t       *size);

#endif
