# Bounded TH128 browser-host replay codec

## Plan and scope

Goal: translate the verified Python native replay resource codec, not game
simulation, into a standalone browser ES module. Spec: full-port plan task C
and `docs/research/replay.md`. Python remains unchanged.

Only new `site/runtime/replay.mjs`, `tests/replay-codec.test.mjs` and this
document belong to this worker. No host, pipeline, server, build or other
test edits; no commits, original executable runs or E: drive writes.

- [x] Observe failing synthetic native-byte and strict validation tests.
- [x] Implement bounded cipher/LZSS and preservation-oriented encoding.
- [x] Verify all three actual demos and edited bytes against Python.
- [x] Publish the exact integration contract and remaining limitations.

Architecture: dependency-free ES module, fixed file/payload/frame budgets,
raw-byte edit authority and lazy frame tuples. Native uint64 timestamps use
`bigint`; all offsets/counts use validated integer arithmetic. Encoding may
reuse verified original ciphertext or force bounded literal-only compression.

## Attribution

The translated cipher, LZSS and format interpretation retain the same MIT
reference: kikones34/th128-replay-fixer, commit
`595d1451edbf4a55e121d45174b877f6eb0446d5`. The complete license is already
in `docs/research/replay-license.txt`, byte-verified against that commit.
Pinned source hashes and actual-byte corrections are in `replay.md`.

```text
https://github.com/kikones34/th128-replay-fixer/tree/595d1451edbf4a55e121d45174b877f6eb0446d5
https://encoding.spec.whatwg.org/
```

## Exact exports for main

```js
export function decodeReplay(input) {}
export function encodeReplay(decoded, options = {}) {}
```

These are the only exports. `decodeReplay` accepts an ordinary, same-realm
`Uint8Array` (including Node Buffer in tests), takes an owned snapshot, and
returns synchronously. It respects the view's byte offset and byte length,
not its entire underlying buffer. Arrays, strings, ArrayBuffer, DataView,
other typed-array element types and spoofed Uint8Array prototypes fail.
Detached, shared and resizable backing storage also fails explicitly.
No Node imports, fetches, DOM access, server routes or host wiring are used.

`encodeReplay(decoded, { recompress = false } = {})` returns `Uint8Array`.
Options must be a plain record and `recompress`, when supplied, must be an
actual boolean. Null, numeric/string booleans and array options are rejected,
not coerced. With `recompress: false`, the original encrypted payload is
reused only after its bounded decode equals the assembled raw components.
A changed payload or invalid cached ciphertext causes fresh literal encoding.
With `recompress: true`, the cache is bypassed and literal encoding is forced.
The encoded output is decoded for structural validation before return.

### Returned structure

Property names intentionally match Python. `Bytes` below means an ordinary
owned `Uint8Array`, not a hex/base64 string. `number` values are JavaScript
Numbers; native uint32/signed int32 fields are exactly representable.

```text
{
  format: "th128-replay/1",
  header: {
    raw: Bytes[36], replayVersion: 2, gameVersion: number,
    compressedSize: number, uncompressedSize: number, userDataOffset: number
  },
  metadata: {
    raw: Bytes[112], nameRaw: Bytes[12], name: string | null,
    timestamp: bigint, scoreRaw: number, score: number,
    gameConfigRaw: Bytes[60], slowdown: number, stageCount: number,
    route: number, routeName: string | null,
    rank: number, rankName: string | null, lastStage: number
  },
  stages: [{
    rawHeader: Bytes[144], stage: number, stageName: string | null,
    seed: number, numFrames: number, size: number,
    scoreRaw: number, score: number,
    shotPowerRaw: number, shotPower: number,
    maxPivRaw: number, maxPiv: number,
    positionRaw: [number, number], position: [number, number],
    continues: number, graze: number,
    motivationRaw: number, motivation: number,
    perfectFreezeRaw: number, perfectFreeze: number,
    freezeArea: number,
    inputRaw: Bytes[numFrames * 6], inputs: LazyReplayInputs,
    fpsRaw: Bytes
  }],
  payloadTailRaw: Bytes,
  encodedPayload: Bytes,
  rawPayload: Bytes,
  dataGapRaw: Bytes,
  userBlocks: [{
    headerRaw: Bytes[12], kind: number,
    data: Bytes, raw: Bytes[12 + data.length], text: string | null
  }],
  fileTailRaw: Bytes
}
```

