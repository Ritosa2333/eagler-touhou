# TH19 1.10c gameplay verification

Original EXE SHA256: `8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9`.
Runtime WASM SHA256: `df7affaf0e1c9f1c599ba9eeb8daf45f38d9ac4b4816091dd49567f5030aab54`.

| Behavior | Original evidence | Implementation / verification |
| --- | --- | --- |
| VS EX/Boss ranks | 0x517f86–0x518062; table 0x5aa5a0 | Stored ranks 0,0,2,4; displayed 1,1,3,5. Story initialization remains separate. All four difficulties tested. |
| Automatic combo attacks | Death rewards 0x51d4e0, update 0x50df40 | Pre-death combo >4: eligible spirit deaths add 15 to each available meter; EX threshold100, Boss200; cooldown60/240 frames, decay every4 frames. EX recovery accumulator follows native path. No energy spend, barrier recovery or rank increase. |
| Combo initial timer | 0x50e251 | Native +0x1e4 initialized to0. |
| Missing-barrier warning | 0x4e9532; aura.anm script3 label2/3 | C3 energy threshold; native two-color animation, 16-frame cycle. |
| Last-heart warning | 0x5141c0; front.anm script16 label7 | Pink/white, 20-frame cycle. |
| Enoko traps | pl15.sht init21/update13/hit0; init 0x54aca0; hit 0x54fba0 | Init marks persistent collision; real C1 and C2 shots damage durable targets without disappearing. |
| Enhanced animal HP | Original default.ecl GhostPureHP routines | Actual VM execution: ordinary/enhanced wolf1000/1200, otter700/900, eagle800/1100 on first arrival. Existing values retained. |
| Quit | 0x55a110 selection branch, sound9 | Immediate launcher exit is the requested UI behavior. |

Browser regression: `ports/th19/tests/gameplay-parity-browser-smoke.mjs`; includes animation checkpoint restore. `versus-settings-report.json` covers four difficulties, synchronized life settings, X/C input and deterministic rollback. These checks use the shared normal/multiplayer core. Original executable and extracted binary resources are not included in this archive.
