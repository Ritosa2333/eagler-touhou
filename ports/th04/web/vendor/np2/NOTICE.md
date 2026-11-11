# Upstream components

NP2-wasm by irori, based on Neko Project II.

- Source: https://github.com/irori/np2-wasm
- Prebuilt distribution: https://irori.github.io/np2-wasm/
- Downloaded: 2026-10-03
- License: BSD-3-Clause; see LICENSE in this directory.
- Local copies: np2-wasm.js, np21.js, np21.wasm, font.bmp.
- The launcher uses NP21 (i386) for compatibility with the disk's VEM486 driver.
- font_cn.bmp is copied from the user's existing local game bundle, not from upstream.

The local wrapper removes the unused NP2 factory; NP21 remains unchanged.
SHA256SUMS.json records hashes of the shipped runtime files, including this wrapper.
The original wrapper and unused NP2 core are retained in the sibling maintenance directory.

## Separate input-lockstep adapter

`np21-lockstep.js` is generated locally by `tools/build_lockstep_runtime.py`
from the shipped `np21.js`; it shares the existing `np21.wasm`. The generated
adapter changes scheduling, clocks, input dispatch and audio callback ownership.
`web/netplay/np2-clock.js` holds the readable deterministic clock/audio adapter.
The ordinary local-play runtime uses `np21-60.js`, generated from `np21.js` by
`tools/build_frame_runtime.py`. It limits native main-loop callbacks to 60 Hz
without changing the native elapsed-time clock or the upstream WASM.
`frame-runtime.json` records its source and generated hashes. Source/build fingerprints
are recorded in `web/netplay/runtime.json`. Runtime behavior is pending user
acceptance; generation is not gameplay or network verification.


## Cached native dispatch (2026-10-07)

Both generated adapters cache `WebAssembly.Table.get` results per instance.
`tools/optimize_np21_dispatch.py` restricts this change to the pinned WASM hash.
The table is static (2244 entries); the disassembly audit found no table mutation
instructions. Upstream WASM, emulated CPU clocks, native function bodies and
exception handling remain unchanged. Regenerate both adapters with
`python tools/build_lockstep_runtime.py`.
