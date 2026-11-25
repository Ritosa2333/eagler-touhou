import argparse
import json
import struct

from .machine import Machine

PLAYER = 0x004c22c4
TRANCE_FLAGS = 0x004be830
DIALOG_MANAGER = 0x004c2190
PLAYER_X = 0x5b8
PLAYER_Y = 0x5bc
HIT_SIZES = 0x660
PLAYER_STATE = 0x65c
HIT_TIMER = 0x14688
PLAYER_FLAGS = 0x14698
HIT_HANDLER = 0x444ca0
CIRCLE_COLLISION = 0x444140
SIZE_TABLES = {'4c0ba8': (0x4c0ba8, 4), '4c0bb8': (0x4c0bb8, 4), '4c0bc8': (0x4c0bc8, 4), '4c0bd8': (0x4c0bd8, 4)}


def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def collision_probe(machine):
    player = machine.alloc(0x15000)
    sizes = machine.alloc(8)
    bullet = machine.alloc(8)
    machine.write(PLAYER, struct.pack('<I', player))
    machine.write(player + HIT_SIZES, struct.pack('<I', sizes))
    machine.write(HIT_HANDLER, b'\xc3')

    def run(bx, by, size, radius, state=0, invulnerable=0, flags=0, trance=0, dialog=0):
        machine.write(sizes, struct.pack('<ff', 0.0, size))
        machine.write(player + PLAYER_X, struct.pack('<ff', 0.0, 0.0))
        machine.write(player + PLAYER_STATE, struct.pack('<i', state))
        machine.write(player + HIT_TIMER, struct.pack('<i', invulnerable))
        machine.write(player + PLAYER_FLAGS, struct.pack('<I', flags))
        machine.write(TRANCE_FLAGS, struct.pack('<I', trance))
        machine.write(DIALOG_MANAGER, struct.pack('<I', dialog))
        machine.write(bullet, struct.pack('<ff', bx, by))
        return machine.call(CIRCLE_COLLISION, [bits(radius)], eax=bullet) & 0xffffffff

    gate = []
    for state in range(6):
        for invulnerable in (-1, 0, 1):
            gate.append({'state': state, 'invulnerable': invulnerable,
                         'result': run(0.0, 0.0, 2.0, 1.0, state=state, invulnerable=invulnerable)})
    extras = {'trance': run(0.0, 0.0, 2.0, 1.0, trance=0x100), 'flag2': run(0.0, 0.0, 2.0, 1.0, flags=2),
              'flag1': run(0.0, 0.0, 2.0, 1.0, flags=1), 'dialog': run(0.0, 0.0, 2.0, 1.0, dialog=0x20000000)}
    machine.write(TRANCE_FLAGS, struct.pack('<I', 0))
    machine.write(DIALOG_MANAGER, struct.pack('<I', 0))
    sweeps = []
    for size, radius in ((2.0, 1.0), (3.0, 2.0), (3.0, 0.5)):
        rows = []
        for distance in (0.0, 0.5 * radius, 0.99 * (size ** 2 + radius ** 2) ** 0.5, (size ** 2 + radius ** 2) ** 0.5,
                         1.01 * (size ** 2 + radius ** 2) ** 0.5, 41.0, 42.0, 42.011, 42.012, 43.0, 100.0):
            rows.append([distance, run(float(distance), 0.0, size, radius)])
        sweeps.append({'size': size, 'radius': radius, 'rows': rows})
    return {'gate': gate, 'extras': extras, 'sweeps': sweeps}


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', default='/work/games/th13/th13.exe')
    parser.add_argument('--out', default='/work/artifacts/th13-fidelity/collision.json')
    options = parser.parse_args(argv)
    machine = Machine(options.exe)
    tables = {name: [machine.f32(address + 4 * index) for index in range(count)]
              for name, (address, count) in SIZE_TABLES.items()}
    document = {'schema': 'th13/oracle-probes/v1', 'tables': tables, 'collision': collision_probe(machine)}
    text = json.dumps(document, indent=1, sort_keys=True) + '\n'
    with open(options.out, 'w') as handle:
        handle.write(text)
    return text


if __name__ == '__main__':
    print(main())
