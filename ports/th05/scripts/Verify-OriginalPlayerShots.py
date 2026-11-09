"""Capture retail TH05 player-shot state using a locally owned MAIN.EXE."""
import argparse
import json
from pathlib import Path
import runpy

POWERS = (1, 6, 12, 16, 24, 32, 48, 72, 96, 128)
SHOT_POOL = 0xBA8E
SHOT_BYTES = 18
SHOT_COUNT = 64


def capture(vm, character, level, power, homing):
    vm.u.mem_write(vm.DS * 16 + SHOT_POOL, bytes(SHOT_COUNT * SHOT_BYTES))
    vm.w(0x3626, 0)  # RNG ring cursor
    vm.w(0xC4BE, 0x1D70 + 20 * character, 2)  # Character's shot function table
    vm.w(0x36F0, power)
    vm.call(0xE3E2, far=True)  # Choose the shot routine for this power
    if vm.r(0x36F1) != level:
        raise RuntimeError(f"Unexpected retail shot level for power {power}")
    vm.w(0x36D2, 192 * 16, 2)
    vm.w(0x36D4, 320 * 16, 2)
    vm.w(0x3F8A, (120 if homing else -999) * 16, 2)
    vm.w(0x3F8C, 140 * 16, 2)
    frames, cursors = [], []
    for frame in range(72):
        vm.frame = frame
        vm.w(0x36F2, 18 - frame % 18)
        cursors.append(vm.r(0x3626))
        vm.indirect(0xC4BC)
        vm.call(0x133DB)  # Update movement, homing, acceleration and clipping
        shots = []
        for slot in range(SHOT_COUNT):
            address = SHOT_POOL + SHOT_BYTES * slot
            if vm.r(address) == 1:
                motion = [vm.r(address + offset, 2, True) for offset in (2, 4, 10, 12)]
                attributes = [vm.r(address + offset) for offset in (14, 15, 16)]
                shots.append(motion + attributes)
        frames.append(shots)
    return dict(char=character, level=level, power=power, homing=homing,
                frames=frames, cursors=cursors)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path, required=True)
    parser.add_argument("--resources", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    oracle = runpy.run_path(str(Path(__file__).with_name("Verify-OriginalDemos.py")))
    vm = oracle["VM"](1, args.exe, oracle["read_resources"](args.resources), stage=0, difficulty=1)
    ring = list(vm.u.mem_read(vm.DS * 16 + 0x3526, 256))
    cases = [
        capture(vm, character, level, power, homing)
        for character in range(4)
        for level, power in enumerate(POWERS)
        for homing in (False, True)
    ]
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(dict(ring=ring, cases=cases), separators=(",", ":")),
                           encoding="utf-8")
    print(f"Captured {len(cases)} cases and {sum(len(case['frames']) for case in cases)} frames")


if __name__ == "__main__":
    main()
