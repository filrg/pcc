#ifndef PCC_VEC3F_H
#define PCC_VEC3F_H

typedef struct
{
  union
  {
    struct
    {
      float x, y, z;
    };
    struct
    {
      float r, g, b;
    };
  };
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
