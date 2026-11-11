# NP21 mobile runtime

Install the build-only dependency with `npm install --ignore-scripts` in this
directory (WABT pinned to 1.0.39), then run `python tools/build_lockstep_runtime.py`
from the project root. Deploy the complete generated `web` directory; no Node or
WABT is needed on the web server. Keep both original and mobile WASM files.

The pinned upstream binary is never overwritten. Exact WAT anchors fail closed.
The native invoke bridge eliminates five WASM-to-JS dispatch sites; it preserves
numeric longjmp, JS exception identity and the stack on catchable exceptions.
Uncatchable native WASM traps stop the instance at the JS frame boundary.
Browsers without WebAssembly.JSTag/Exception use the original binary. The module
option `disableMobileOptimization` selects that compatibility path for testing.

TH03's 19-cycle and TH04's 17-cycle idle loops are recognized by exact bytes,
CPU flags, operands, mode and memory mapping. Only complete loops are elided;
the final partial loop and interrupt boundary run in the original interpreter.
Page checks do not mutate the TLB. Diagnostic counters are removed, leaving the
original two mutable WASM globals for rollback snapshots.

TH03's blocking attack-warning `frame_delay(1)` additionally recognizes the
audited 17-cycle polling loop at IP 0x196. It requires AX/counter zero, the
stack argument one, matching comparison flags, ordinary RAM and identity
pages in real/VM86 mode. The original 27 refresh warning remains intact.
`node tools/test_frame_wait.cjs` compares the pinned interpreter's complete
heap at every cycle remainder and checks rejection paths.

Network checksums use a separate 259-byte read-only WASM helper, retaining
the existing FNV-1a algorithm and byte range. It adds no native mutable state;
unsupported helpers fall back to JavaScript. Generate the embedded helper
with `node tools/build_native_hash.cjs`, then rebuild the lockstep runtime.
`node tools/test_native_hash.mjs` checks byte tails, boundaries, memory growth,
the full heap, and fallback against the original checksum.

Local games additionally yield after at most two native frames, retaining up to
12 pending frames, and upload RGB565 pixels directly to WebGL. Native timing and
audio remain unchanged. Software rendering retains the SDL presentation path.
The original game's simulation rate is approximately 56.4 Hz, not 60 Hz.

Network games retain the original deterministic 60 Hz host clock, frame batching,
software presentation, audio and snapshot logic. Runtime manifests fingerprint
the new binary as well as the compatibility runtime. Both peers must refresh.
