import argparse
import hashlib
import json
from pathlib import Path
import struct

from bullet_probe import NativeBullet, ROOT, EXE_SHA256, datum
from freeze_probe import emulate, STACK_POINTER

PLAYER, MANAGER, SYSTEM = 0x300000, 0x380000, 0x390000


def timer(previous, current, flags=1):
    return struct.pack('<iifII', previous, current, float(current), 0x4b6f58, flags)


class NativePlayerFire(NativeBullet):
    def head(self, *, state=1, current=-1, previous=-2, holding=0, pressed=0, flags=1):
        data = [datum('state', PLAYER + 0xf78, state, writable=False),
                datum('shotTimer', PLAYER + 0x438fc, timer(previous, current, flags)),
                datum('holding', 0x4db210, holding, kind='I', writable=False),
                datum('pressed', 0x4db21c, pressed, kind='I', writable=False),
                datum('nonNormalWrites', PLAYER + 0x8ed4, bytes(8))]
        result = emulate([self.chunk(0x43f1b0, 0x43f264), self.chunk(0x43f47b, 0x43f48b)],
                         0x43f1b0, data, arguments=struct.pack('<I', PLAYER),
                         stops=[0x43f25d, 0x43f264, 0x43f2e1, 0x43f48b], budget=1024)
        result.update(state=state, holding=holding, pressed=pressed, current=current, previous=previous, flags=flags)
        return result

    def charge(self, *, held=0, holding=0, busy=0, mode=0, power=10000, saved=0):
        data = [datum('heldTimer', PLAYER + 0x43910, timer(held - 1, held)),
                datum('scale', PLAYER + 0x87b0, struct.pack('<f', .5)),
                datum('saved', PLAYER + 0x438f4, saved),
                datum('holding', 0x4db210, holding, kind='I', writable=False),
                datum('managerPointer', 0x4b8938, MANAGER, kind='I', writable=False),
                datum('busy', MANAGER + 0x6b218, busy, writable=False),
                datum('systemPointer', 0x4b892c, SYSTEM, kind='I', writable=False),
                datum('mode', SYSTEM + 0x3c, mode, writable=False),
                datum('power', 0x4b4d6c, power),
                datum('clock', 0x4b6f58, struct.pack('<f', 1), writable=False)]
        data += [self.constant(pointer, size) for pointer, size in
                 ((0x4a6698, 8), (0x4a6694, 4), (0x4a6588, 8))]
        result = emulate([self.chunk(0x43f2e3, 0x43f48b), self.chunk(0x402ac0, 0x402b18),
                          self.chunk(0x403280, 0x4032bd), self.chunk(0x41f410, 0x41f470)],
                         0x43f2e3, data, registers={'edi': PLAYER, 'ebx': 0, 'ebp': STACK_POINTER + 0x100},
                         stack={0x108: struct.pack('<I', PLAYER)},
                         stops=[0x43f3dc, 0x43f48b], budget=2048)
        result.update(held=held, holding=holding, busy=busy, mode=mode, power=power, saved=saved)
        return result


def report_destination(value):
    destination = Path(value).resolve()
    allowed = ROOT / 'local/research'
    if destination.parent != allowed or not destination.name.startswith('player-fire-') or destination.suffix != '.json':
        raise ValueError('report must be owned local/research/player-fire-*.json')
    return destination


def probe(executable):
    native = NativePlayerFire(executable)
    try:
        heads = [native.head(current=count, previous=count - 1, holding=holding, pressed=pressed, flags=flags)
                 for count in (-1, 0, 6, 14) for holding, pressed in ((0, 0), (1, 1), (1, 0), (512, 0))
                 for flags in (0, 1)]
        heads += [native.head(current=3, previous=3), native.head(state=2)]
        charges = [native.charge(held=held, holding=holding) for held in (-40, -1, 0, 1, 11, 14, 19, 20, 44, 45, 79, 80, 109, 110, 139, 140)
                   for holding in (0, 1)]
        charges += [native.charge(held=held, holding=holding, busy=busy, mode=mode, power=power)
                    for held, holding, busy, mode, power in
                    ((44, 1, 0, 0, 2999), (45, 0, 0, 0, 2999), (45, 0, 0, 0, 3000),
                     (45, 1, 0, 0, 3000), (0, 1, 1, 0, 10000), (0, 1, 0, 1, 10000), (19, 1, 0, 0, 9999))]
        for sample in heads + charges:
            if sample['stop']['reason'] not in ('boundary', 'unsupported_call'):
                raise ValueError(f'unexpected original execution stop: {sample["stop"]}')
        return {'schema': 'th128-original-byte-player-fire-fixtures/1', 'executable': {'sha256': EXE_SHA256,
                'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
                'windowsGameProcessLaunched': False, 'nativeCpuTrace': False, 'headCases': heads,
                'chargeCases': charges, 'code': native.code, 'constants': native.constants,
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                'engineSha256': {name: hashlib.sha256((Path(__file__).parent / name).read_bytes()).hexdigest()
                                 for name in ('bullet_probe.py', 'freeze_probe.py')}}
    finally:
        native.image.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/player-fire-fixtures.json'))
    arguments = parser.parse_args()
    destination = report_destination(arguments.output)
    report = probe(arguments.exe)
    destination.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8', newline='\n')
    print(f'{len(report["headCases"])} fire heads, {len(report["chargeCases"])} charge prefixes; no fabricated call returns')


if __name__ == '__main__':
    main()
