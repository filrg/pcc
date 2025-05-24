#define _POSIX_C_SOURCE 199309L
#include <pcc/pcc.h>
#include <pcc/point_cloud.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *read_from_file(const char *filename, uint64_t *out_size)
{
  FILE    *fp       = NULL;
  long     filesize = 0;
  char    *buffer   = NULL;
  uint64_t read     = 0;

  fp                = fopen(filename, "rb");
  if (!fp)
  {
    perror("Failed to open file");
    return NULL;
  }

  if (fseek(fp, 0, SEEK_END) != 0)
  {
    perror("fseek failed");
    fclose(fp);
    return NULL;
  }

  filesize = ftell(fp);
  if (filesize < 0)
  {
    perror("ftell failed");
    fclose(fp);
    return NULL;
  }
  rewind(fp);

  buffer = malloc((uint64_t)filesize);
  if (!buffer)
  {
    perror("malloc failed");
    fclose(fp);
    return NULL;
  }

  read = fread(buffer, 1, (uint64_t)filesize, fp);
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
  uint64_t           rsize    = 0;
  char              *rbuff    = NULL;
  pcc_point_cloud_t *pcds     = NULL;
  uint64_t           num_pcds = 0;
  uint64_t           i        = 0;
  char               filename[512];

  struct timespec    start, end;

  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s input.bin output_format\n", argv[0]);
    fprintf(stderr, "Example: %s input.bin rec%%04d.ply\n", argv[0]);
    return 1;
  }

  rbuff = read_from_file(argv[1], &rsize);
  if (!rbuff)
  {
    return 1;
  }

  clock_gettime(CLOCK_MONOTONIC, &start);
  // timer
  pcc_multiple_decode(rbuff, rsize, &pcds, &num_pcds);

  clock_gettime(CLOCK_MONOTONIC, &end);

  long   seconds      = end.tv_sec - start.tv_sec;
  long   nanoseconds  = end.tv_nsec - start.tv_nsec;
  double milliseconds = seconds * 1000.0 + nanoseconds / 1e6;

  printf("Elapsed time: %.3f ms\n", milliseconds);

  free(rbuff);

  if (!pcds || num_pcds == 0)
  {
    fprintf(stderr, "Failed to decode any point clouds\n");
    return 1;
  }

  for (i = 0; i < num_pcds; ++i)
  {
    snprintf(filename, sizeof(filename), argv[2], i);
    pcds[i].write_to_file(
        &pcds[i], filename, 0); // 0 or whatever options needed
    pcc_point_cloud_destroy(&pcds[i]);
  }

  free(pcds);

  return 0;
}
