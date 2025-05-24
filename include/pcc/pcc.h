#pragma once

#include "pcc/pcc_export.h"
#include <pcc/def.h>
#include <pcc/point_cloud.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

  PCC_EXPORT void pcc_encode(const float         *positions,
                             const unsigned char *colors,
                             uint32_t             num_points,
                             const float          error,
                             char               **wbuff,
                             uint64_t            *wsize);

  PCC_EXPORT void pcc_decode(const char     *rbuff,
                             uint64_t        rsize,
                             float         **positions,
                             unsigned char **colors,
                             uint32_t       *num_points);

  PCC_EXPORT void pcc_multiple_encode(const pcc_point_cloud_t *pcds,
                                      uint64_t  num_pcds,
                                      float     error,
                                      char    **wbuff,
                                      uint64_t *wsize);

  PCC_EXPORT void pcc_multiple_decode(const char         *rbuff,
                                      uint64_t            rsize,
                                      pcc_point_cloud_t **pcds_out,
                                      uint64_t *num_pcds_out);

#ifdef __cplusplus
} // extern "C"
#endif
