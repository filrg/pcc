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

#define _POSIX_C_SOURCE 199309L
#include <emscripten/emscripten.h>
#include <pcc/pcc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

EMSCRIPTEN_KEEPALIVE
void pcc_free(void *ptr)
{
  if (ptr != NULL)
  {
    free(ptr);
  }
}

EMSCRIPTEN_KEEPALIVE
void *pcc_malloc(uint32_t size)
{
  if (size != 0)
  {
    return malloc((size_t)size);
  }
  return NULL;
}

EMSCRIPTEN_KEEPALIVE
pcc_point_cloud_t *wasm_pcc_multiple_decode(const char *rbuff,
                                            uint32_t    rsize,
                                            uint32_t   *num)
{
  pcc_point_cloud_t *pcds     = NULL;
  uint64_t           num_pcds = 0;
  uint64_t           i        = 0;

  struct timespec    start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  // timer
  pcc_multiple_decode(rbuff, (uint64_t)rsize, &pcds, &num_pcds);

  clock_gettime(CLOCK_MONOTONIC, &end);

  long   seconds      = end.tv_sec - start.tv_sec;
  long   nanoseconds  = end.tv_nsec - start.tv_nsec;
  double milliseconds = seconds * 1000.0 + nanoseconds / 1e6;

  printf("Elapsed time: %.3f ms\n", milliseconds);

  if (!pcds || num_pcds == 0)
  {
    fprintf(stderr, "Failed to decode any point clouds\n");
    return 0;
  }

  *num = (uint32_t)num_pcds;

  return pcds;
}

EMSCRIPTEN_KEEPALIVE
pcc_point_cloud_t *pcc_get_point_cloud_at(pcc_point_cloud_t *pc_list,
                                          int                index)
{
  return &pc_list[index];
}

EMSCRIPTEN_KEEPALIVE
float *pcc_get_positions(pcc_point_cloud_t *pc)
{
  return pc->positions;
}

EMSCRIPTEN_KEEPALIVE
uint8_t *pcc_get_colors(pcc_point_cloud_t *pc)
{
  return pc->colors;
}

EMSCRIPTEN_KEEPALIVE
uint32_t pcc_get_size(pcc_point_cloud_t *pc)
{
  return pc->size;
}
