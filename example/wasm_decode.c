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
