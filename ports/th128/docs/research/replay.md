# Bounded native TH128 replay codec

## Implementation plan (task C)

Goal: inspect native TH128 replay metadata and stage/input records safely,
with a preservation-oriented encoder; do not claim native playback fidelity.
Spec: `docs/2026-10-28-full-port-plan.md`, task C.

Architecture: a standalone Python codec using the existing `FormatError`,
`require`, `span` and integer readers. No additional runtime dependencies.
Verification date: 2026-10-29 (Asia/Shanghai).

Files: `tools/th128_assets/replay.py` implements the codec;
`tests/test_replay.py` supplies independent synthetic and private-demo tests;
this document records provenance, evidence and limits. No pipeline/UI edits,
Git operations, original executable runs or E: drive writes are permitted.

- [x] Verify the five existing source references against upstream commit
  `595d1451edbf4a55e121d45174b877f6eb0446d5` (kikones34/th128-replay-fixer).
- [x] Write and observe failing cipher, LZSS, malformed-container and preservation tests.
- [x] Implement bounded two-pass cipher, MSB-first LZSS and native record parsing.
- [x] Implement byte-exact unchanged encoding and raw-component edited encoding.
- [x] Verify every actual demo, forced recompression and the wider Python suite.

Design: raw headers, metadata/configuration, frame bytes, FPS/unknown suffixes,
USER data and container gaps/tails remain authoritative bytes. Parsed values
are read-only views, not an instruction to normalize native unknown values.
Input tuples are lazily decoded to avoid per-frame object amplification.
Hard budgets are enforced before allocation/expansion; caller budgets may
only tighten those limits. LZSS cannot read uninitialized history, truncate a
token or expand beyond its declaration. A literal-only encoder avoids an
unbounded match search; original ciphertext is reused only after validation.

Review focus: partial cipher blocks and odd ignored tails; compressed-token
truncation; ring initialization and overlap; structural size/frame arithmetic;
USER length overflow and independent tag preservation; unknown bytes and float bit patterns.

## Attribution

The cipher and wire-format interpretation are adapted from kikones34's MIT
licensed TH128 replay fixer at the pinned commit above. The complete upstream
license is preserved in `docs/research/replay-license.txt`. This adaptation
replaces unchecked pointer arithmetic and decompression with bounded Python
readers. Original game/demo bytes are private and are not redistributed here.

Pinned primary sources:

```text
https://github.com/kikones34/th128-replay-fixer/tree/595d1451edbf4a55e121d45174b877f6eb0446d5
https://raw.githubusercontent.com/kikones34/th128-replay-fixer/595d1451edbf4a55e121d45174b877f6eb0446d5/LICENSE
```

All five prepared `work/upstream/replay_src_*` files were compared byte for
byte with `src/` at that exact commit, not with a moving branch:

| Source | SHA-256 |
| --- | --- |
| `types.h` | `56f733ef034ac992f352269cda470a3caa484be59f6e1a39bb814bf35a41391d` |
| `th128_core.cpp` | `71cf0b7524c4d631973f7ef940991679646d85b4944f9bd52fb15fd5047f98fe` |
| `encryption.cpp` | `d37034feb5b0bd1ffe7e1a0d22793b87a3932f90326a8cd46c2144ba0feee729` |
| `compression.cpp` | `b29de93c71fae586d5bd6c8464c73f36a5a9e150c1ea31533bb9cec34a8078d0` |
| `th128_parse.cpp` | `7802039cfc5419ec2dbc824cb4fbfb231cf291354692d1f59c64e2818b72b2e6` |

## Public interface

```python
decode_replay(data: bytes, *, max_compressed_size=8388608,
              max_uncompressed_size=33554432, max_frames=1000000) -> dict
encode_replay(decoded: dict, *, max_compressed_size=8388608,
              max_uncompressed_size=33554432, max_frames=1000000) -> bytes
```

Malformed, unsupported or over-budget containers raise the existing
`th128_assets.formats.FormatError`. Callers can lower, not raise, hard limits.
`bytes` and `bytearray` inputs are accepted. Returned dictionaries contain
binary data and lazy sequences; they are not directly JSON-serializable.

