# First real ECL emission: production integration

This work continues the approved full-port plan. It is **not a playable stage**
and does not close the overall port. It replaces neither missing game rules
nor unknown callbacks with a demo pattern.

## Executed production chain

The new explicit first-emission mode executes:

1. Real `st_a1_1.ecl` + `default.ecl` + `st_a1_1b.ecl`, Normal, original demo0
   seed 34772. The ECL VM, negative-reference providers and RNG are shared with
   the existing world kernel.
2. Opt-in local enemy passes: constructor update before insertion, saved-next
   traversal and guard `0x80000`. Proven bounded interpolation, absolute/relative
   XY integration and combined-position quantization run **before ECL**.
3. Actual opcode501 argument resolution and the supported default emission
   position policy. The accepted world opcodes do not configure alternate
   offsets/exclusion radii; unsupported later operations still stop.
4. A real bounded `bullet::Manager` pool, capacity **2000**, cyclic search from
   the native cursor, one constructor callback per successfully allocated
   bullet, partial-volley saturation and retained sound requests.
5. Original aim/geometry and type8 birthtail rules, then a mandatory actual
   `BulletVisuals` constructor using original `bullet.anm`, script43, type8
   color10 remapping, IRQ8 and exactly two synchronous animation ticks.
6. Read-only physical/animation snapshots and WebGL drawing of the resulting
   original sprite. There is no fallback renderer or arbitrary bullet texture.

The new mode is available under **首发子弹实装检查（不是可玩关卡）** in
`site/play.html`. The older **首关世界调度检查** intentionally keeps its
configuration-only baseline and still stops at501 when no bullet host is
installed. These are two explicit diagnostic modes, not conflicting results.

## Concrete observed state

With local enemy passes enabled, the first allocated bullet is:

- Type8, color10, original script43, pool slot0 and identity1.
- Motion state2, consumed first-extension index1, birth IRQ8.
- Actual remapped sprite328, script integer0 still0, two ANM ticks executed.
- Physical Z **0.1f**, from native constant `0x4a6aa0`, not a guessed zero.

The local kernel reaches its first post-emission boundary at tick116, with the
first enemy at approximately **(-133.09,7.44)**. These positions are no longer
the unchanged birth point. The player aim target is an **explicit diagnostic
input (0,400)**, not an already integrated player manager. Float snapshots in
JSON are formatted for inspection; bit-exact component tests use binary words.

The next bullet-update phase remains an explicit stop until its collision,
ETEX, age/timer and animation scheduling is supported. Creating a real bullet
and advancing the ECL instruction is not evidence for a complete bullet life.

## Original-byte evidence for pool and draw origin

`tools/research/bullet_pool_probe.py` runs five sparse native-constructor
selection cases at `0x4091f0`, stopping at `0x4092dd`: immediate free slot,
one/two occupied slots, sentinel wrap, and wrap followed by an occupied slot.
Only declared sparse fields are mapped, not an unbounded original pool.
Undeclared reads, unknown calls and budgets stop; no allocator return is made up.

Additional tests execute the real501 handler through its position stores,
stopping before `0x40a8af -> 0x40a8f0`. The override threshold is the original
double representation of **0.9f**, not an invented sentinel. Default reset
offsets select combined enemy position; override mode uses the authored base.

The physical-to-display prefix at **`0x408f52..0x408f88`** proves:

`screenX = f32(physicalX + 128.0 + 192.0)`

`screenY = f32(physicalY + 16.0)`

Z is copied unchanged. Thus TH128's bullet origin is **(320,16)**; importing
TH12's X224 would shift the new rendering by96 pixels. The probe maps original
bytes through the next conditional branch so Unicorn can translate the block,
but stops before that branch and records only the declared prefix execution.
The runtime/WASM regression asserts the320 offset. No draw or GPU behavior is
inferred from these position stores alone.

The288-byte type8 profile is copied read-only from the hash-pinned original
Japanese EXE at **`0x4b2dd0`** to `local/runtime/bullet-profile-8.bin`, with a
separate provenance file. Only that exact binary path is served; neighboring
JSON, research artifacts, original EXEs/configuration and traversal are denied.
It contains original table data, not replacement TH12 formations.

## Boundaries and host policy

Local pass ordering is constrained by `enemy-scheduling.md`. Calling one such
pass per diagnostic kernel tick and starting the main constructor at explicit
enable time are **host bootstrap choices**, not a completed native global
callback/frame trace. Full enemy ANM ownership, pre-ECL bounds/visibility,
damage/removal, other motion modes and all world effects remain incomplete.
Unsupported flags/termination/removal paths reject rather than silently delete.

The bullet manager supports only the evidenced type8/aim0..1/extension0 or2,
zero exclusion distance and its loaded profile. Saturated pools stop further
births without substituting a new pattern. An empty volley skips count loops
but still retains its authored sound request. Sounds are requests, not a claim
that the complete native sound manager has been executed.

Missing animation constructors, malformed profiles, missing sprites, unknown
ANM instructions/RNG sources and invalid spans fail. World reentrant reset is
rejected before clearing entities/VMs. Once local execution begins, later
configuration is rejected **before** changing target, manager callback or visual
ownership; the regression advances afterward to prove rejected targets do not
affect later bullets.

Browser requests retain version/action ownership. Delayed emission cannot
override a newer reset/start; stale audio-start success or failure cannot
activate or stop a newer action. Both failures were reproduced before fixes.
The browser smoke now checks original ANM state, a nonempty rendered canvas,
zero runtime errors and390px overflow, not just saved diagnostic text.

All native-byte claims are **Unicorn emulation of read-only original bytes**,
not a Windows original-game process, native-CPU trace or full-game visual oracle.
