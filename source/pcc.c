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

#include "pcc/pcc.h"
#include <pcc/octree.h>
#include <stdlib.h>
#include <string.h>
#include <zstd.h>

static char *pcc_compress_with_zstd_s(const char *data,
                                      uint64_t    size,
                                      uint64_t   *compressed_size)
{
  uint64_t max_compressed_size = 0;
  char    *compressed          = NULL;
  max_compressed_size          = ZSTD_compressBound(size);
  compressed                   = malloc(max_compressed_size);
  if (!compressed)
  {
    return NULL;
  }

  *compressed_size =
      ZSTD_compress(compressed, max_compressed_size, data, size, 1);
  if (ZSTD_isError(*compressed_size))
  {
    free(compressed);
    return NULL;
  }

  return compressed;
}

static char *pcc_decompress_with_zstd_s(const char *compressed_data,
                                        uint64_t    compressed_size,
                                        uint64_t *decompressed_size)
{
  char                    *decompressed = NULL;
  const unsigned long long original_size =
      ZSTD_getFrameContentSize(compressed_data, compressed_size);
  if (original_size == ZSTD_CONTENTSIZE_ERROR ||
      original_size == ZSTD_CONTENTSIZE_UNKNOWN)
  {
    return NULL;
  }

  decompressed = malloc((uint64_t)original_size);
  if (!decompressed)
  {
    return NULL;
  }

  const uint64_t actual_size = ZSTD_decompress(
      decompressed, original_size, compressed_data, compressed_size);
  if (ZSTD_isError(actual_size))
  {
    free(decompressed);
    return NULL;
  }

  *decompressed_size = actual_size;
  return decompressed;
}

void pcc_encode(const float         *positions,
                const unsigned char *colors,
                uint32_t             num_points,
                const float          error,
                char               **wbuff,
                uint64_t            *wsize)
{
  pcc_point_cloud_t pcd        = {0};
  pcc_octree_t      oct        = {0};
  char             *lossly     = NULL;
  uint64_t          lossly_len = 0;

  pcc_point_cloud_init(&pcd);
  pcc_octree_init(&oct);

  pcd.positions = positions;
  pcd.colors    = colors;
  pcd.size      = num_points;

  oct.read_from_point_cloud(&oct, pcd, error);
  oct.write_to_buff(&oct, &lossly, &lossly_len);

  *wbuff = pcc_compress_with_zstd_s(lossly, lossly_len, wsize);

  pcc_octree_destroy(&oct);
  free(lossly);
}

void pcc_decode(const char     *rbuff,
                uint64_t        rsize,
                float         **positions,
                unsigned char **colors,
                uint32_t       *num_points)
{
  pcc_octree_t      oct        = {0};
  pcc_point_cloud_t pcd        = {0};
  char             *lossly     = NULL;
  uint64_t          lossly_len = 0;

  pcc_octree_init(&oct);
  pcc_point_cloud_init(&pcd);

  lossly = pcc_decompress_with_zstd_s(rbuff, rsize, &lossly_len);

  oct.read_from_buff(&oct, lossly, lossly_len);

  pcd.read_from_octree(&pcd, oct);

  *positions  = pcd.positions;
  *colors     = pcd.colors;
  *num_points = pcd.size;

  pcc_octree_destroy(&oct);
  free(lossly);
}

void pcc_multiple_encode(const pcc_point_cloud_t *pcds,
                         uint64_t                 num_pcds,
                         float                    error,
                         char                   **wbuff,
                         uint64_t                *wsize)
{
  uint64_t  lossly_total_len = 0;
  char     *ptr              = NULL;
  // stuf needs to be free
  char    **lossly_arr       = NULL;
  uint64_t *lossly_len_arr   = NULL;
  char     *lossly_total     = NULL;

  lossly_arr     = (char **)malloc(sizeof(char *) * num_pcds);
  lossly_len_arr = (uint64_t *)malloc(sizeof(uint64_t) * num_pcds);

  lossly_total_len += sizeof(num_pcds);

  for (uint64_t i = 0; i < num_pcds; i++)
  {
    lossly_arr[i]     = NULL;
    lossly_len_arr[i] = 0;

    pcc_octree_t oct  = {0};

    pcc_octree_init(&oct);

    oct.read_from_point_cloud(&oct, pcds[i], error);
    oct.write_to_buff(&oct, &(lossly_arr[i]), &(lossly_len_arr[i]));

    lossly_total_len +=
        sizeof(lossly_len_arr[i]) + lossly_len_arr[i];

    pcc_octree_destroy(&oct);
  }

  lossly_total = (char *)malloc(lossly_total_len);
  ptr          = lossly_total;

  memcpy(ptr, &num_pcds, sizeof(num_pcds));
  ptr += sizeof(num_pcds);

  for (uint64_t i = 0; i < num_pcds; i++)
  {
    memcpy(ptr, &(lossly_len_arr[i]), sizeof(lossly_len_arr[i]));
    ptr += sizeof(lossly_len_arr[i]);

    memcpy(ptr, lossly_arr[i], lossly_len_arr[i]);
    ptr += lossly_len_arr[i];
  }

  *wbuff = pcc_compress_with_zstd_s(
      lossly_total, lossly_total_len, wsize);

  free(lossly_total);
  free(lossly_len_arr);
  for (uint64_t i = 0; i < num_pcds; i++)
  {
    free(lossly_arr[i]);
  }
  free(lossly_arr);
}

void pcc_multiple_decode(const char         *rbuff,
                         uint64_t            rsize,
                         pcc_point_cloud_t **pcds_out,
                         uint64_t           *num_pcds_out)
{
  char    *ptr              = NULL;

  char    *lossly_total     = NULL;
  uint64_t lossly_total_len = 0;
  uint64_t num_pcds         = 0;

  lossly_total =
      pcc_decompress_with_zstd_s(rbuff, rsize, &lossly_total_len);
  ptr = lossly_total;

  memcpy(&num_pcds, ptr, sizeof(num_pcds));
  ptr += sizeof(num_pcds);

  *pcds_out = (pcc_point_cloud_t *)malloc(sizeof(pcc_point_cloud_t) *
                                          num_pcds);

  for (uint64_t i = 0; i < num_pcds; i++)
  {
    pcc_point_cloud_init(&((*pcds_out)[i]));
  }

  for (uint64_t i = 0; i < num_pcds; i++)
  {
    uint64_t     lossly_len = 0;
    pcc_octree_t oct        = {0};
    pcc_octree_init(&oct);

    memcpy(&lossly_len, ptr, sizeof(lossly_len));
    ptr += sizeof(lossly_len);

    oct.read_from_buff(&oct, ptr, lossly_len);
    ptr += lossly_len;

    (*pcds_out)[i].read_from_octree(&((*pcds_out)[i]), oct);

    pcc_octree_destroy(&oct);
  }
  *num_pcds_out = num_pcds;
  free(lossly_total);
}
