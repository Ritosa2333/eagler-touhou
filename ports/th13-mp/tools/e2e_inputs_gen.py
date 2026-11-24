#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."))

from oracle import replayfmt


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--replay", default="games/replays/th13_01.rpy")
    ap.add_argument("--out", default="artifacts/tmp-scratch/e2e_inputs.json")
    args = ap.parse_args()

    data = open(args.replay, "rb").read()
    r = replayfmt.ReplayFile()
    r.decode(data)

    stages = []
    for s in r.stages:
        held = []
        for i in range(s.frame_count):
            h, _p, _r = s.frame(i)
            unknown = h & ~0x0BFF
            if unknown:
                raise SystemExit("stage %d frame %d: unmapped held bits %#x"
                                 % (s.number, i, unknown))
            if (h & 0x800) and not (h & 0x200):
                raise SystemExit("stage %d frame %d: lone 0x800 without 0x200"
                                 % (s.number, i))
            held.append(h)
        stages.append({
            "number": s.number,
            "seed": s.stamp,
            "frames": s.frame_count,
            "start_score": s.score,
            "held": held,
        })

    doc = {
        "source": args.replay,
        "player_name": r.player_name,
        "score": r.score,
        "character": r.character,
        "shot_type": r.shot_type,
        "rank": r.rank,
        "slow_rate": r.slow_rate,
        "stage_count": r.stage_count,
        "stages": stages,
    }
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    with open(args.out, "w") as f:
        json.dump(doc, f)
    total = sum(s["frames"] for s in stages)
    print("wrote %s: %d stages, %d frames, seeds=%s"
          % (args.out, len(stages), total, [s["seed"] for s in stages]))
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
