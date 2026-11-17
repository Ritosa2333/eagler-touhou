"""Compare TH05 PC-98 player input and enemy projectile motion with the port."""
import argparse
import json
from pathlib import Path
import runpy


def player_cases(vm):
    # Port and PC-98 cardinal bits use the same order. Shift is a separate
    # retail byte, not part of the direction mask.
    overlaps = [key for old in range(16) for new in range(16) for key in (old, new, new, 0)]
    boundaries = [key for key in (5, 9, 6, 10, 1, 2, 4, 8) for _ in range(120)]
    keys = overlaps + boundaries + [key | 64 for key in overlaps + boundaries]
    cases = []
    for character in range(4):
        vm.put(vm.resident + 0x14, character)
        vm.call(0xB063)
        vm.w(0x36D0, 0, 2)
        vm.w(0x36D2, 192 * 16, 2)
        vm.w(0x36D4, 184 * 16, 2)
        for address in (0x36EE, 0x36EF, 0x36F2, 0x36F3, 0x36F4):
            vm.w(address, 0)
        frames = []
        for key in keys:
            vm.w(0x3064, key & 15, 2)
            vm.w(0x3066, bool(key & 64))
            vm.call(0xCC3D)
            frames.append([vm.r(offset, 2, True) for offset in (0x36D2, 0x36D4, 0x36DA, 0x36DC)])
        cases.append(frames)
    return dict(keys=keys, cases=cases)


# Motion, cloud, origin, angle, speed, special parameter, turn angle, clear tick.
BULLETS = (
    ("straight", [-2, 0, 3072, 1500, 39, 28, 0, 0, -1]),
    ("clip-left", [-2, 0, -96, 1500, 128, 32, 0, 0, -1]),
    ("clip-bottom", [-2, 0, 3072, 5984, 64, 32, 0, 0, -1]),
    ("cloud-forwards", [-2, 4, 3072, 1500, 207, 39, 0, 0, -1]),
    ("cloud-backwards", [-2, 3, 3072, 1500, 39, 28, 0, 0, -1]),
    ("cloud-boundary", [-2, 4, -108, 1500, 128, 5, 0, 0, -1]),
    ("deceleration", [-1, 4, 3072, 1500, 219, 19, 0, 0, -1]),
    ("turn-aimed", [1, 0, 1000, 1500, 145, 19, 2, 0, -1]),
    ("turn-relative", [2, 0, 3072, 1500, 245, 25, 2, 56, -1]),
    ("accelerate", [3, 0, 3072, 1500, 29, 8, 2, 0, -1]),
    ("accelerate-wrap", [3, 0, 3072, 1500, 29, 252, 7, 0, -1]),
    ("turn-to-angle", [4, 0, 3072, 1500, 245, 37, 0, 80, -1]),
    ("bounce-x", [5, 0, 6080, 1500, 245, 48, 2, 0, -1]),
    ("bounce-y", [6, 0, 3072, 5824, 65, 50, 2, 0, -1]),
    ("bounce-corner", [7, 0, 14, 14, 160, 38, 1, 0, -1]),
    ("bounce-top", [8, 0, 3072, 30, 192, 48, 1, 0, -1]),
    ("gravity", [9, 0, 3072, 1500, 224, 24, 2, 0, -1]),
    ("exact-line", [10, 0, 3072, 1500, 17, 32, 0, 0, -1]),
    ("clear-fresh", [-2, 0, 3072, 1500, 17, 32, 0, 0, 0]),
    ("clear-aged", [-2, 0, 3072, 1500, 17, 32, 0, 0, 3]),
    ("graze-right", [-2, 0, 3290, 5100, 0, 0, 0, 0, -1, 3000, 5100, 0]),
    ("graze-left", [-2, 0, 2710, 5100, 0, 0, 0, 0, -1, 3000, 5100, 0]),
    ("graze-bottom", [-2, 0, 3000, 5440, 0, 0, 0, 0, -1, 3000, 5100, 0]),
    ("graze-before-hit", [-2, 0, 3064, 5100, 0, 0, 0, 0, -1, 3000, 5100, 0]),
    ("invulnerable-no-graze", [-2, 0, 3064, 5100, 0, 0, 0, 0, -1, 3000, 5100, 255]),
)


