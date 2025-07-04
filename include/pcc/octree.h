/* This file is part of PCC.
 * Copyright (C) 2025 FIL Research Group, ANSA Laboratory
 *
 * PCC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

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
