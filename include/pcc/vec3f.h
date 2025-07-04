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

#ifndef PCC_VEC3F_H
#define PCC_VEC3F_H

typedef struct
{
  float x, y, z;
} pcc_vec3f_t;

static inline float pcc_absf(float value)
{
  return (value < 0.0F) ? -value : value;
}

static inline pcc_vec3f_t pcc_vec3f_add(const pcc_vec3f_t vec1,
                                        const pcc_vec3f_t vec2)
{
  return (pcc_vec3f_t){.x = vec1.x + vec2.x,
                       .y = vec1.y + vec2.y,
                       .z = vec1.z + vec2.z};
}

static inline pcc_vec3f_t pcc_vec3f_sub(const pcc_vec3f_t vec1,
                                        const pcc_vec3f_t vec2)
{
  return (pcc_vec3f_t){.x = vec1.x - vec2.x,
                       .y = vec1.y - vec2.y,
                       .z = vec1.z - vec2.z};
}

static inline pcc_vec3f_t pcc_vec3f_scale(const pcc_vec3f_t vec,
                                          const float       scalar)
{
  return (pcc_vec3f_t){
      .x = vec.x * scalar, .y = vec.y * scalar, .z = vec.z * scalar};
}

static inline pcc_vec3f_t
pcc_vec3f_scale_reverse(const float scalar, const pcc_vec3f_t vec)
{
  return pcc_vec3f_scale(vec, scalar);
}

static inline int pcc_vec3f_equal(const pcc_vec3f_t vec1,
                                  const pcc_vec3f_t vec2)
{
  const float epsilon = PCC_FLOAT_ZERO;
  return (pcc_absf(vec1.x - vec2.x) < epsilon) &&
         (pcc_absf(vec1.y - vec2.y) < epsilon) &&
         (pcc_absf(vec1.z - vec2.z) < epsilon);
}

static inline int pcc_vec3f_not_equal(const pcc_vec3f_t vec1,
                                      const pcc_vec3f_t vec2)
{
  return !pcc_vec3f_equal(vec1, vec2);
}

static inline float pcc_vec3f_dot(const pcc_vec3f_t vec1,
                                  const pcc_vec3f_t vec2)
{
  return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
}

static inline float pcc_vec3f_distance2(const pcc_vec3f_t point1,
                                        const pcc_vec3f_t point2)
{
  pcc_vec3f_t sub = {0};
  sub             = pcc_vec3f_sub(point1, point2);
  return pcc_vec3f_dot(sub, sub);
}

static inline unsigned char
pcc_vec3f_octant(const pcc_vec3f_t point, const pcc_vec3f_t center)
{
  unsigned char octant = 0;
  if (point.x > center.x)
  {
    octant |= 4;
  }
  if (point.y > center.y)
  {
    octant |= 2;
  }
  if (point.z > center.z)
  {
    octant |= 1;
  }
  return octant;
}

#endif // PCC_VEC3F_H
