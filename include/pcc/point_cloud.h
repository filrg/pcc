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