def bullet_case(vm, name, start):
    motion, cloud, x, y, angle, speed, parameter, delta, clear, *player = start
    player_x, player_y, invulnerable = player or (3000, 5100, 255)
    slot = 0x5F2A + 399 * 32
    vm.u.mem_write(vm.DS * 16 + 0x5F2A, bytes(400 * 32))
    vm.w(0x36D2, player_x, 2)
    vm.w(0x36D4, player_y, 2)
    vm.w(0x36EE, invulnerable)
    vm.w(0x3712, 0, 4)
    vm.w(0x3F98, 0, 2)
    vm.w(0x3F94, parameter)
    vm.w(0x3F96, 0)  # Zap flag.
    for offset, value in ((0, 1), (16, speed), (17, angle), (18, cloud),
                          (19, 1 if motion > 0 else 0 if motion == -1 else 2),
                          (20, motion), (21, speed), (22, 32 if motion == -1 else 0),
                          (23, 72 - speed if motion == -1 else delta)):
        vm.w(slot + offset, value)
    for offset, value in ((2, x), (4, y), (24, 84 + ((angle + 3) & 255) // 8),
                          (26, x), (28, y)):
        vm.w(slot + offset, value, 2)
    vm.call(0x152DA, vm.C3, args=(slot + 10, angle, speed))
    frames = []
    for tick in range(160):
        vm.w(0x5766, tick & 1)
        vm.w(0x3F97, max(0, 20 - (tick - clear)) if clear >= 0 and tick >= clear else 0)
        vm.w(0x36F3, 0)
        vm.call(0x1C0DF, vm.C3, True)
        flag = vm.r(slot)
        row = [flag]
        if flag:
            row.extend(vm.r(slot + offset, 2, True) for offset in (2, 4, 10, 12, 24))
            row.extend(vm.r(slot + offset) for offset in (16, 17, 18, 19, 1))
            row.extend((vm.r(0x3712, 4), vm.r(0x3F98, 2), vm.r(0x36F3)))
        frames.append(row)
        if not flag:
            break
    return dict(name=name, start=start, frames=frames)


# Mode, origin, angle, width, speed, tail, head, age, grow age, shrink age,
# final width and clear tick. Laser drawing still runs for slot retirement.
LASERS = (
    ("laser-side-exit", [1, 5200, 1000, 14, 6, 81, 256, 256, 0, 12, 0, 6, -1]),
    ("laser-top-exit", [1, 2000, 2500, 195, 6, 73, 256, 256, 0, 22, 0, 6, -1]),
    ("laser-cancel", [1, 3072, 1280, 73, 6, 85, 256, 256, 0, 24, 0, 6, 16]),
    ("laser-grow-shrink", [2, 3072, 1280, 55, 1, 0, 256, 8800, 0, 9, 35, 8, -1]),
    ("laser-shrink-rearm", [6, 3072, 1280, 55, 8, 0, 256, 8800, 0, 9, 35, 8, -1]),
)


def laser_case(vm, name, start):
    slot = 0x578C
    vm.u.mem_write(vm.DS * 16 + slot, bytes(32 * 24))
    for offset, value in zip((0, 2, 4, 10, 11, 12, 6, 8, 14, 16, 18, 20), start):
        vm.w(slot + offset, value, 1 if offset in (0, 10, 11, 20) else 2)
    frames = []
    for tick in range(200):
        vm.w(0x3F97, int(tick == start[-1]))
        vm.w(0x36F3, 0)
        vm.call(0x12BAD)
        vm.call(0x12CE5)
        mode = vm.r(slot)
        row = [mode]
        if mode:
            row.extend(vm.r(slot + offset, 2, True) for offset in (6, 8, 12, 14))
            row.extend((vm.r(slot + 11), vm.r(0x36F3)))
        frames.append(row)
        if not mode:
            break
    return dict(name=name, start=start, frames=frames)


def slowdown_cases(vm):
    slot = 0x5F2A
    cases = []
    for difficulty in range(5):
        threshold = 42 + difficulty * 8
        for count in (threshold - 1, threshold, threshold + 32):
            for odd in (0, 1):
                for turbo in (0, 1):
                    vm.u.mem_write(vm.DS * 16 + slot, bytes(400 * 32))
                    for i in range(count):
                        address = slot + i * 32
                        vm.w(address, 1)
                        vm.w(address + 2, 1000, 2)
                        vm.w(address + 4, 1000, 2)
                        vm.w(address + 19, 2)
                    vm.w(0x3F97, 0)
                    vm.w(0x5766, odd)
                    vm.w(0x576F, difficulty)
                    vm.w(0x5776, turbo)
                    vm.w(0x576A, 1, 2)
                    vm.call(0x1C0DF, vm.C3, True)
                    cases.append([difficulty, count, odd, turbo, vm.r(0x576A, 2)])
    return cases


def rank_cases(vm, oracle):
    initial, tuning = [], []
    for difficulty in range(5):
        vm.put(vm.resident + 0x11, difficulty)
        vm.w(0x576E, 6 if difficulty == 4 else 0)
        vm.call(0xB063)
        initial.append([vm.r(offset) for offset in (0x5770, 0x5772, 0x5771)])
        for rank in (16, 23, 24, 25, 32, 37, 38, 39, 44, 48, 54, 58):
            vm.w(0x5770, rank)
            for group in range(16):
                for ways, spread, stack, delta in ((1, 1, 1, 3), (5, 10, 4, 8), (40, 16, 10, 11)):
                    template = (group, ways, spread, stack, delta)
                    for offset, value in zip((0x5784, 0x5786, 0x5787, 0x5788, 0x5789), template):
                        vm.w(offset, value)
                    vm.call(vm.C3 * 16 - vm.BASE + vm.r(0x41DE, 2), vm.C3)
                    tuned = [vm.r(offset) for offset in (0x5784, 0x5786, 0x5787, 0x5788, 0x5789)]
                    tuning.append([difficulty, rank, *template, tuned])

    speeds = []
    for rank in range(16, 59):
        vm.w(0x5770, rank)
        row = []
        for speed in range(256):
            vm.u.reg_write(oracle["UC_X86_REG_AX"], speed)
            vm.call(0x171EA, vm.C3)
            row.append(vm.u.reg_read(oracle["UC_X86_REG_AX"]) & 255)
        speeds.append(row)

    slot = 0xB084
    vm.w(0x576E, 0)
    vm.put(vm.resident + 0x11, 1)
    vm.call(0xB063)
    vm.w(0x5770, 24)
    vm.w(0x1E02, 0)
    vm.w(0x1E7E, 0, 2)
    vm.w(0x1E80, 0, 2)
    items = []
    for kind, pull, dream, y, repeat in ((2, 0, 1, 1000, 12), (2, 1, 1, 1000, 10),
                                        (1, 0, 1, 5000, 17), (1, 0, 128, 5000, 20),
                                        (1, 1, 1, 1000, 12), (5, 0, 128, 1000, 1)):
        for _ in range(repeat):
            vm.w(0x1E84, pull)
            vm.w(0x36F6, dream)
            vm.w(slot + 2, 1000, 2)
            vm.w(slot + 4, y, 2)
            vm.w(slot + 14, kind)
            vm.call(0x1CDFB, vm.C3, args=(slot,))
            items.append([kind, pull, dream, y, vm.r(0x5770), vm.r(0x1E02)])

    vm.w(0x1E02, 31)
    vm.call(0xB55A)
    after_stage_init = vm.r(0x1E02)
    # Execute the stage-clear Rank band, stopping before score display.
    vm.stub(0x1C9A8)
    vm.u.reg_write(oracle["UC_X86_REG_BP"], 0xFD00)
    summary = []
    for graze, points in [(0, n) for n in (99, 100, 101, 199, 200, 201, 499, 500, 501, 799, 800, 801)] + [(100, n) for n in (799, 800, 801)]:
        vm.put(0x90000 + 0xFD00 - 4, (1000 + graze * 5) * points, 4)
        vm.w(0x5770, 32)
        vm.call(0x1C95A, vm.C3)
        summary.append([graze, points, vm.r(0x5770)])
    return dict(initial=initial, tuning=tuning, speeds=speeds, items=items,
                afterStageInit=after_stage_init, summary=summary)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", required=True, type=Path)
    parser.add_argument("--resources", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    oracle = runpy.run_path(str(Path(__file__).with_name("Verify-OriginalDemos.py")))
    vm = oracle["VM"](1, args.exe, oracle["read_resources"](args.resources), stage=0, difficulty=1)
    player = player_cases(vm)
    bullets = [bullet_case(vm, name, start) for name, start in BULLETS]
    lasers = [laser_case(vm, name, start) for name, start in LASERS]
    slowdown = slowdown_cases(vm)
    rank = rank_cases(vm, oracle)
    args.output.write_text(json.dumps(dict(player=player, bullets=bullets, lasers=lasers, slowdown=slowdown, rank=rank),
                                      separators=(",", ":")),
                           encoding="utf-8")
    print(f"Captured {len(player['keys']) * len(player['cases'])} player frames and "
          f"{sum(len(case['frames']) for case in bullets)} bullet frames")


if __name__ == "__main__":
    main()
