#include <emscripten/emscripten.h>
#include <pcc/pcc.h>
#include <stdint.h>
#include <stdlib.h>

EMSCRIPTEN_KEEPALIVE
void pcc_free(void *ptr)
{
  free(ptr);
}

EMSCRIPTEN_KEEPALIVE
void wasm_pcc_decode(const char     *rbuff,
                     uint64_t        rsize,
                     float         **positions,
                     unsigned char **colors,
                     uint32_t       *num_points)
{
  pcc_decode(rbuff, rsize, positions, colors, num_points);
}
