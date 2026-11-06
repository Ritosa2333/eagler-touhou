#!/usr/bin/env python3
"""E2E 逐帧键态生成器：th13_01.rpy → 每关 held u16 数组（JSON）。

外层 oracle/replayfmt.py 解码（ReplayFile.cpp 的 python 孪生），把每关
「当前态」u16 流原样导出，供浏览器端 E2E 驱动作为玩家键盘键态（每游戏帧
一个 held 值，经 _th13_key 键沿注入生产 GameKeyboard/GameInput 链）。

用法（容器内；$ 经外层 make exec 会被吞，复合命令落脚本文件）：
  python3 eagler-touhou/ports/th13/tools/e2e_inputs_gen.py \
      --replay games/replays/th13_01.rpy --out artifacts/tmp-scratch/e2e_inputs.json
"""
from __future__ import annotations

import argparse
import json
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."))

from oracle import replayfmt  # noqa: E402


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
            # 注入映射只覆盖这些位；0x800(C) 恒与 0x200(Ctrl) 同现于原版流
            #（录侧 quirk：C 键 = 0x200|0x800，GameKeyboard vk67 同源）。
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
