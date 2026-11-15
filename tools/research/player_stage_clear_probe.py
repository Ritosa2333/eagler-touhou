import argparse
import hashlib
import json
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-player-stage-clear-fixtures/1'

SLICES = {
    'stageClearSlots': (0x41fe20, 0x41ffbc),
    'normalInitSlots': (0x43a180, 0x43a30e),
    'flagCounter': (0x43b3b7, 0x43b3c6),
    'counterLimit': (0x43b4a0, 0x43b4ad),
    'playerUpdateCall': (0x43b7e0, 0x43b7f3),
    'normalInitCalls': (0x43ac40, 0x43ac56),
    'createSecondHandles': (0x43d3dc, 0x43d4f3),
    'createFirstHandles': (0x43d554, 0x43d6b6),
    'storeFirstHandle': (0x43da6b, 0x43da7b),
    'handleGetter': (0x43e4f0, 0x43e500),
    'deathSlotsCallback': (0x43cebe, 0x43cf58),
}

ANCHORS = {
    'stageClearSlots': (
        'or dword ptr [ebx + 0x438ec], 0x10',
        'add ebx, 0x8868',
        'mov dword ptr [ebp - 4], 8',
        'mov edx, 3',
        'mov word ptr [esi + 0x3f8], ax',
        'mov dword ptr [edx + 0x438f0], 0',
    ),
    'normalInitSlots': (
        'and dword ptr [ebx + 0x438ec], 0xffffffef',
        'mov edx, 2',
        'mov word ptr [esi + 0x3f8], ax',
        'mov dword ptr [edx + 0x438f0], 0',
    ),
    'flagCounter': (
        'test byte ptr [esi + 0x438ec], 0x10',
        'inc dword ptr [esi + 0x438f0]',
    ),
    'counterLimit': ('cmp dword ptr [esi + 0x438f0], 0x1e',),
    'playerUpdateCall': ('call 0x43d380',),
    'normalInitCalls': ('call 0x43a180', 'call 0x43d380'),
    'createSecondHandles': (
        'lea esi, [edi + 0x8868]',
        'call 0x41f2a0',
        'mov dword ptr [esi], eax',
        'add esi, 0xe4',
    ),
    'createFirstHandles': (
        'lea esi, [edi + 0x8824]',
        'call 0x4670b0',
    ),
    'storeFirstHandle': ('mov dword ptr [esi + 0x40], ecx',),
    'handleGetter': (
        'imul ecx, ecx, 0xe4',
        'mov ecx, dword ptr [ecx + edx + 0x8864]',
        'ret',
    ),
    'deathSlotsCallback': (
        'mov edx, 1',
        'mov word ptr [edi + 0x3f8], ax',
        'call eax',
    ),
}


def address(value):
    return f'0x{value:08x}'


def read(image, start, size):
    if type(size) is not int or not 0 < size <= 65536:
        raise ValueError('bounded read size')
    result = bytes(image.get_data(start - 0x400000, size))
    if len(result) != size:
        raise ValueError('truncated original read')
    return result


def decode(image, start, end, name):
    code = read(image, start, end - start)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    instructions = list(decoder.disasm(code, start))
    if sum(item.size for item in instructions) != len(code):
        raise ValueError(f'undecodable original slice {name}')
    return code, [{'address': address(item.address), 'bytes': item.bytes.hex(),
                   'text': f'{item.mnemonic} {item.op_str}'.strip()} for item in instructions]


def require_anchors(name, instructions):
    text = '\n'.join(item['text'] for item in instructions)
    missing = [anchor for anchor in ANCHORS[name] if anchor not in text]
    if missing:
        raise ValueError(f'{name} missing original anchors: {missing}')


