# Wolf arrival guard regression

Seed 424242 / 131071 first diverged at cumulative frame 13344. Native enemy 67209 died on that visit; Web enemy 8120 survived with 20 HP until the following visit. Each chain damage pass applied 30 HP on both sides.

The converted wolf entered its first arrival visit with a script already advanced by the CPU prepass. Web moved it on that visit and scheduled a separate held visit afterward. Native consumed the arrival guard on the conversion visit. Moving the hold to the conversion visit removes the extra protected damage tick.

Candidate WASM: `551fe50ad4e677958a6df5e9fc164885fb0fec818487d806d7e66af5a08cb777`.

The candidate passed all 36000 cumulative frames for 424242: primary RNG, controls, player positions, bullet counts, and 4946173 bullet positions matched exactly. Animation RNG is recorded separately and is outside the requested gameplay scope.

Before/after paired images: `screenshots/seed424242-before/frame-13344.png` and `screenshots/seed424242-after/frame-13344.png`. Web is left; native is right. The header records cumulative and round frames and the gameplay mismatch count.

Seed 1000003 / 999983 still diverges in CPU controls at cumulative frame 35570, in round two. A full 35-seed same-build regression is running; no aggregate completion is claimed.

CPU trace windows use the round clock while capture trace windows use cumulative frames. The capture tool now accepts a separate `--cpu-trace-window FIRST LAST` to inspect later rounds without changing the comparison window.
