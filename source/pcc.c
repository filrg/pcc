#include "pcc/pcc.h"
#include <pcc/octree.h>
#include <stdlib.h>
#include <string.h>
#include <zstd.h>

static char *pcc_compress_with_zstd_s(const char *data,
                                      size_t      size,
                                      size_t     *compressed_size)
{
  size_t max_compressed_size = 0;
  char  *compressed          = NULL;
  max_compressed_size        = ZSTD_compressBound(size);
  compressed                 = malloc(max_compressed_size);
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
                                        size_t      compressed_size,
                                        size_t *decompressed_size)
{
  char                    *decompressed = NULL;
  const unsigned long long original_size =
      ZSTD_getFrameContentSize(compressed_data, compressed_size);
  if (original_size == ZSTD_CONTENTSIZE_ERROR ||
      original_size == ZSTD_CONTENTSIZE_UNKNOWN)
  {
    return NULL;
  }

  decompressed = malloc((size_t)original_size);
  if (!decompressed)
  {
    return NULL;
  }

  const size_t actual_size = ZSTD_decompress(
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
                size_t          rsize,
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

void pcc_multiple_encode(const pcc_point_cloud_t *pcds,
                         size_t                   num_pcds,
                         float                    error,
                         char                   **wbuff,
                         size_t                  *wsize)
{
  char  **raw_buffers = malloc(num_pcds * sizeof(char *));
  size_t *raw_sizes   = malloc(num_pcds * sizeof(size_t));
  if (!raw_buffers || !raw_sizes)
  {
    free(raw_buffers);
    free(raw_sizes);
    *wbuff = NULL;
    *wsize = 0;
    return;
  }

  size_t total_size = sizeof(uint32_t); // for num_pcds header

  // Serialize octree (without compression) for each point cloud
  for (size_t i = 0; i < num_pcds; i++)
  {
    pcc_octree_t oct = {0};
    pcc_octree_init(&oct);

    // Serialize octree from point cloud with given error
    oct.read_from_point_cloud(&oct, pcds[i], error);

    char  *buff     = NULL;
    size_t buff_len = 0;
    oct.write_to_buff(&oct, &buff, &buff_len);

    pcc_octree_destroy(&oct);

    if (!buff)
    {
      // Cleanup previously allocated buffers
      for (size_t j = 0; j < i; j++)
        free(raw_buffers[j]);
      free(raw_buffers);
      free(raw_sizes);
      *wbuff = NULL;
      *wsize = 0;
      return;
    }

    raw_buffers[i] = buff;
    raw_sizes[i]   = buff_len;
    total_size += sizeof(uint32_t) + buff_len;
  }

  // Allocate buffer for combined raw serialized data with header
  char *combined = malloc(total_size);
  if (!combined)
  {
    for (size_t i = 0; i < num_pcds; i++)
      free(raw_buffers[i]);
    free(raw_buffers);
    free(raw_sizes);
    *wbuff = NULL;
    *wsize = 0;
    return;
  }

  // Write header (num_pcds)
  // uint32_t *header = (uint32_t *)combined;
  // *header          = (uint32_t)num_pcds;
  uint32_t tempo = (uint32_t)num_pcds;
  memcpy(combined, &tempo, sizeof(uint32_t));

  size_t offset = sizeof(uint32_t);

  // Write sizes + buffers
  for (size_t i = 0; i < num_pcds; i++)
  {
    uint32_t *size_ptr = (uint32_t *)(combined + offset);
    *size_ptr          = (uint32_t)raw_sizes[i];
    offset += sizeof(uint32_t);

    memcpy(combined + offset, raw_buffers[i], raw_sizes[i]);
    offset += raw_sizes[i];

    free(raw_buffers[i]);
  }

  free(raw_buffers);
  free(raw_sizes);

  // Compress once for the whole combined buffer
  *wbuff = pcc_compress_with_zstd_s(combined, total_size, wsize);
  free(combined);
}

void pcc_multiple_decode(const char         *rbuff,
                         size_t              rsize,
                         pcc_point_cloud_t **pcds_out,
                         size_t             *num_pcds_out)
{
  size_t decompressed_size = 0;
  char  *decompressed =
      pcc_decompress_with_zstd_s(rbuff, rsize, &decompressed_size);
  if (!decompressed)
  {
    *pcds_out     = NULL;
    *num_pcds_out = 0;
    return;
  }

  if (decompressed_size < sizeof(uint32_t))
  {
    free(decompressed);
    *pcds_out     = NULL;
    *num_pcds_out = 0;
    return;
  }

  const char *ptr      = decompressed;
  const char *end      = decompressed + decompressed_size;

  // uint32_t    num_pcds = *(uint32_t *)ptr;
  uint32_t    num_pcds = 0;
  memcpy(&num_pcds, ptr, sizeof(uint32_t));
  ptr += sizeof(uint32_t);

  if (num_pcds == 0)
  {
    free(decompressed);
    *pcds_out     = NULL;
    *num_pcds_out = 0;
    return;
  }

  pcc_point_cloud_t *pcds =
      malloc(num_pcds * sizeof(pcc_point_cloud_t));
  if (!pcds)
  {
    free(decompressed);
    *pcds_out     = NULL;
    *num_pcds_out = 0;
    return;
  }

  memset(pcds, 0, num_pcds * sizeof(pcc_point_cloud_t));

  for (uint32_t i = 0; i < num_pcds; i++)
  {
    if (ptr + sizeof(uint32_t) > end)
    {
      // corrupted
      goto error_cleanup;
    }
    uint32_t size_i = *(uint32_t *)ptr;
    ptr += sizeof(uint32_t);
    if (ptr + size_i > end)
    {
      // corrupted
      goto error_cleanup;
    }

    // Deserialize octree for this chunk
    pcc_octree_t      oct = {0};
    pcc_point_cloud_t pcd = {0};
    pcc_octree_init(&oct);
    pcc_point_cloud_init(&pcd);

    oct.read_from_buff(&oct, ptr, size_i);
    pcd.read_from_octree(&pcd, oct);

    pcc_octree_destroy(&oct);

    pcds[i] = pcd;

    ptr += size_i;
  }

  free(decompressed);
  *pcds_out     = pcds;
  *num_pcds_out = num_pcds;
  return;

error_cleanup:
  for (uint32_t j = 0; j < num_pcds; j++)
  {
    free(pcds[j].positions);
    free(pcds[j].colors);
  }
  free(pcds);
  free(decompressed);
  *pcds_out     = NULL;
  *num_pcds_out = 0;
}