def source_fact(path):
    data = (ROOT / path).read_bytes()
    return {'path': path, 'sha256': hashlib.sha256(data).hexdigest(),
            'bytes': len(data), 'text': data.decode('utf-8', errors='replace')}


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        slices = {}
        for name, (start, end) in SLICES.items():
            code, instructions = decode(image, start, end, name)
            require_anchors(name, instructions)
            slices[name] = {'start': address(start), 'endExclusive': address(end),
                            'sha256': hashlib.sha256(code).hexdigest(),
                            'instructions': instructions}
        player_shot = source_fact('src/game/PlayerShotHost.cpp')
        player_shot_header = source_fact('src/game/PlayerShotHost.hpp')
        player_birth_header = source_fact('src/game/PlayerShotBirthRules.hpp')
        player_hit = source_fact('src/game/PlayerHitHost.cpp')
        player_slots = source_fact('src/game/PlayerStageClearSlots.cpp')
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'apiReturnsAssigned': False,
            'slices': slices,
            'constants': {
                'playerGlobal': address(0x4b8a80),
                'anmRegistry': address(0x4d2e50),
                'anmRegistryLists': [address(0x8b9704), address(0x8b970c)],
                'playerFlagOffset': 0x438ec,
                'playerStageClearMask': 0x10,
                'playerStageClearCounter': 0x438f0,
                'playerStageClearReset': 0,
                'playerStageClearLimit': 0x1e,
                'playerShotCount': 0x438f8,
                'handleBaseOffsets': [0x8864, 0x8868],
                'groupCount': 8,
                'groupStride': 0xe4,
                'slotsPerGroup': 2,
                'callbackOffset': 0x4c8,
                'entityStateOffset': 0x3f8,
                'stageClearCallbackArgument': 3,
                'normalInitCallbackArgument': 2,
                'deathCallbackArgument': 1,
                'chainNextOffset': 0x14,
                'chainSiblingOffset': 4,
                'chainStopOffset': 0x18,
            },
            'handleOwners': {
                'secondSlot': {
                    'function': address(0x43d380),
                    'groupBase': '+0x8868',
                    'allocator': address(0x41f2a0),
                    'observedScriptImmediates': [0xe, 0xf, 0xd, 0x11, 0x12],
                    'storeSites': [address(0x43d495), address(0x43d4af),
                                   address(0x43d4df)],
                },
                'firstSlot': {
                    'function': address(0x43d380),
                    'groupBase': '+0x8864',
                    'allocator': address(0x4670b0),
                    'observedScriptImmediates': [0xb, 0xc, 0xf, 0x10],
                    'observedLayer': 0xc,
                    'observedFlags': 0,
                    'storeSite': address(0x43da71),
                },
                'ordinaryUpdateCallSite': address(0x43b7ee),
                'normalInitCallSite': address(0x43ac51),
                'normalInitClearCallSite': address(0x43ac4c),
                'deathCleanupCallSite': address(0x43b84e),
            },
            'currentModel': {
                'playerShotHost': {
                    'path': player_shot['path'],
                    'sha256': player_shot['sha256'],
                    'containsPlayerHandleOffsets': ('0x8864' in player_shot['text']
                                                    or '0x8868' in player_shot['text']),
                    'usesSyntheticVisualHandle': 'nextVisualHandle_' in player_shot['text'],
                    'writesShotImageHandleAt4c': '0x4c' in player_shot['text'],
                },
                'playerShotHostHeader': {
                    'path': player_shot_header['path'],
                    'sha256': player_shot_header['sha256'],
                    'shotSlotCount': 256 if 'ShotSlotCount' in player_shot_header['text'] else None,
                },
                'playerShotBirthHeader': {
                    'path': player_birth_header['path'],
                    'sha256': player_birth_header['sha256'],
                    'shotSlotCount': 256,
                    'shotSlotBytes': 0x78,
                },
                'playerHitHost': {
                    'path': player_hit['path'],
                    'sha256': player_hit['sha256'],
                    'hasStageClearFlagCounterOwner': ('applyStageClearStart' in player_hit['text']
                                                      and 'stageClearCounter438f0' in player_hit['text']),
                },
                'playerStageClearSlots': {
                    'path': player_slots['path'],
                    'sha256': player_slots['sha256'],
                    'requiresExplicitEmptyDeclaration': 'explicitEmpty' in player_slots['text'],
                },
                'gap': ('PlayerShotHost owns 256 independent shot VMs with synthetic '
                        'visualHandle values; it does not write Player+0x8864/+0x8868. '
                        'The original ordinary update 0x43d380 creates those two handle '
                        'slots per active group. Integration must explicitly configure '
                        'actual VMs/handles, not assume the 256-shot pool is the 8x2 set.'),
            },
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/player-stage-clear-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = Path(arguments.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/player-stage-clear-fixtures.json')
    fixture = probe(Path(arguments.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('player stage-clear slices %d, groups %d, handles %s, current PlayerShotHost writes slots %s'
          % (len(fixture['slices']), fixture['constants']['groupCount'],
             fixture['constants']['handleBaseOffsets'],
             fixture['currentModel']['playerShotHost']['containsPlayerHandleOffsets']))


if __name__ == '__main__':
    main()
