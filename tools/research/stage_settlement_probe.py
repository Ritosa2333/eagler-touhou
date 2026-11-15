import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-stage-settlement-fixtures/1'

SLICES = {
    'stageClearPrologue': (0x424fb0, 0x42507e),
    'slotClearPrologue': (0x41fe20, 0x41ffbc),
    'scoreAccumulate': (0x40e4e0, 0x40e516),
}

ANCHORS = {
    'stageClearPrologue': (
        'push 0x52',
        'push 0x17',
        'push 0',
        'call 0x4670b0',
        'mov dword ptr [esi + 0x58cc], ecx',
        'mov ecx, dword ptr [edx*4 + 0x4b7220]',
        'mov dword ptr [esi + 0x5a38], ecx',
        'mov dword ptr [0x4b4cc4], eax',
        'cmp eax, 0x3b9aca00',
        'mov dword ptr [0x4b4cc4], 0x3b9ac9ff',
        'or dword ptr [esi + 0x5a04], 0x100',
        'mov dword ptr [esi + 0x5a08], 0xfff0bdc1',
        'mov dword ptr [esi + 0x5a14], 0x4b6f58',
        'mov dword ptr [esi + 0x5a08], 0xffffffff',
    ),
    'slotClearPrologue': (
        'or dword ptr [ebx + 0x438ec], 0x10',
        'add ebx, 0x8868',
        'mov dword ptr [ebp - 4], 8',
        'mov ecx, dword ptr [ebx - 4]',
        'mov ecx, dword ptr [ebx]',
        'mov eax, dword ptr [esi + 0x4c8]',
        'call eax',
        'mov word ptr [esi + 0x3f8], ax',
        'cmp dword ptr [esi + 0x18], 0',
        'mov esi, dword ptr [esi + 0x14]',
        'mov esi, dword ptr [esi + 4]',
        'add ebx, 0xe4',
        'dec dword ptr [ebp - 4]',
        'jne 0x41fe50',
        'mov dword ptr [edx + 0x438f0], 0',
    ),
    'scoreAccumulate': (
        'imul dword ptr [ebp + 8]',
        'mov dword ptr [0x4b4cc4], eax',
        'cmp eax, 0x3b9aca00',
        'mov dword ptr [0x4b4cc4], 0x3b9ac9ff',
    ),
}

SAFE_ORDINARY_STAGES = [1, 2, 4, 6, 7, 9, 11, 12, 14]
ROUTE_FINAL_STAGES = [3, 5, 8, 10, 13, 15]
EXTRA_STAGE = 16


def address(value):
    return f'0x{value:08x}'


def read(image, start, size):
    if type(size) is not int or not 0 < size <= 65536:
        raise ValueError('bounded read size')
    result = bytes(image.get_data(start - 0x400000, size))
    if len(result) != size:
        raise ValueError('truncated original read')
    return result


def decode(image, start, end):
    code = read(image, start, end - start)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    instructions = list(decoder.disasm(code, start))
    if sum(item.size for item in instructions) != len(code):
        raise ValueError('undecodable original slice')
    return code, [{'address': address(item.address), 'bytes': item.bytes.hex(),
                   'text': f'{item.mnemonic} {item.op_str}'.strip()} for item in instructions]


def require_anchors(name, instructions):
    text = '\n'.join(item['text'] for item in instructions)
    missing = [anchor for anchor in ANCHORS[name] if anchor not in text]
    if missing:
        raise ValueError(f'{name} missing original anchors: {missing}')


def table_rows(table):
    return [{'stage': stage, 'base': base, 'scoreAdd': base // 10}
            for stage, base in enumerate(table)]


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        slices = {}
        for name, (start, end) in SLICES.items():
            code, instructions = decode(image, start, end)
            require_anchors(name, instructions)
            slices[name] = {'start': address(start), 'endExclusive': address(end),
                            'sha256': hashlib.sha256(code).hexdigest(),
                            'instructions': instructions}
        tableBytes = read(image, 0x4b7220, 96)
        table = list(struct.unpack('<24i', tableBytes))
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'apiReturnsAssigned': False,
            'slices': slices,
            'spawn': {
                'callAddress': address(0x4670b0),
                'anmManagerGlobal': address(0x4b8950),
                'anmManagerOffset': 0x5a34,
                'anmId': 0x52,
                'layer': 0x17,
                'flags': 0,
                'handleDestination': 0x58cc,
            },
            'score': {
                'tableAddress': address(0x4b7220),
                'tableBytes': tableBytes.hex(),
                'tableSha256': hashlib.sha256(tableBytes).hexdigest(),
                'stageBaseScoreTable': table,
                'rows': table_rows(table),
                'cap': 999999999,
                'clampThreshold': 1000000000,
            },
            'manager': {
                'flags': 0x5a04,
                'baseScore': 0x5a38,
                'handle': 0x58cc,
                'field5a08': 0x5a08,
                'field5a0c': 0x5a0c,
                'field5a10': 0x5a10,
                'field5a14': 0x5a14,
                'field5a18': 0x5a18,
                'flagsStageClearMask': 0x100,
            },
            'ownership': {
                'referenceBase': address(0x4b8a80),
                'slotListBaseOffset': 0x8868,
                'groupCount': 8,
                'groupStride': 0xe4,
                'slotOffsets': [-4, 0],
                'slotCount': 2,
                'slotFlagOffset': 0x438ec,
                'slotFlagMask': 0x10,
                'slotFinishOffset': 0x438f0,
                'slotFinishValue': 0,
                'entityStateOffset': 0x3f8,
                'entityStateValue': 3,
                'callbackOffset': 0x4c8,
                'callbackArgument': 3,
                'chainNextOffset': 0x14,
                'chainSiblingOffset': 0x4,
                'chainStopOffset': 0x18,
            },
            'stageBounds': {
                'safeOrdinaryStages': SAFE_ORDINARY_STAGES,
                'routeFinalStages': ROUTE_FINAL_STAGES,
                'extraStage': EXTRA_STAGE,
                'minimum': 0,
                'maximum': 23,
            },
            'callBoundaries': {
                'stageClearPrologue': address(0x424fb0),
                'slotClearPrologue': address(0x41fe20),
                'scoreAccumulate': address(0x40e4e0),
                'anmSpawn': address(0x4670b0),
                'notOwned': [address(0x411c70), address(0x4273a0)],
            },
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/stage-settlement-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = Path(arguments.out).resolve()
    expected = (ROOT / 'local/research').resolve()
    if destination.parent != expected or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/stage-settlement-fixtures.json')
    fixture = probe(Path(arguments.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('stage settlement slices %d, stage table %d rows, safe ordinary stages %s'
          % (len(fixture['slices']), len(fixture['score']['stageBaseScoreTable']),
             fixture['stageBounds']['safeOrdinaryStages']))


if __name__ == '__main__':
    main()
