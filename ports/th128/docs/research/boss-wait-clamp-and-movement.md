# Boss wait, area clamp, base drops and random movement

Main-thread-only continuation after the geometry526 checkpoint. No agents,
native Windows game launches or modifications to the original installation.

##420 is a held ECL command

Original41b3dd checks exactly EnemyManager+5c/+60/+64. A nonzero word returns-1;
the original core46fbe0 branches before bothPC advancement and the context-clock
increment. Other contexts still run. An all-zero set returns0 and advances the
next authored command during the same pass. Slots3..7 do not gate420.

The new explicit HostCall hold result preservesPC/time for that one context,
executes it once per tick and does not block enemy movement, geometry or ANM.
Malformed, rejected and reentrant operations remain faults, not successful waits.
The query uses the existing owning412 boss table, never an invented empty table.
Ten original handler prefixes and six independent original core tails are
recorded; scalar inputs to the latter are declared, not assigned host returns
misrepresented as integrated call execution.

##404 is center/size after combined quantization

Original419f25 stores four floats and sets0x40000. Complete416030 first combines
absolute+relative, computes pre-clamp velocity and quantizes the combined position.
It then clamps combinedXY to center±size/2 and reconstructs absoluteXYZ by
subtracting relativeXYZ. The velocity remains the pre-clamp delta. There is no
extra post-clamp quantization. Inverted extents follow the original branch order,
not std::clamp's invalid min>max assumptions.

Three original registration cases and14 complete helper returns match portable
position/velocity/absolute words exactly in ordinary andUBSan builds. Local
construction guards remain intact. The unrelated0x8000000 transform still stops.

##410 is the base item drop

Original41a45e writes only EnemyData+3e54, not score or the entire drop list.
Eight original complete setter paths preserve all other ten drop words and both
spread fields. The live host updates the same existing DropState word0, preserving
additional drop configuration already written by407/408.

##312 is original randomized Boss movement

Original419f9e chooses direction before reading interpolation mode/speed/duration.
It depends on actual combinedXY,404 center/size, PlayerX and shared game RNG.
Outside the central quarter-area band it consumes a32-bit signed angle (two RNG
calls); inside it also consumes a16-bit modulo3 sample. PlayerX controls which
horizontal side is favored. Vertical bounds force the sign; original near-vertical
avoidance and the separately rounded constants are retained.

226 complete original312 setups cover seeds, PlayerX equality, both horizontal
branches and quarter-area thresholds. All direction words, final seeds and2/3
call counts match exactly in ordinary andUBSan WASM tests. No golden-specific
direction or fake RNG return is inserted into production.

The pair initializes(chosenAngle,authoredSpeed)→(chosenAngle,0) and timer(-1,0,0),
without instantly changing current polar values. Only proved positive-duration
mode4 is accepted. Twenty original mode4 helper returns and caller normalization
prefixes verify exact ease-out float stores, including initial angles beyond±pi.
The next real local pass normalizes twice and moves; draw/snapshot calls do not.
Existing305 mode0 behavior remains separate. Missing actual player/area context,
unproved modes and invalid values explicitly stop.

## Actual browser progression

Both1280/390 widths pass3270's420 with main held while mesh angles continue.
They pass404 and410 at3291, enter actualMBoss1, and execute312. On the next local
pass the real boss moves from(0,144) to(1.84000003,144.470001), speed1.90125 and
angle0.254468411 for this Normal/seed34772 input context.

The next stop is3292, entity75/MBoss1At2 offset296/op524. No420/404/410/312/526
command was skipped. Sixteen originaltext82 mesh rows still render. This is an
attack-preparation boundary, not a completed volley or Boss fight.

The earlier420/404/410-only checkpoint is preserved in
`../boss-wait-clamp-verification.json`:159 Node/C++/WASM,342 Python,3 visual-probe
tests and15 browser scripts passed on95-source build9f950016875ebb34e2f9ac92538f7698be516688feab08d05f08ae39c1e62393.
The subsequent312 generationd26a8d3d39549eaa0f8347b0841ad79ff32a0d1d432141f030019cb3ec994975
has97 matching source hashes and passes162 Node/C++/WASM,343 Python and3 visual
probe tests without skips;16 browser scripts also pass. Original/prepared assets
are unchanged. Additional no-exceptions checks pass3 cases with1 throwing-case
test explicitly skipped as inapplicable. See `../enemy312-verification.json` and
the latest `../main-thread-verification.json`, not an older checkpoint's numbers.

## Remaining

524 is not just a harmless emitter store: it sets emitter+24, used by the
original40959b radial birth-position calculation. The pending attack also needs
actual type4 profile/visuals and type2/type1 extension continuation. Do not register
the opcode while ignoring its consumer. Full414 reset/resolve/invoke, completeBoss,
Z/freezing/Perfect Freeze, lasers, backgrounds, routes/endings and replay
simulation remain unfinished.
