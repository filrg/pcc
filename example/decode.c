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
