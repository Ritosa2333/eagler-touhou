import argparse
import hashlib
import json
import struct
from pathlib import Path

import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-original-type13-bullet/1'
PROFILE_ADDRESS = 0x4b24d0 + 13 * 0x120


def profile_bytes(executable):
    binary = Path(executable).read_bytes()
    if hashlib.sha256(binary).hexdigest() != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        return bytes(image.get_data(PROFILE_ADDRESS - 0x400000, 0x120))
    finally:
        image.close()


def actual_emitter():
    catalog = json.loads((ROOT / 'local/catalog.json').read_text(encoding='utf-8'))
    raw = (ROOT / 'local' / catalog['variants']['ja']['manifest']).parent / 'raw'
    ecl = json.loads((raw.parent / 'ecl' / 'st_a1_2.json').read_text(encoding='utf-8'))
    for subroutine in ecl['subroutines']:
        if subroutine['name'] != 'MBoss1At1':
            continue
        for command in subroutine['commands']:
            if command['offset'] == 64:
                words = list(struct.unpack('<3i', bytes.fromhex(command['raw'])))
                if words != [0, 13, 6]:
                    raise ValueError('unexpected actual type13 emitter record')
                return {'subroutine': 'MBoss1At1', 'offset': 64, 'slot': words[0],
                        'type': words[1], 'color': words[2], 'raw': command['raw']}
    raise ValueError('actual type13 emitter record is missing')


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/type13-bullet-fixtures.json'))
    args = parser.parse_args(argv)
    data = profile_bytes(Path(args.exe))
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/type13-bullet-fixtures.json')
    script = struct.unpack_from('<i', data, 0)[0]
    secondary = struct.unpack_from('<i', data, 0x11c)[0]
    hitbox = struct.unpack_from('<f', data, 0x104)[0]
    auxiliary = struct.unpack_from('<f', data, 0x108)[0]
    appearance = struct.unpack_from('<i', data, 0x10c)[0]
    selector = struct.unpack_from('<i', data, 0x110)[0]
    freeze = struct.unpack_from('<i', data, 0x118)[0]
    remap = {}
    cancel = {}
    for color in range(16):
        remap[str(color)] = list(struct.unpack_from('<4i', data, 4 + color * 16))
        cancel[str(color)] = struct.unpack_from('<i', data, 0x10 + color * 16)[0]
    if (script, secondary, appearance, selector, freeze) != (48, 0, 4, 6, 0) or \
            abs(hitbox - 2.4) > 1e-6 or abs(auxiliary - 16.0) > 1e-6:
        raise ValueError('unexpected type13 profile fields')
    fixture = {
        'schema': SCHEMA,
        'sourceExeSha256': EXE_SHA256,
        'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        'launchesOriginalExecutable': False,
        'profile': {
            'address': f'0x{PROFILE_ADDRESS:08x}', 'bytes': 288,
            'sha256': hashlib.sha256(data).hexdigest(), 'hex': data.hex(),
            'type': 13, 'script': script, 'secondaryScript': secondary,
            'hitboxSize': hitbox, 'auxiliarySize': auxiliary,
            'appearance': appearance, 'selector': selector, 'freezePolicy': freeze,
            'identitySpriteRemap': False,
            'spriteRemapColor6': remap['6'],
            'cancellationScriptColor6': cancel['6'],
            'cancellationScriptColor0': cancel['0'],
            'spriteRemap': remap, 'cancellationScripts': cancel,
        },
        'actualEmitter': actual_emitter(),
        'cancellationBoundary': {
            'root': cancel['6'], 'children': [348],
            'note': 'script349 op88 child348 (two op88 commands); current CancellationEffects covers 357/356 and 371/370, not 349/348',
        },
        'supportedDomain': {
            'profile': 'script48, no secondary, hitbox2.4, aux16, appearance4, selector6, freeze0',
            'visuals': 'single primary ANM; non-identity color remap',
            'tick': 'generic single-VM typed motion domain; no type-specific branch',
            'cancellation': 'profile value retained; CancellationEffects owner remains unsupported for349/348',
        },
        'limitations': ['No original EXE process execution.', 'Cancellation effects 349/348 are not implemented in this source scope.'],
    }
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('type13 profile 0x4b3370 script48 color6 cancel349 children348; remap', remap['6'])


if __name__ == '__main__':
    main()
