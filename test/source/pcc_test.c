#include "pcc/pcc.h"

#include <string.h>

int main(int argc, char const *argv[])
{
  (void)argc;
  (void)argv;

  return strcmp(exported_function(), "pcc") == 0 ? 0 : 1;
}
