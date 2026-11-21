import argparse
import hashlib
import json
from pathlib import Path
import struct

from bullet_probe import NativeBullet, ROOT, address, datum, emulate


BULLET = 0x390000
PROFILE = 0x4b2dd0


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local' / 'research'
    if (allowed.resolve() != allowed or destination.parent != allowed
            or destination.suffix != '.json' or not destination.name.startswith('bullet-visual')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be private local/research/bullet-visual*.json')
    return destination


class NativeBulletVisual(NativeBullet):
    def remap(self, color, sprite):
        if type(color) is not int or not 0 <= color <= 15 or type(sprite) is not int or not 0 <= sprite <= 3:
            raise ValueError('only type8/color0..15/four sprite input slots are supported')
        block = bytearray(0x11b4)
        struct.pack_into('<I', block, 0x4ec, BULLET)
        struct.pack_into('<2h', block, 0x11b0, 8, color)
        result = emulate([self.chunk(0x40ccb0, 0x40cce8)], 0x40ccb0,
                         [datum('bullet', BULLET, bytes(block), writable=False), self.constant(PROFILE, 0x120)],
                         registers={'ecx': BULLET + 8, 'edx': sprite}, budget=64)
        result.pop('memory')
        result['id'] = f'color{color:02d}-sprite{sprite}'
        result['input'] = {'type': 8, 'color': color, 'sprite': sprite,
                           'callbackOwner': address(BULLET), 'callbackVM': address(BULLET + 8),
                           'remap': list(struct.unpack('<4i', self.read(PROFILE + 4 + color * 16, 16)))}
        result['result'] = result['registers']['eax']
        return result


def animation_source():
    local = (ROOT / 'local').resolve()
    catalog = json.loads((local / 'catalog.json').read_text(encoding='utf-8'))
    manifest = (local / catalog['variants']['ja']['manifest']).resolve()
    source = (manifest.parent / 'raw' / 'bullet.anm').resolve()
    if not source.is_relative_to(local) or not 0 < source.stat().st_size <= 128 * 1024 * 1024:
        raise ValueError('unexpected actual Japanese bullet ANM resource')
    return {'resource': 'ja/raw/bullet.anm', 'sha256': hashlib.sha256(source.read_bytes()).hexdigest()}


def probe(executable):
    native = NativeBulletVisual(executable)
    try:
        profile = native.read(PROFILE, 0x120)
        cases = [native.remap(color, sprite) for color in range(16) for sprite in range(4)]
        if any(sample['stop']['reason'] != 'returned' for sample in cases):
            raise ValueError('actual-byte sprite callback did not return within its boundary')
        unknown = emulate([native.chunk(0x40974b, 0x409750)], 0x40974b, budget=16)
        if unknown['stop']['reason'] != 'unsupported_call':
            raise ValueError('undeclared native call was not stopped')
        for start, end in ((0x409750, 0x4097d0), (0x459689, 0x4596a4), (0x4099df, 0x409a02)):
            native.chunk(start, end)
        after = hashlib.sha256(native.executable.read_bytes()).hexdigest()
        return {'schema': 'th128-original-byte-bullet-visual-fixtures/1',
                'nativeCpuTrace': False, 'windowsGameProcessLaunched': False,
                'executable': {'sha256': hashlib.sha256(native.binary).hexdigest(), 'sha256After': after,
                               'unchanged': native.binary == native.executable.read_bytes()},
                'animation': animation_source(),
                'profile': {'type': 8, 'address': address(PROFILE), 'stride': 0x120,
                            'script': struct.unpack_from('<i', profile, 0)[0],
                            'secondaryScript': struct.unpack_from('<i', profile, 0x11c)[0],
                            'remapFormula': '0x4b24d4 + type*0x120 + color*16 + sprite*4'},
                'code': native.code, 'constants': native.constants, 'cases': cases, 'caseCount': len(cases),
                'unknownCall': unknown,
                'staticEvidence': {'resetCall': '0x0040974b', 'primaryBindCall': '0x00409777',
                                   'secondaryScriptRead': '0x0040979f', 'bindTickCall': '0x0045969f',
                                   'postTailPrimaryTickCall': '0x004099e9',
                                   'postTailSecondaryTickCall': '0x004099fd'},
                'limitations': ['Bounded actual-byte Unicorn helper emulation, not native Windows CPU execution.',
                                'Only type8 and sprite source slots 0..3, for colors 0..15.',
                                'Initialization scheduling is static EXE evidence; the full native ANM interpreter is not emulated.',
                                'No host API stubs, native process, full bullet update, collision or stage completion.']}
    finally:
        native.image.close()


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded original-byte TH128 bullet sprite callback')
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/bullet-visual-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.output)
    report = probe(arguments.exe)
    if not report['executable']['unchanged']:
        raise ValueError('read-only executable digest changed')
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(f'{report["caseCount"]} bounded actual-byte callbacks; no native Windows CPU or game process')


if __name__ == '__main__':
    main()
