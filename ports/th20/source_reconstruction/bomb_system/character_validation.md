`character_cpu_cases.inc` and `character_start_cpu_cases.inc` add 97,024 original
CPU checks to `../sprite_renderer/pool_cpu_validation.json`. The executable runs
unmodified original instructions only as an isolated oracle. Production code is
compiled C++; the test patches no instructions.

- 256 complete-object construction cases each for Marisa4784e0 and Reimu479280.
- 2,048 Marisa478680 cases compare the whole Bomb, 0x2200-byte Player fixture,
  complete HitCtrlInf and Sprite pool/list state, and return value. Cases cover
  absent beams, times -1/0/1/2/3/299/300/301/302/500, integer-frame transitions,
  varied timer rates, angle steering, interrupts, and real allocation/activation
  of the three rectangular damage regions. Each case also compares the real
  478610 animation-position callback, including the complete Animation.
- 8,192 ReimuOrb479360 cases compare all 0xd8 bytes, complete EnemyController,
  enemy links and Sprite pool. Cases cover orbit, delayed launch, launch frame,
  target selection and steering, no-target slowing, timer behavior and Motion
  modes0..3. These paths use actual shared Motion and Enemy search C++.
- 512 ReimuOrb47a090 cases execute actual ANM selection, VM initialization,
  registration and circle-damage creation. Complete orb, damage and Sprite
  state plus the file's spawn counter are compared.
- 1,024 Reimu479860 cases compare all 0x14f8 Bomb bytes, Player, damage and
  Sprite state. Cases cover active/inactive orbs, the frame120 finish check,
  frame240 quiet retirement and damage-position following. The generated
  lifecycle clocks avoid the unresolved cancellation branches.
- 256 complete start cases each for Marisa478c00 and Reimu479fd0 compare Bomb,
  Player, secondary-owner notification, Enemy state, the SoundInf prefix,
  Sprite state and spawn counts. Marisa creates the actual ScreenInf shake
  object and scheduler node; their complete records are compared after
  canonicalizing code/object/allocation pointers, then both objects are retired
  through their respective implementations. The sound and Screen code is real
  recovered production code, with no sound/render substitution.

ANM fixtures provide valid 70-entry template/script tables with an immediate
stop instruction. Thus these checks exercise the actual VM/registration path
but do not claim equivalence for complete player animation assets. Marisa's
57-child cancellation loop is excluded; Reimu's frame0/40 spawn-then-cancel
branches, nonquiet retirement and total-damage>=300 explosion are excluded.
The four unresolved rectangle/circle cancellation calls have test-only throwing
definitions, so accidentally reaching an excluded branch fails the run. Their
production declarations remain unresolved. GPU pixel output, continuous
multi-frame gameplay and Bullet/Laser controller behavior are not established
by these state comparisons.

The report binds hashes for the tested source and fixtures. Original callbacks,
C++ vptrs and separately allocated Screen/scheduler objects necessarily have
different addresses; only their identified pointer fields are canonicalized.
The original478610 helper is thiscall with an unused receiver and one stack
argument (RET4); its original478380 wrapper and the public recovered callback
are cdecl. The test uses that verified calling convention.