The returned structure contains:

- `header`: raw 36-byte header and replay/game version, USER offset and lengths.
- `metadata`: raw 112 bytes, name (first eight bytes, CP932), full 12-byte name,
  Unix timestamp, stored/derived score, 60-byte config, slowdown, stage count,
  route, rank and last-stage identifiers. Unknown labels produce `None`.
- `stages`: native stage identifier/seed, frame count, initial score/power,
  position, continues, graze, motivation and freeze fields; complete
  `rawHeader`, `inputRaw` and opaque `fpsRaw` bytes are retained.
- Each stage's `inputs` is a `ReplayInputs` sequence of little-endian uint16
  `(holding, pressed, released)` tuples. Indexing is lazy, slicing returns a
  list and iteration does not preallocate frame objects. All 16 bits,
  including unknown bits and the `holding == 0xffff` marker, are preserved.
- `userBlocks`: two native USER headers/data blocks when present, including
  `headerRaw`, independent `kind`, raw `data` and a best-effort CP932 `text` view.
- `encodedPayload`, `rawPayload`, `payloadTailRaw`, `dataGapRaw` and
  `fileTailRaw`: original ciphertext, full decompressed data, unparsed data
  after stages, pre-USER gap and post-USER tail respectively. With USER
  offset zero, all bytes after ciphertext remain `fileTailRaw`.

Stored score is exposed as `scoreRaw`; `score` multiplies it by ten following
the reference. Shot power is stored plus one; positions divide by 128;
max-PIV, motivation and perfect-freeze divide by 100. These are reference
interpretations, not independently verified native gameplay semantics.
Unknown route/rank/stage IDs and NaN float bit patterns are retained rather
than rejected or normalized. No timestamp-to-local-time conversion occurs.

### Encoding and editing

**Raw components are authoritative. Changing a derived scalar view does not
edit a replay.** To edit a score, for example, replace `metadata['raw']` with
bytes modified by `struct.pack_into('<I', buffer, 20, stored_score)`.

Encoder inputs are `header['raw']`, `metadata['raw']`, each stage's
`rawHeader`/`inputRaw`/`fpsRaw`, `payloadTailRaw`, `dataGapRaw`,
`userBlocks[*]['headerRaw']`/`['data']`, and `fileTailRaw`.
`rawPayload`, `inputs`, `userBlocks[*]['raw']`, text and parsed scalar values
are views, not edit inputs. The encoder recalculates only:

- Replay USER offset, compressed size and uncompressed size.
- Metadata stage count and stage frame counts/payload sizes.
- USER block sizes, retaining each raw header's independent tag.

Original ciphertext is reused only if its bounded decrypted/decompressed
bytes equal the assembled payload. This gives byte-exact unchanged
roundtrips, without blindly trusting a caller-supplied cache. Removing or
invalidating `encodedPayload` forces literal-only LZSS encoding followed by
the inverse cipher passes. Unknown bytes and float encodings are not rebuilt
from Python values. USER summary text is not regenerated on edits: callers
must deliberately update it if a matching human-readable summary is desired.

## Wire format and safety

The 0x24-byte little-endian header has magic `128r` at 0x00, replay version 2
at 0x04, USER offset at 0x0c, game version at 0x10, compressed size at 0x1c,
and decompressed size at 0x20. All other bytes are retained. The three demos
use game-version zero and USER text saying `Version debug`; the codec does
not incorrectly require a retail 0x100 game-version value.

The 0x70-byte metadata prefix has name at 0x00, uint64 timestamp at 0x0c,
stored score at 0x14, config at 0x18, float slowdown at 0x54, stage count at
0x58, route at 0x5c, rank at 0x64 and last-stage ID at 0x68. Stage headers
are 0x90 bytes, followed by `size` bytes of body. Their uint16 stage and seed
are at 0x00/0x02; uint32 frame count and body size at 0x04/0x08; initial
score, power and max-PIV at 0x0c/0x10/0x14; signed positions at 0x18/0x1c;
continues/graze at 0x20/0x28; motivation/perfect-freeze/float area at
0x80/0x84/0x88. Bytes not interpreted remain in `rawHeader`.