`metadata.timestamp` is **always bigint**, even for the demo dates that fit
in Number. It preserves the entire uint64 range, including
`18446744073709551615n`; do not convert arbitrary dates to Number.
Use `.toString()` when producing JSON diagnostics. Other derived numeric
views use the same scaling as Python: score times ten, shot power plus one,
positions divided by 128 and PIV/motivation/perfect-freeze divided by 100.
These are format-reference interpretations, not new gameplay semantics.

Float views may be NaN/Infinity; their exact raw encodings remain in raw
headers. Unknown route, rank and stage labels are `null`, not guessed.
Text views use best-effort WHATWG `shift_jis` with replacement and are `null`
if that TextDecoder is unavailable. This is not a promise of complete
Python `cp932` display equivalence for every extension byte. Raw name/USER
bytes remain exact and authoritative; actual demo names match Python.

### Frame accessor

```js
const decoded = decodeReplay(bytes);
const stage = decoded.stages[0];
const count = stage.inputs.length;
const [holding, pressed, released] = stage.inputs.at(0);
const finalTuple = stage.inputs.at(-1);
```

`stage.inputs` is a frozen accessor object, **not an array of frame objects**.
It has a read-only `length`, a `raw` getter referencing `stage.inputRaw`,
`at(index)` and `[Symbol.iterator]()`; there is no bracket-index API.
`at` returns a fresh three-element uint16 tuple array in
`[holding, pressed, released]` order. Negative indices count from the end.
All 16 key bits, including unknown bits and `holding === 0xffff`, survive.

Indices must be actual safe-integer Numbers within range: strings, bigint,
booleans, null/undefined, objects, fractions, NaN, infinities and unsafe
integers throw. Out-of-range access throws rather than returning undefined.
Transferring/detaching frame storage makes access fail explicitly, not read
zeros. Iteration is lazy; avoid `[...stage.inputs]` for large recordings if
you do not intentionally want to allocate a tuple per frame.

```js
for (const [holding, pressed, released] of stage.inputs) {
  consumeNativeInputTuple(holding, pressed, released);
}
```

The consumer above is illustrative only. This module does not simulate
frames, map inputs into a game core or certify any native playback behavior.

## Edit authority and preservation

The encoder consumes only these raw components:

- `header.raw`, `metadata.raw`.
- Each stage's `rawHeader`, `inputRaw` and `fpsRaw`.
- `payloadTailRaw`, `dataGapRaw`, `fileTailRaw`.
- Each USER block's `headerRaw` and `data`.
- Optional `encodedPayload`, used only as a verified ciphertext cache.

Parsed scalar fields, `rawPayload`, frame tuple objects and USER `raw`/`text`
are views, not edit instructions. For example:

```js
const raw = decoded.metadata.raw;
const data = new DataView(raw.buffer, raw.byteOffset, raw.byteLength);
data.setUint32(20, 9000, true);
const encoded = encodeReplay(decoded);
const updated = decodeReplay(encoded);
```

The caller must validate edited scalar values before using DataView setters;
JavaScript setters themselves perform coercion, and the codec accepts raw
bytes, not unchecked scalar-edit arguments. Internally all computed uint32
fields are checked as safe integers in range before writing them.

The encoder recomputes only USER offset, compressed/uncompressed lengths,
stage count, per-stage frame counts/body sizes and USER block lengths. It
retains raw unknown header words, uint64 timestamp bits, float encodings,
USER tags, gaps and tails. USER tags are independent uint32 fields at byte
8, not the upper half of a uint64 length. Exactly zero or two parsed USER
blocks are supported. Unknown bytes after them remain an opaque file tail.

Raw typed arrays are mutable. Derived metadata/length views do not refresh
when you mutate/replace bytes; decode the encoded output to obtain refreshed
views. USER text is not automatically regenerated after a score/name edit.
Returned byte components are independent of the original input and of the
aggregate `rawPayload`; altering that aggregate does not silently edit the
authoritative component fields.

## Fixed bounds and failure contract

| Bound | Maximum |
| --- | ---: |
| Complete native file | 16 MiB |
| Encrypted/compressed payload | 8 MiB |
| Decompressed payload | 32 MiB |
| Combined USER blocks including headers | 4 MiB |
| Stage count | 1..3 |
| Aggregate frame count | 1,000,000 |

