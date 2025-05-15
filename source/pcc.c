#include "pcc/pcc.h"
#include <pcc/octree.h>
#include <pcc/point_cloud.h>
#include <stdlib.h>
#include <zstd.h>

static char *pcc_compress_with_zstd_s(const char  *data,
                                      const size_t size,
                                      size_t      *compressed_size)
{
  size_t max_compressed_size = ZSTD_compressBound(size);
  char  *compressed          = malloc(max_compressed_size);
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

static char *pcc_decompress_with_zstd_s(const char  *compressed_data,
                                        const size_t compressed_size,
                                        size_t *decompressed_size)
{
  unsigned long long const original_size =
      ZSTD_getFrameContentSize(compressed_data, compressed_size);
  if (original_size == ZSTD_CONTENTSIZE_ERROR ||
      original_size == ZSTD_CONTENTSIZE_UNKNOWN)
  {
    return NULL;
  }

  char *decompressed = malloc((size_t)original_size);
  if (!decompressed)
  {
    return NULL;
  }

  size_t const actual_size = ZSTD_decompress(
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
                const uint32_t       num_points,
                const float          error,
                char               **wbuff,
                size_t              *wsize)
{
  pcc_point_cloud_t pcd        = {0};
  pcc_octree_t      oct        = {0};
  char             *lossly     = NULL;
  size_t            lossly_len = 0;

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
                const size_t    rsize,
                float         **positions,
                unsigned char **colors,
                uint32_t       *num_points)
{
  pcc_octree_t      oct        = {0};
  pcc_point_cloud_t pcd        = {0};
  char             *lossly     = NULL;
  size_t            lossly_len = 0;

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
