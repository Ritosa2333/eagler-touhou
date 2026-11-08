"""Capture Bomb timers and damage from an owned PC-98 TH05 MAIN.EXE.

Compare with the browser build via browser.mjs --pc98-combat FILE.
"""
import argparse
import itertools
import json
import runpy
from pathlib import Path

native = runpy.run_path(str(Path(__file__).with_name('Verify-OriginalDemos.py')))
VM = native['VM']
AX = native['UC_X86_REG_AX']


def new_game(exe, resources, stage, character):
    vm = VM(5 if stage == 6 else 1, exe, resources, stage=stage,
            difficulty=4 if stage == 6 else 1)
    vm.put(vm.resident + 0x14, character)
    vm.call(0xb063)
    vm.call(0xb55a)
    return vm


def damage(vm, boss, bombing, age, frame, x, y, shots):
    for address, value in [(0x1dc0, boss), (0x4206, bombing),
                           (0x4207, age), (0x5767, frame % 4)]:
        vm.w(address, value)
    for address, value in [(0x36d2, x), (0x36d4, y),
                           (0xc4c0, 192 * 16), (0xc4c2, 96 * 16),
                           (0xc4c4, 24 * 16), (0xc4c6, 24 * 16)]:
        vm.w(address, value, 2)
    vm.w(0xc648, len(shots), 2)
    for i, strength in enumerate(shots):
        shot = 0xba8e + i * 18
        for offset, value in [(0, 1), (14, 20), (16, strength)]:
            vm.w(shot + offset, value)
        active = 0xc4c8 + 6 * i
        vm.w(active, 192 * 16, 2)
        vm.w(active + 2, 96 * 16, 2)
        vm.w(active + 4, shot, 2)
    vm.call(0x13683, vm.C1, True)
    return vm.u.reg_read(AX)


def lifecycle(vm, phase, held):
    for offset, value in [(0x36ee, 0), (0x36ef, 0), (0x36f3, 0),
                          (0x36f4, 0), (0x4ad5, 3), (0x36f6, 128),
                          (0x5770, 32), (0x5ac9, phase)]:
        vm.w(offset, value)
    vm.w(0x36d2, 192 * 16, 2)
    vm.w(0x36d4, 300 * 16, 2)
    ring = list(vm.u.mem_read(vm.DS * 16 + 0x3526, 256))
    cursor = vm.r(0x3626)
    frames = []
    for frame in range(240):
        vm.frame = frame
        vm.w(0x5764, frame, 2)
        for offset, mask in [(0x5766, 1), (0x5767, 3), (0x5768, 7), (0x5769, 15)]:
            vm.w(offset, frame & mask)
        key = 16 if frame == 0 or held else 0
        vm.w(0x3064, key, 2)
        vm.call(0xcc3d)
        vm.call(0x1c0df, vm.C3, True)
        if vm.r(0x4206):
            vm.indirect(0x4208)
        frames.append([key, *[vm.r(o) for o in (0x4206, 0x4207, 0x36ee,
                                                0x3f97, 0x4ad5, 0x36f6,
                                                0x1e84, 0x5770, 0x3626)]])
    return dict(ring=ring, cursor=cursor, frames=frames)


def special_clear(vm, kind, angle, clear):
    slot = 0xb40e
    vm.w(0x5770, 32)
    vm.w(0x36d2, 3000, 2)
    vm.w(0x36d4, 5000, 2)
    vm.w(0xb3f5, angle)
    vm.w(0xb3f6, 3000, 2)
    vm.w(0xb3f8, 3000, 2)
    vm.w(0xb402, 16, 2)
    vm.w(0xb406, 201, 2)
    vm.w(0xb40c, 61)
    add, step = (0x1ed41, 0x1edba) if kind == 0 else (0x1aa7e, 0x1aacf)
    vm.call(add, vm.C3)
    frames = []
    for frame in range(56):
        vm.w(0x3f97, 20 - (frame - clear) if 0 <= frame - clear < 20 else 0)
        vm.call(step, vm.C3)
        flag = vm.r(slot)
        row = [flag]
        if flag:
            row.extend(vm.r(slot + offset, 2, True) for offset in (2, 4, 10, 12, 18))
            row.append(vm.r(slot + (14 if kind == 0 else 16), 2, True))
        frames.append(row)
    return dict(kind=kind, angle=angle, clear=clear, frames=frames)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--resources', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    resources = native['read_resources'](args.resources)
    scenarios = []
    for stage, character in itertools.product((0, 6), range(4)):
        vm = new_game(args.exe, resources, stage, character)
        cases = []
        for boss, bombing, age, frame, shots in itertools.product(
                (0, 1), (0, 1), (15, 16, 17, 76, 80), (0, 1), ([], [16, 9, 1])):
            positions = [(192 * 16, 320 * 16)]
            if character == 1 and bombing and age in (16, 76):
                positions = [(x * 16, y * 16) for x, y in
                             [(192, 71), (168, 72), (216, 95), (217, 320)]]
            for x, y in positions:
                request = [boss, bombing, age, frame, x, y, shots]
                cases.append([*request, damage(vm, *request)])
        scenarios.append(dict(stage=stage, character=character, cases=cases))
    bombs = []
    for character, (phase, held) in itertools.product(range(4), [(0, False), (2, False), (2, True)]):
        vm = new_game(args.exe, resources, 0, character)
        bombs.append(dict(character=character, phase=phase, **lifecycle(vm, phase, held)))
    specials = [
        special_clear(new_game(args.exe, resources, 4 + kind, 0), kind, angle, clear)
        for kind, angle, clear in itertools.product((0, 1), (19, 173), (2, 20, 40))
    ]
    args.output.write_text(json.dumps(dict(damage=scenarios, bombs=bombs, specials=specials),
                                      separators=(',', ':')),
                           encoding='utf-8')


if __name__ == '__main__':
    main()

