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
