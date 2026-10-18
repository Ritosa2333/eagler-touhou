`WeaponStoneInf` is the actual 0x84-byte owner in `game_session::Context::overlay_owner` (+0x2c), created by 0x534dd0. Its four strategies, phase effects and mesh are reconstructed C++. Production never executes or embeds the original program. The Weapon vtable has thirty slots; slot zero is reset, and destruction is nonvirtual.

All eighteen factory positions now have their original concrete behavior: both characters' indices 0 through 7, plus index 8's original alias of index 0. The sixteen concrete classes are FocusBoost, FlagBoost, Orbit, Bar, Shield, Yellow, Ring and Cloud for each character. `evidence/weapon_vtables.json` records every original constructor/vtable/method. `module_status.json` maps source files to original entry addresses.

The common code restores shot-pattern arithmetic, SHT coordinate pointers, input/phase guards and duration. Owner code supplies lifecycle, four selection paths, scheduler registration, reset, firing dispatch and release. Frame code restores 532b20 and 534d00. Production uses actual Sprite, Player firing, Item, DamageRegion, Bullet, Laser, audio, shared RNG and Context storage. Yellow has its own two 0x24-byte BSS states, distinct from Shield's BSS. The new 47d240 filtered-circle cancellation retains the real predicate callback and removed count. 505e40 in `player_entity/firing_at_position.cpp` shares the real Shot creation chain.

The separate x86 oracle records **870,400 passing checks, zero failures**:

- 188,416 common weapon checks and 32,768 passive/Orbit/query checks.
- 65,536 owner phase/frame checks with explicit virtual and rendering boundaries.
- 61,440 Bar; 69,632 Ring; 102,400 Shield; 69,632 Cloud checks.
- 258,048 Yellow constructor/reset, shared BSS, option motion/RNG, shooting, activation, passive, phase and callback checks.
- 10,240 complete filtered-circle comparisons, including real original list traversal, predicate results, entity mutation and count; final Bullet cancellation is a shared observed boundary.
- 12,288 complete 505e40 comparisons of sentinel/period logic, shot count and every packed pattern, frame, position and Option argument; Shot initialization is a shared observed boundary.

Full field bytes, return values and call traces are compared. Original and reconstructed vptr/callback addresses are normalized explicitly. RNG callbacks in the Yellow fixture use the independently validated real shared RNG implementation on both sides. Tests found and corrected a Yellow difference: the doubled angle must wrap before sine, even though the unwrapped formula is mathematically periodic.

These are isolated original-machine-code tests; the original EXE entry point is never called. Rendering/effect allocation, audio, item and damage allocation remain observed boundaries in these tests. Owner lifecycle/selection/factory integration and phase mesh deformation compile but are not claimed fully original-CPU validated. Allocation failures, invalid indices and concurrency are outside this suite. Full-game equivalence is not established.

Only `replay_inherited_stone(slot)` remains an unrecovered external dependency of this module: it must read the real Replay owner 5c60fc+1c and record+ec, without duplicate storage. Startup 534dd0/534110, Player power, StoneMenu refresh, Item phase activation and Gameplay selection have real adapters.

Build the library with CMake VS2019 Win32 and C++20. Build `oracle/` separately and run `th20_overlay_cpu_compare.exe ORIGINAL.exe cpu_validation.json` for differential validation.