Decryption applies `(block, mask, increment) = (0x800, 0x5e, 0xe7)` then
`(0x80, 0x7d, 0x36)`; encryption reverses the passes. A short tail below a
quarter-block is ignored, and the original length's odd bit is subtracted
as in the pinned source. Mask state continues between blocks. In particular,
the odd 6,191-byte demo payload exercises an odd final processed block.
For tiny inputs whose reference loop count would become negative, the safe
codec processes zero bytes rather than reproducing that C++ underflow.

LZSS is MSB-first: a `1` plus eight bits is a literal; a `0` plus 13-bit
nonzero absolute ring index and four-bit length copies 3..18 bytes from an
8,192-byte ring. The encoded index is one greater than the source ring slot;
zero is the terminator, so slot 8191 cannot start a reference. Overlapping
copies and ring wrap are supported. References to not-yet-initialized history,
partial tokens, early terminators and copies past the declared size fail.
After exactly the declared output, only a zero terminator (possibly truncated
to remaining bits) and zero byte padding are allowed: 1..21 zero bits total.
Extra nonzero data or excessive padding fails instead of being silently ignored.

| Hard bound | Limit |
| --- | ---: |
| Entire native file | 16 MiB |
| Compressed/encrypted payload | 8 MiB |
| Decompressed payload | 32 MiB |
| Combined two USER blocks, including headers | 4 MiB |
| Stages | 1..3 |
| Aggregate input frames across all stages | 1,000,000 |

Lengths are checked before copying/expansion; each binary span and USER offset
must stay within the container, stage input bytes cannot exceed their body,
and a USER offset cannot overlap ciphertext. Python integer arithmetic avoids
wraparound in uint32/uint64 length attacks. Decompression is bounded by both
the declared output and the theoretical maximum eight output bytes per
compressed byte. Parsing allocates no per-frame dictionaries. Some bounded
raw byte slices are duplicated; these limits are not an exact process-RSS cap.

The new-data encoder does no history search: its compressed length is
`ceil((9 * payload_bytes + 2) / 8)`. A highly compressible payload accepted by
the decoder may exceed the 8 MiB compressed cap when edited/re-encoded, in
which case encoding explicitly fails. It does not relax caps to force output.

### Corrections to the published reference

Actual bytes take precedence over misleading upstream comments:

1. A USER header is `magic[4], uint32 size_including_header, uint32 kind`,
   **not a uint64 size**. All three demos' first-kind values are 0 and second-kind
   values are 1; the second header is `55 53 45 52 20 00 00 00 01 00 00 00`.
   Treating this as a uint64 size incorrectly requests 4,294,967,328 bytes.
   The implementation preserves any tag value without treating it as a length.
2. The upstream comment that trailing FPS data looks like
   `ceil(num_frames / 2)` does not fit any actual demo: the observed lengths
   are 223/225/204, not 3,343/3,361/3,047. No cadence or sample interpretation
   is enforced; all remaining stage-body bytes stay opaque.
3. The source's claim that max-PIV is always 1,000,000 is not a validity
   constraint: these three stage headers store 100,000. No such invariant
   is enforced here.

## Actual-demo evidence

The private source is resolved on every test through
`local/catalog.json -> variants.ja.manifest` and its parent `raw/demo{0,1,2}.rpy`.
No original game executable or installer was launched, and no E: drive file
was modified. Each demo has one stage, name `ZUN     `, no gap/payload/file
tail, and USER blocks of 168 and 32 bytes, tagged 0 and 1 respectively.

