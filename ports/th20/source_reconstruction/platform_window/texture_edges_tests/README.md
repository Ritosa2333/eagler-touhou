# Transparent texture edge recovery

Production source: `sprite_renderer/texture_edges.hpp/.cpp`.
`repair_transparent_texels(IDirect3DTexture9&)` recovers 0x451910 and four
neighbor accumulation helpers 0x451640, 0x451700, 0x4517c0 and 0x451880.

Supported formats are A8R8G8B8 (also UNKNOWN/0), A1R5G5B5, A4R4G4B4 and
A8R3G3B2. Only alpha-zero texels receive new RGB values: the integer mean of
nonzero-alpha horizontal/vertical neighbors, zero when none exist. There is
no diagonal propagation or alpha change. Unsupported formats still preserve
the original GetSurfaceLevel/GetDesc/LockRect/UnlockRect/Release sequence.

The isolated original CPU oracle has 222,014 passing cases: every 16-bit
input to the three packed-format helpers, 20,000 32-bit samples, null
accumulators, 5,400 memory-backed COM surface cases and two null surfaces.
Surface comparisons cover complete pixel and guard/padding bytes plus ordered
COM calls. Included are positive, negative and odd pitches; dimensions zero
and one; zero/full/mixed alpha; UNKNOWN alias and unsupported formats; and
failing HRESULTs with valid output pointers, which the original ignores.

The oracle's COM implementation is a test fixture. Production source has no
PE loader, retained instructions or dummy GPU implementation. This validates
the edge-repair algorithm and COM calling behavior, not all GPU/game output.
