# Spell-practice cards and drawing

`practice_cards.cpp` implements original `51feb0` (refresh, returns 0), `51fd80` (selection, returns 0), and `52cb30` (fallback-profile group availability). `practice_draw.cpp` implements `528a50` (score rows, returns 1). They compile as ordinary C++ and call the existing actual Text/ANM source.

The shared CPU oracle adds **65,536 checks with no differences**: 4,096 full score draws plus group queries, 2,048 selections, and 2,048 refreshes. It compares whole TitleInf/Renderer/current and fallback Profile storage, return values, all 64 fixture ANMs/free lists/generation, file counters and six complete cached TextJob objects. Inputs include every valid card/group, all phase/color/unlock/capture branches, 64-bit score extremes, signed counts, long and CP932 names, stale handles, and five window scales.

The original specimen is mapped only inside the test executable. Its instructions are not patched. Synthetic valid ANM label/stop programs and existing zero-frame text jobs provide bounded resources; file IO, fresh string GDI/rasterization, GPU display and complete gameplay are outside this group. Real production environment binding is supplied separately.

Original `52cb30` retains an unused this value and ends in `RET 8`; the decompiler marked it cdecl. The first test invocation used that misleading convention, caused stack drift, and was corrected in the fixture. `528a50` references `5755b0` for the locked score; `575564` is an interior substring of `57555c` and was removed from the data generator.

`draw_validation.json` records this group together with Music and stage selection, with exact shared-report and compiled-source hashes. No complete-game reconstruction claim follows from these checks.
