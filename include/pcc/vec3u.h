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

#ifndef PCC_VEC3U_H
#define PCC_VEC3U_H

#include <stdint.h>

typedef struct
{
  uint8_t x, y, z;
} pcc_vec3u_t;

static inline pcc_vec3u_t pcc_vec3u_add(const pcc_vec3u_t vec1,
                                        const pcc_vec3u_t vec2)
{
  return (pcc_vec3u_t){.x = (uint8_t)(vec1.x + vec2.x),
                       .y = (uint8_t)(vec1.y + vec2.y),
                       .z = (uint8_t)(vec1.z + vec2.z)};
}

static inline pcc_vec3u_t pcc_vec3u_sub(const pcc_vec3u_t vec1,
                                        const pcc_vec3u_t vec2)
{
  return (pcc_vec3u_t){.x = (uint8_t)(vec1.x - vec2.x),
                       .y = (uint8_t)(vec1.y - vec2.y),
                       .z = (uint8_t)(vec1.z - vec2.z)};
}

static inline pcc_vec3u_t pcc_vec3u_scale(const pcc_vec3u_t vec,
                                          const uint8_t     scalar)
{
  return (pcc_vec3u_t){.x = (uint8_t)(vec.x * scalar),
                       .y = (uint8_t)(vec.y * scalar),
                       .z = (uint8_t)(vec.z * scalar)};
}

static inline pcc_vec3u_t
pcc_vec3u_scale_reverse(const uint8_t scalar, const pcc_vec3u_t vec)
{
  return pcc_vec3u_scale(vec, scalar);
}

static inline int pcc_vec3u_equal(const pcc_vec3u_t vec1,
                                  const pcc_vec3u_t vec2)
{
  return (vec1.x == vec2.x) && (vec1.y == vec2.y) &&
         (vec1.z == vec2.z);
}

static inline int pcc_vec3u_not_equal(const pcc_vec3u_t vec1,
                                      const pcc_vec3u_t vec2)
{
  return !pcc_vec3u_equal(vec1, vec2);
}

#endif // PCC_VEC3U_H
