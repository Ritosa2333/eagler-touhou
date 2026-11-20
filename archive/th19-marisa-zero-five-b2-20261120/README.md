# Second fresh five-seed zero-error batch

All five Marisa CPU-vs-CPU seeds passed36000 cumulative frames across rounds. Total180000 frames. Coordinate tolerance0 and maximum coordinate difference0. Primary RNG,CPU controls,player positions and bullet states match. Pure visual animation remains outside gameplay scope.

Final WASM:504a685602c8a8edd571771d5fae775dffda9c7db6ec40aa85ae754468b0693f.

[Aggregate](complete.json),[per-seed results](final-results.json),[identities and coverage](final-status.json),[zero-tolerance validations](validation/).

Initial run:4/5 passed. Seed1689226635 /1374292823 first differed in CPU controls at3195. A converted eagle at(-3.71,467.77) was retired one visit early. Native retained it as an approaching CPU obstacle,so the left CPU moved right. Web had removed it and moved left. Primary RNG and bullet states remained aligned,but the CPU/player difference failed validation.

The existing preceding-animation CPU-bound cache covered converted wolves and otters. Adding converted eagles to that same cache preserves the native lifetime through CPU sampling. The ordinary field-retirement pass keeps its existing bounds. The entire five-seed batch was rerun on the final build.

Frame3195 paired comparison: [before](screenshots/seed1689226635-before/frame-3195.png),[after](screenshots/seed1689226635-after/frame-3195.png). Web is left,native right. Cumulative gameplay mismatch count changes from1 to0.

Earlier batch results belong to their recorded builds and are not included as final-build coverage here.
