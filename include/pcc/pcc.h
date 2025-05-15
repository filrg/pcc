#pragma once

#include "pcc/pcc_export.h"
#include <pcc/def.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

  PCC_EXPORT void pcc_encode(const float         *positions,
                             const unsigned char *colors,
                             const uint32_t       num_points,
                             const float          error,
                             char               **wbuff,
                             size_t              *wsize);

  PCC_EXPORT void pcc_decode(const char     *rbuff,
                             const size_t    rsize,
                             float         **positions,
                             unsigned char **colors,
                             uint32_t       *num_points);

#ifdef __cplusplus
} // extern "C"
#endif
