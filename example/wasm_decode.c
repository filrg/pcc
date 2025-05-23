#include <emscripten/emscripten.h>
#include <pcc/pcc.h>
#include <stdint.h>
#include <stdlib.h>

// Optional: a free wrapper for JS
EMSCRIPTEN_KEEPALIVE
void pcc_free(void *ptr)
{
  free(ptr);
}

// Just make sure pcc_decode is kept alive by the linker
EMSCRIPTEN_KEEPALIVE
void wasm_pcc_decode(const char     *rbuff,
                     size_t          rsize,
                     float         **positions,
                     unsigned char **colors,
                     uint32_t       *num_points)
{
  pcc_decode(rbuff, rsize, positions, colors, num_points);
}
