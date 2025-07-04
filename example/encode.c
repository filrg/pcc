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

void write_to_file(const char    *filename,
                   const char    *data,
                   const uint64_t size)
{
  FILE *fp = fopen(filename, "wb"); // "wb" = write binary
  if (!fp)
  {
    perror("Failed to open file");
    return;
  }

  uint64_t written = fwrite(data, 1, size, fp);
  if (written != size)
  {
    perror("Failed to write all data");
  }

  fclose(fp);
}

int main(int argc, char **argv)
{
  pcc_point_cloud_t pcd   = {0};
  char             *wbuff = NULL;
  uint64_t          wsize = 0;

  pcc_point_cloud_init(&pcd);

  pcd.read_from_file(&pcd, argv[1]);

  pcc_encode(pcd.positions,
             pcd.colors,
             pcd.size,
             (const float)atof(argv[2]),
             &wbuff,
             &wsize);

  write_to_file(argv[3], wbuff, wsize);

  pcc_point_cloud_destroy(&pcd);
  free(wbuff);
}
