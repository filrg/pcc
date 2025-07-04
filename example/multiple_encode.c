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
#include <pcc/pcc.h>
#include <pcc/point_cloud.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void write_to_file(const char  *filename,
                   const char  *data,
                   const size_t size)
{
  FILE    *fp      = NULL;
  uint64_t written = 0;

  fp               = fopen(filename, "wb"); // "wb" = write binary
  if (!fp)
  {
    perror("Failed to open file");
    return;
  }

  written = fwrite(data, 1, size, fp);
  if (written != size)
  {
    perror("Failed to write all data");
  }

  fclose(fp);
}

int main(int argc, char **argv)
{
  int                i, j;
  float              error;
  char              *output_file;
  int                num_pcds;
  pcc_point_cloud_t *pcds  = NULL;
  char              *wbuff = NULL;
  uint64_t           wsize = 0;

  struct timespec    start, end;

  if (argc < 4)
  {
    fprintf(
        stderr,
        "Usage: %s <error> <output_file> <input1> [input2 ...]\n",
        argv[0]);
    return 1;
  }

  error       = (float)atof(argv[1]);
  output_file = argv[2];
  num_pcds    = argc - 3;
  pcds        = malloc(sizeof(pcc_point_cloud_t) * num_pcds);
  if (!pcds)
  {
    fprintf(stderr, "Failed to allocate memory for point clouds\n");
    return 1;
  }

  for (i = 0; i < num_pcds; ++i)
  {
    pcc_point_cloud_init(&pcds[i]);
    pcds[i].read_from_file(&pcds[i], argv[i + 3]);
  }

  clock_gettime(CLOCK_MONOTONIC, &start);
  // timer
  pcc_multiple_encode(
      pcds, (uint64_t)num_pcds, error, &wbuff, &wsize);

  clock_gettime(CLOCK_MONOTONIC, &end);

  long   seconds      = end.tv_sec - start.tv_sec;
  long   nanoseconds  = end.tv_nsec - start.tv_nsec;
  double milliseconds = seconds * 1000.0 + nanoseconds / 1e6;

  printf("Elapsed time: %.3f ms\n", milliseconds);

  if (!wbuff)
  {
    fprintf(stderr, "Encoding failed\n");
    for (i = 0; i < num_pcds; ++i)
      pcc_point_cloud_destroy(&pcds[i]);
    free(pcds);
    return 1;
  }

  write_to_file(output_file, wbuff, wsize);

  for (i = 0; i < num_pcds; ++i)
    pcc_point_cloud_destroy(&pcds[i]);
  free(pcds);
  free(wbuff);

  return 0;
}
