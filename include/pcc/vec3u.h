#ifndef PCC_VEC3U_H
#define PCC_VEC3U_H

#include <stdint.h>

typedef struct
{
  union
  {
    struct
    {
      uint8_t x, y, z;
    };
    struct
    {
      uint8_t r, g, b;
    };
  };
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
