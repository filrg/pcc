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

/* This file is part of PCSTREAM.
 * Copyright (C) 2025 FIL Research Group, ANSA Laboratory
 *
 * PCSTREAM is free software: you can redistribute it and/or modify
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
#ifndef PLY_H_
#define PLY_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  uint32_t ply_count_vertex(const char *filename);
  uint32_t ply_count_face(const char *filename);
  int      ply_pointcloud_loader(const char    *filename,
                                 float         *pos,
                                 unsigned char *rgb);
  int      ply_mesh_loader(const char *filename,
                           float      *pos,
                           uint32_t   *indices);

  int      ply_mesh_writer(const char *filename,
                           float      *pos,
                           uint32_t    num_verts,
                           uint32_t   *indices,
                           uint32_t    num_indices,
                           int         binary);
#ifdef __cplusplus
}
#endif
#endif // PLY_H_
