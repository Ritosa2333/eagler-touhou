#pragma once
#include "CustomGeometry.hpp"

namespace th12 {
struct MeshRecord {
  int32_t bank, sprite, textureEntry, layer, blend;
  uint32_t primitive, pass;
  int32_t drawPriority;
  uint32_t owner, nodeId, vertexOffset, vertexCount, filter, addressFlags;
};
static_assert(sizeof(MeshRecord) == 56);
extern std::vector<MeshRecord> meshDrawState;
extern std::vector<custom_geometry::Vertex> meshVertexState;
void captureCustomGeometry();
} // namespace th12
