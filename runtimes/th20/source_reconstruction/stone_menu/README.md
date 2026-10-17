# StoneMenuInf source reconstruction

This module restores the stone selection menu as real C++ and defines the unique `0x005c6120` owner. It does not execute or embed original game instructions. The static library compiles; complete game linking and replay equivalence still depend on other modules.

`StoneMenuInf` is exactly `0x21104` bytes: callback owner at zero, two `0x4c` cursors at `0x28/0x74`, nine animation handles at `0xc4`, 88 names at `0xf4`, five descriptions per name at `0x58f4`, frame state at `0x210f4`, and context at `0x21100`. The constructor preserves the original three padding bytes at `0x210f9`. Text retains the original CP932 bytes.

| Original VA | C++ implementation | Recovered behavior |
| --- | --- | --- |
| 004aef00,004aee90,004afa70 | `menu::Cursor` construction/destruction | Cursor fields, empty PMR vector and two owned history containers |
| 004bed50,004bfa00 | `menu::select`, `menu::move` | Selection, exclusions, minimum, clamp/wrap |
| 00515b90,00516170 | `StoneMenuInf` constructor/destructor | Complete owner state, callbacks, animation teardown and file unload |
| 00519960 | `initialize`, `parse_text` | Blocking ANM load, update/draw registration, real text labels and parser |
| 0051cc20,0051b6d0 | `create_controller`, `release` | Real allocation, initialization failure and unique global ownership |
| 0051c7a0 | `StoneMenuInf::select_context` | Actual view/context binding |
| 00519840,005198d0 | `hide`, `clear` | Original interruption/deletion order |
| 005167b0 | `update` | Complete states 0–8, directional/repeated input, unlock and stock checks, profile selection/cancellation, animation and meter updates |
| 005189c0 | `draw` | Categories, extra choices, counts, descriptions, colors and coordinate transforms |
| 0051a0b0 | `open` | All three open modes, initial stock selections, animation setup and frame states |
| 005163c0,0051c6f0,0051c8b0 | update helpers | Inventory reset and committed/pending selection transfer |
| 00438f10 | `sprite::set_animation_texture_rectangle` | Floating crop geometry and all UV coordinates |
| 0044c670,0044c6b0 | `sprite::find_animation_child` | Handle resolution and original recursive matching/count semantics |
| 0044efc0 | `sprite::execute_animation_interrupt` | Intrusive iteration and immediate VM dispatch order |

`cpu_validation.json` records **110,128 passed, 0 failed**, comparing real original function bodies against compiled Win32 C++. It includes 4,096 complete update/open cases, 3,200 draw cases and the actual stock text parser. Assertions compare the full owner, Player state, animation objects, text properties, inventory and ordered cross-module call arguments. Source hashes and exact scope are recorded by `record_evidence.py`.

The oracle maps original instructions only inside its isolated test executable, never calls the original entry point, and replaces external resource/ANM/audio/metadata interfaces with deterministic recorded boundaries. Production calls recovered implementations. Overlay selection refresh `534080` remains explicitly unresolved. Special-color selection `513f70` and the `5c6118` owner getter `513dd0` now bind the actual `special_state` module; its own report covers original CPU comparisons. No default result stands in for unresolved code.

Cursor history mutation, invalid or corrupt inputs, allocation failure, whole-game rendering and deterministic replay are not established by this module's comparisons. Recovery of the overlay is the next integration work.
