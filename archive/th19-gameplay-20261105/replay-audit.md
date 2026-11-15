# Original ReplayInf evidence

EXE SHA256: `8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9`.

- `0x550dd0` sets up ReplayInf recording/playback callbacks and restores the RNG seed and stage state during playback initialization.
- `0x550250` opens `replay/%s`, checks header DWORD `0x72383174` (little-endian `t18r`) and WORD version `6`. The loader decrypts and decompresses before parsing up to six stage segments, with a `0x140` global header and `0x2558` stage header. Input segment lengths use six-byte units.
- `0x550130` updates physical input edge/repeat state and advances the active frame counter. Its decompiled body does not write the web runtime's 80-byte input records.
- `0x550120` dispatches to `0x550750` for playback. Each native six-byte frame contains three uint16 values: held, pressed and released button masks. Six bytes of 0xFF terminate playback. This differs from the current web 80-byte RLE input ABI.
- These findings establish existing native replay machinery. They do not establish a complete TH19 two-player replay file contract or interoperability of the web format.

The gameplay update preserves the tested web `.rpy` format. Rebuilding a native-compatible writer remains separate work pending verification of the complete TH19 recording path and state layout. A filename extension alone does not imply compatibility.