Unlike Python's optional tightened budgets, this browser API exposes only
these fixed ceilings; callers cannot increase them. All ordinary malformed
binary/record input throws an Error with:

```text
name = "ReplayFormatError"
code = "TH128_REPLAY_FORMAT"
message = a specific structural/budget diagnostic
```

No partial replay is returned on failure. Lengths are checked before copying
or expansion; every span/USER offset is checked and cannot overlap ciphertext
or escape its container. Offsets and counts do not use truncating uint32
coercions. LZSS rejects uninitialized ring references, incomplete tokens,
expansion beyond the declaration, early termination and nonzero/excess tail
bits. As in Python, only 1..21 zero bits after exact output are accepted as
a full/truncated terminator and byte padding. Overlap and 8,192-byte window
wrap use initialized data only. Cipher masks continue across blocks, including
the odd final processed block exercised by the 6,191-byte demo payload.

No per-frame tuples are allocated at decode time. Some bounded byte slices
are duplicated; the format caps are not an exact process-memory/RSS cap.
Fresh encoding is literal-only, with length `ceil((9 * decodedBytes + 2) / 8)`;
an edited but otherwise decodable highly compressed replay can exceed the
compressed cap and fail explicitly. The APIs are synchronous; they do not
accept AbortSignal. Worker scheduling/termination is the host's responsibility.

## Focused verification

Date: 2026-10-29, Node v24.20.0 and Python 3.14.7.

- RED: `node --test tests/replay-codec.test.mjs` first failed with
  `ERR_MODULE_NOT_FOUND` before the browser module was written.
- GREEN: the same command passed **22/22 tests**, zero skips/failures.
  That comprises 19 top-level cases and the three actual-demo subtests.
- No global test sweep was run for this task: other workers may have RED files.
- The Python bridge is read-only, runs `python -B -c` with
  `PYTHONDONTWRITEBYTECODE=1`, and creates no fixtures or bytecode on disk.
  Python's existing implementation/tests were not edited.
- Thirteen Python comparisons cover the maximum uint64 timestamp, synthetic
  native data in unchanged/forced/edited modes, and all three real demos in
  those same three modes. Encoded **whole files** match Python byte for byte,
  not merely after decoding. Parsed timestamp/name/score/route/rank,
  stage IDs/seeds/counts/frame access and opaque data also match.
- Edited comparisons change stored score, append a native input record and
  FPS byte, alter an unknown stage-header byte and extend USER content. The
  unknown words and updated sizes remain identical between implementations.
- Other focused cases cover input/subarray ownership, Buffer pooling,
  detached/shared/resizable rejection, strict indices/options, unknown IDs,
  NaN bit preservation, every truncated synthetic-file prefix, three-stage
  boundaries, aggregate budgets, USER size/tag separation, LZSS overlap/wrap,
  invalid references/tokens/terminators and resource caps.

Private real data is resolved through
`local/catalog.json -> variants.ja.manifest` and the manifest parent's
`raw/demo0.rpy`, `raw/demo1.rpy`, `raw/demo2.rpy`. Original file SHA-256 values
are pinned in the test and `replay.md`; no original bytes are added to source.

| Demo | Route / rank | Stage | Frames | Compressed / decoded bytes |
| --- | --- | --- | ---: | ---: |
| 0 | A1 / Normal | A1-1 | 6,686 | 6,228 / 40,595 |
| 1 | B1 / Normal | B1-1 | 6,722 | 6,191 / 40,813 |
| 2 | C1 / Hard | C1-1 | 6,094 | 5,702 / 37,024 |

All three unchanged native files reproduce exactly. Forced encoding produces
the same 45,906/46,151/41,889-byte files as Python. All edited full-file
comparisons pass too. Final review was self-review, not an independent review.

## Remaining boundaries

- **Native playback/loading compatibility is unverified.** Codec agreement
  does not validate simulation, RNG, route progression or replay sync.
- This is host resource processing only, not a JavaScript replacement core.
  No ECLVM work or host/server/build integration is included.
- Only version-2, bounded 1..3-stage native containers, the documented USER
  shape and strict initialized-history/terminator policy are supported.
  Synthetic three-stage/C2 cases are not native full-route playback evidence.
- Raw fields are required for edits. Display text is best-effort, timestamps
  require bigint-aware diagnostics and compressed output can grow substantially.
- The main worker owns serving/routing this ES module and integrating the
  exact accessor/structure above. This worker stops after this tested handoff.
