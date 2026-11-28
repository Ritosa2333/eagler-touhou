import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-perfect-owner-integration/1'
SLICES = {
    'activate407810': (0x407810, 0x4078af),
    'protect407a70': (0x407a70, 0x407c4e),
    'phase407c50': (0x407c50, 0x407d15),
    'phase120407d6d': (0x407d6d, 0x407deb),
    'laserDamage410a60': (0x410a60, 0x410b4d),
}
CONSTANTS = (0x4a69ec, 0x4a68a8, 0x4a6a60, 0x4a6618)


def read(image, address, size):
    data = bytes(image.get_data(address - 0x400000, size))
    if len(data) != size:
        raise ValueError('truncated original read')
    return data


def disasm(image, start, end):
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [{'address': f'0x{item.address:08x}', 'bytes': item.bytes.hex(),
             'text': f'{item.mnemonic} {item.op_str}'.strip()}
            for item in decoder.disasm(read(image, start, end - start), start)]


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        slices = {name: disasm(image, start, end) for name, (start, end) in SLICES.items()}
        constants = {}
        for address in CONSTANTS:
            payload = read(image, address, 8)
            constants[f'0x{address:08x}'] = {'bytes': payload.hex(),
                                              'f32': struct.unpack('<f', payload[:4])[0],
                                              'f64': struct.unpack('<d', payload)[0]}
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'slices': slices,
            'constants': constants,
            'protection': {
                'writer': '0x407a70',
                'playerFields': {'+0x438d8': 'previous=199', '+0x438dc': 'current=200',
                                 '+0x438e0': 'fractional=float200(const 0x4a69ec)',
                                 '+0x438e4': 'clock=0x4b6f58', '+0x438e8': 'flags bit0'},
                'semantics': 'assignment of 200, not max/extend of existing value',
            },
            'phase': {
                'activate': '0x407810 sets system +0x3c=1 and calls 0x407a70 when global empty',
                'freeze120': '0x407d6d sets manager +0x6b1fc bit4; bullet freeze owner',
                'laserDamage180': '0x407c50 timer>=180 calls 0x410a60',
            },
            'damage': {
                'spawn': '0x43df90',
                'onlyCallSite': '0x410b15',
                'trigger': '0x410a60 after perfect phase timer >=180, per active laser with +0x4c==0',
                'position': 'laser+0x10 three floats',
                'radius': 'laser+0x1c * 0x4a68a8 (3.8)',
                'growth': 0.0,
                'duration': 1,
                'damage': 70,
                'tag': 1,
                'notActivationDamage': 'no 0x43df90 call in 0x407810/0x407a70 activation slice',
            },
            'hostGap': {
                'damageAtActivation': 'current PerfectFreezeHost calls damage(input.radius) at activation; original has no such damage-field call',
                'phase180Missing': 'host phase stops at120 and has no phase180 laser-damage event',
                'laserOwnerMissing': 'WorldExports has no laser object list/owner; laserCount is insufficient for radius/position',
                'protectionSetterMissing': 'PlayerHitHost has stepProtection but no public 200-timer assignment',
            },
            'limitations': ['Static read-only slices only; no original process.'],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/perfect-owner-integration-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/perfect-owner-integration-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('perfect protection 200 @0x407a70; damage field 0x43df90 only @0x410b15 phase180 radius=laser+1c*3.8 damage70 tag1')


if __name__ == '__main__':
    main()