| File | Route / difficulty | Stage | Frames | Compressed / decoded bytes | Stored / derived score | Seed | Opaque FPS bytes |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: |
| `demo0.rpy` | A1 / Normal | A1-1 (1) | 6,686 | 6,228 / 40,595 | 114,806 / 1,148,060 | 34,772 | 223 |
| `demo1.rpy` | B1 / Normal | B1-1 (6) | 6,722 | 6,191 / 40,813 | 86,121 / 861,210 | 5,253 | 225 |
| `demo2.rpy` | C1 / Hard | C1-1 (11) | 6,094 | 5,702 / 37,024 | 87,724 / 877,240 | 42,295 | 204 |

Native UTC timestamps are 2010-08-02 22:21:50, 22:24:05 and 22:28:07
(Unix 1280787710, 1280787845 and 1280788087). USER text shows 2010-08-03
07:21/07:24/07:28, nine hours later; the codec does not rewrite either form.
Reference slowdown floats are approximately 0.045311, 0.056145 and 0.0.

| File | Container SHA-256 |
| --- | --- |
| `demo0.rpy` (6,464 bytes) | `33024e1b08c2a0f27c0b84f4a49d609e4b931d15c0359f88406cd5b257973c9e` |
| `demo1.rpy` (6,427 bytes) | `26adf86f10326d3672bfabf4a034f1a2a1fa606416cd3f29529c7550c3a3bcc6` |
| `demo2.rpy` (5,938 bytes) | `53d688e575961f793fba91621045d4dd520719b05fe0feadfb4905c4822866b3` |

All three unchanged replays reproduce byte for byte. Forced literal
re-encoding produces files of 45,906/46,151/41,889 bytes; decoding those
recovers identical full payload, stage headers, frame bytes and opaque bytes.
Private machine-readable evidence is in `local/research/replay-validation.json`.

## Verification record

- RED: `python -B -m unittest discover -s tests -p test_replay.py -v` failed
  because the requested replay module did not exist, before implementation.
- During implementation the real demos exposed the USER uint64 reference
  error. A focused `-k user_length` regression was observed failing with
  `FormatError: invalid USER block size`, then passed after separating size/tag.
- GREEN: `python -B -m unittest discover -s tests -p test_replay.py -v`:
  **31 tests passed**, including every actual demo (no private-data skips).
- Wider shared-workspace snapshot:
  `python -B -m unittest discover -s tests -p 'test_*.py'`:
  **83 tests, 61 passed, 22 skipped**, zero failures/errors. The unrelated
  skips require optional Unicorn and/or an explicit original-EXE fixture.
  `PYTHONDONTWRITEBYTECODE=1` was set for these runs.
- Deterministic mutation/invalid-byte sweep, seed 296: 1,536 one-to-three-bit
  mutations cycling through the synthetic fixture and three actual demos,
  plus 512 random 0..1023-byte inputs. Of 2,048 cases, 1,250 raised
  `FormatError`; 798 remained structurally valid and all 798 roundtripped
  byte-exactly. No unexpected exception occurred. Valid mutations are not
  evidence of authentication or semantic validity; this format has no
  verified integrity check in this codec.
- Final review: self-review (no subagent tool), covering arithmetic, all raw
  field ownership, cipher boundaries, ring wrap/overlap and preservation.
  This is not an independent reviewer sign-off.

## Remaining limitations

- **Native-game playback of rewritten files is unverified.** Byte-exact
  identity for unchanged files and codec roundtrips do not validate engine
  input consumption, RNG behavior, simulation, display or replay sync.
- This is a standalone codec only: no pipeline, browser/UI, engine or replay
  playback integration is included. No TH12 format substitution is used.
- Only replay version 2, 1..3 stages, absent USER or exactly two USER blocks,
  initialized LZSS history and the documented strict terminator shape are
  supported. Different native variants may be explicitly rejected.
- Demo evidence covers A1/B1/C1 single-stage debug captures, not every route,
  retail replay version, full three-stage run or Extra. Synthetic tests cover
  three-stage parsing, C2 labels and unknown IDs but do not prove native play.
- Config fields, unknown stage words, FPS bytes, USER tags beyond the observed
  values and most key-bit meanings are not assigned new gameplay semantics.
- Literal-only re-encoding is larger than native compression; no compression
  ratio, native compressor byte identity or successful game loading is claimed.
