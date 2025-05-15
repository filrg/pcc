/* This file is part of PCSTREAM.
 * Copyright (C) 2025 FIL Research Group, ANSA Laboratory
 *
 * PCSTREAM is free software: you can redistribute it and/or modify
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
#include <cstring>
#include <miniply/miniply.h>
#include <ply.h>
#include <stdint.h>
#include <stdlib.h>
#define MSH_PLY_INCLUDE_LIBC_HEADERS
#define MSH_PLY_IMPLEMENTATION
#include <msh_ply.h>

static bool _mesh_ply_writer(const char *filename,
                             float      *pos,
                             uint32_t    num_verts,
                             uint32_t   *indices,
                             uint32_t    num_indices,
                             int         binary)
{
  msh_ply_desc_t descriptors[2] = {0};
  int32_t        nf             = 0;
  int32_t        nv             = 0;
  const char    *write_format   = binary ? "wb" : "w";
  msh_ply_t     *pf             = NULL;
  const char    *vertex_props[] = {"x", "y", "z"};
  const char    *face_props[]   = {"vertex_indices"};

  nv                            = (int32_t)num_verts;
  nf                            = (int32_t)(num_indices / 3);

  descriptors[0] = (msh_ply_desc_t){.element_name = (char *)"vertex",
                                    .property_names = vertex_props,
                                    .num_properties = 3,
                                    .data_type      = MSH_PLY_FLOAT,
                                    .data           = &pos,
                                    .data_count     = &nv};

  descriptors[1] = (msh_ply_desc_t){.element_name   = (char *)"face",
                                    .property_names = face_props,
                                    .num_properties = 1,
                                    .data_type      = MSH_PLY_INT32,
                                    .list_type      = MSH_PLY_UINT8,
                                    .data           = &indices,
                                    .data_count     = &nf,
                                    .list_size_hint = 3};

  pf             = msh_ply_open(filename, write_format);
  if (pf)
  {
    msh_ply_add_descriptor(pf, &descriptors[0]);
    msh_ply_add_descriptor(pf, &descriptors[1]);
    msh_ply_write(pf);
  }
  else
    return 0;

  msh_ply_close(pf);
  return 1;
}

static bool _vert_col_ply_loader(const char    *filename,
                                 float         *pos,
                                 unsigned char *rgb)
{
  miniply::PLYReader reader(filename);
  if (!reader.valid())
  {
    return 0;
  }

  uint32_t propIdxs[3];
  bool     gotVerts = false;

  while (reader.has_element() && !gotVerts)
  {
    if (reader.element_is(miniply::kPLYVertexElement))
    {
      if (!reader.load_element() || !reader.find_pos(propIdxs))
      {
        break;
      }

      reader.extract_properties(
          propIdxs, 3, miniply::PLYPropertyType::Float, pos);
      if (reader.find_color(propIdxs))
      {
        reader.extract_properties(
            propIdxs, 3, miniply::PLYPropertyType::UChar, rgb);
      }
      gotVerts = true;
    }
    reader.next_element();
  }
  return 1;
}

static bool _pos_indices_ply_loader(const char *filename,
                                    float      *pos,
                                    uint32_t   *indices)
{
  miniply::PLYReader reader(filename);
  if (!reader.valid())
  {
    return 0;
  }
  uint32_t             faceIdxs[3];
  miniply::PLYElement *faceElem = reader.get_element(
      reader.find_element(miniply::kPLYFaceElement));
  if (faceElem == nullptr)
  {
    return 0;
  }
  faceElem->convert_list_to_fixed_size(
      faceElem->find_property("vertex_indices"), 3, faceIdxs);

  uint32_t indexes[3];
  bool     gotVerts = false, gotFaces = false;

  while (reader.has_element() && (!gotVerts || !gotFaces))
  {
    if (reader.element_is(miniply::kPLYVertexElement) &&
        reader.load_element() && reader.find_pos(indexes))
    {
      reader.extract_properties(
          indexes, 3, miniply::PLYPropertyType::Float, pos);
      gotVerts = true;
    }
    else if (!gotFaces &&
             reader.element_is(miniply::kPLYFaceElement) &&
             reader.load_element())
    {
      reader.extract_properties(
          faceIdxs, 3, miniply::PLYPropertyType::UInt, indices);
      gotFaces = true;
    }
    if (gotVerts && gotFaces)
    {
      break;
    }
    reader.next_element();
  }
  if (!gotVerts || !gotFaces)
  {
    return 0;
  }
  return 1;
}

uint32_t ply_count_vertex(const char *filename)
{
  miniply::PLYReader reader(filename);
  if (!reader.valid())
  {
    return 0;
  }
  uint32_t elemIndex =
      reader.find_element(miniply::kPLYVertexElement);
  if (elemIndex == miniply::kInvalidIndex)
    return 0;
  return reader.get_element(elemIndex)->count;
}
uint32_t ply_count_face(const char *filename)
{
  miniply::PLYReader reader(filename);
  if (!reader.valid())
  {
    return 0;
  }
  uint32_t elemIndex = reader.find_element(miniply::kPLYFaceElement);
  if (elemIndex == miniply::kInvalidIndex)
    return 0;
  return reader.get_element(elemIndex)->count;
}
// Wrapper implementation
int ply_pointcloud_loader(const char    *filename,
                          float         *pos,
                          unsigned char *rgb)
{
  return _vert_col_ply_loader(filename, pos, rgb) ? 1 : 0;
}
int ply_mesh_loader(const char *filename,
                    float      *pos,
                    uint32_t   *indices)
{
  return _pos_indices_ply_loader(filename, pos, indices) ? 1 : 0;
}
int ply_mesh_writer(const char *filename,
                    float      *pos,
                    uint32_t    num_verts,
                    uint32_t   *indices,
                    uint32_t    num_indices,
                    int         binary)
{
  return _mesh_ply_writer(
             filename, pos, num_verts, indices, num_indices, binary)
           ? 1
           : 0;
}
