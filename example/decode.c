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

#include <pcc/pcc.h>
#include <pcc/point_cloud.h>
#include <stdio.h>
#include <stdlib.h>

// Reads the file and returns a heap-allocated buffer + its size
char *read_from_file(const char *filename, uint64_t *out_size)
{
  FILE *fp = fopen(filename, "rb");
  if (!fp)
  {
    perror("Failed to open file");
    return NULL;
  }

  // Seek to end to get file size
  if (fseek(fp, 0, SEEK_END) != 0)
  {
    perror("fseek failed");
    fclose(fp);
    return NULL;
  }

  long filesize = ftell(fp);
  if (filesize < 0)
  {
    perror("ftell failed");
    fclose(fp);
    return NULL;
  }
  rewind(fp); // Go back to beginning

  // Allocate buffer
  char *buffer = malloc((uint64_t)filesize);
  if (!buffer)
  {
    perror("malloc failed");
    fclose(fp);
    return NULL;
  }

  // Read contents
  uint64_t read = fread(buffer, 1, (uint64_t)filesize, fp);
  if (read != (uint64_t)filesize)
  {
    perror("fread failed or incomplete");
    free(buffer);
    fclose(fp);
    return NULL;
  }

  fclose(fp);
  *out_size = read;
  return buffer;
}

int main(int argc, char **argv)
{
  pcc_point_cloud_t pcd   = {0};
  char             *rbuff = NULL;
  uint64_t          rsize = 0;

  pcc_point_cloud_init(&pcd);

  rbuff = read_from_file(argv[1], &rsize);

  pcc_decode(rbuff, rsize, &pcd.positions, &pcd.colors, &pcd.size);

  pcd.write_to_file(&pcd, argv[2], atoi(argv[3]));

  pcc_point_cloud_destroy(&pcd);
  free(rbuff);
}
