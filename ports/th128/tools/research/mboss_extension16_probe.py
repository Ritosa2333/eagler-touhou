import argparse
import hashlib
import json
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-mboss-extension16/1'
RECORD = {'control': 0, 'type': 16, 'A': 60, 'B': 1, 'C': 0, 'D': 0,
          'R': 2.094395160675049, 'S': 0.5, 'T': -999999.0, 'U': -999999.0, 'callback': 0}


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


def actual_record():
    catalog = json.loads((ROOT / 'local/catalog.json').read_text(encoding='utf-8'))
    raw = (ROOT / 'local' / catalog['variants']['ja']['manifest']).parent / 'raw'
    ecl = json.loads((raw.parent / 'ecl' / 'st_a1_2.json').read_text(encoding='utf-8'))
    for subroutine in ecl['subroutines']:
        if subroutine['name'] != 'MBoss1At3':
            continue
        for command in subroutine['commands']:
            if command['offset'] == 432:
                return {'subroutine': 'MBoss1At3', 'offset': 432, 'opcode': command['opcode'],
                        'raw': command['raw'], 'record': RECORD}
    raise ValueError('MBoss1At3 offset432 record is missing')


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        record = actual_record()
        mode_tick = disasm(image, 0x40ae90, 0x40b041)
        if not any('esi + 0xe4c' in item['text'] and 'fld' in item['text'] for item in mode_tick):
            raise ValueError('mode0 angle source missing')
        if not any('esi + 0x9f4' in item['text'] and 'fadd' in item['text'] for item in mode_tick):
            raise ValueError('mode0 current-angle add missing')
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'actualRecord': record,
            'dispatch': {
                'modeSelector': '0x408913 mode field +0xe74',
                'mode0Updater': '0x40ae90',
                'mode2Updater': '0x40b200',
                'mode0TickDisassembly': mode_tick,
                'initializerAngleModeSwitch': '0x409c65 angleMode +0x1c',
                'initializerStores': '0x409d68..0x409d95 period/limit/count/mode/angleMode',
            },
            'mode0Semantics': {
                'angleAtBoundary': 'retained R/e4c plus current bullet angle 0x9f4; no player aim',
                'speedAtBoundary': 'retained target speed e48',
                'elapsedSpeed': 'base - base*time/period',
                'timer': 'unit-clock timer; boundary resets time/count and uses sound requirement',
                'limit': 'increment count; clear active bit16 when count reaches limit',
                'angleMode': 0,
            },
            'mode2Semantics': 'existing targetAnglePrefix + R, unchanged',
            'currentGuard': {
                'before': 'C must equal 2 and D equal 0; actual C0/D0 rejected',
                'newDomain': 'C in {0,2}, D=0; mode field retained',
                'unknown': 'C not 0/2, D not 0, callback nonzero still reject',
            },
            'limitations': [
                'Static original-byte slices only; no original EXE execution.',
                'Sound callback remains an external owner; mode0 boundary reports soundRequired when sound>=0.',
            ],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/mboss-extension16-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/mboss-extension16-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('MBoss record C0/D0 -> type16 mode0 updater 0x40ae90; R/current-angle boundary rule recorded')


if __name__ == '__main__':
    main()
