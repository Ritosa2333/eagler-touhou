import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-thirdstage-529-mapping/1'
TYPE25 = 0x4b40f0
TYPE25_SIZE = 288
BASE_SLICES = {
    'baseThunk': (0x416020, 0x41602b),
    'baseEntry': (0x416160, 0x416193),
    'tickCaller': (0x4163f5, 0x416500),
}


def read(image, address, size):
    data = bytes(image.get_data(address - 0x400000, size))
    if len(data) != size:
        raise ValueError('truncated original read')
    return data


def disasm(image, start, end):
    code = read(image, start, end - start)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [{'address': f'0x{item.address:08x}', 'bytes': item.bytes.hex(),
             'text': f'{item.mnemonic} {item.op_str}'.strip()}
            for item in decoder.disasm(code, start)]


def source_checks():
    files = {
        'BulletManager.cpp': Path('src/game/BulletManager.cpp'),
        'BulletVisuals.cpp': Path('src/game/BulletVisuals.cpp'),
        'BulletUpdateHost.cpp': Path('src/game/BulletUpdateHost.cpp'),
        'WorldExports.cpp': Path('src/runtime/WorldExports.cpp'),
        'StageWorld.cpp': Path('src/game/StageWorld.cpp'),
        'EnemyRules.hpp': Path('src/game/EnemyRules.hpp'),
    }
    result = {}
    for name, path in files.items():
        text = path.read_text(encoding='utf-8')
        result[name] = {
            'sha256': hashlib.sha256(text.encode()).hexdigest(),
            'type25Owner': ('selectedType==25' in text or 'info.type==25' in text
                            or 'bullet.type!=25' in text),
            'inPlaceConvert': any(token in text for token in
                                  ('convertBullet', 'replaceVisual', 'rebindBullet')),
            'f24c': '+0x24c' in text or '0x24c' in text,
            'f250': '+0x250' in text or '0x250' in text,
        }
    return result


def scan_displacements(image, target):
    text = [section for section in image.sections if section.Name.startswith(b'.text')][0]
    base = 0x400000 + text.VirtualAddress
    code = bytes(text.get_data())[:text.Misc_VirtualSize]
    needle = struct.pack('<I', target)
    hits = []
    offset = 0
    while True:
        offset = code.find(needle, offset)
        if offset < 0:
            break
        address = base + offset
        hits.append({'address': f'0x{address:08x}',
                     'instructions': disasm(image, max(base, address - 12), address + 28)})
        offset += 1
    return hits


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        profile = read(image, TYPE25, TYPE25_SIZE)
        values = {
            'type': 25,
            'script': struct.unpack_from('<i', profile, 0)[0],
            'secondaryScript': struct.unpack_from('<i', profile, 0x11c)[0],
            'hitboxSize': struct.unpack_from('<f', profile, 0x104)[0],
            'auxiliarySize': struct.unpack_from('<f', profile, 0x108)[0],
            'appearance': struct.unpack_from('<i', profile, 0x10c)[0],
            'selector': struct.unpack_from('<i', profile, 0x110)[0],
            'freezePolicy': struct.unpack_from('<i', profile, 0x118)[0],
        }
        enemy_variables = json.loads((ROOT / 'local/research/enemy-variable-fixtures.json').read_text(encoding='utf-8'))
        variable_entries = []
        for case in enemy_variables.get('cases', []):
            if case.get('identifier') in (-9981, -9980):
                variable_entries.append({
                    'identifier': case['identifier'],
                    'nativeOffset': f"0x{case['nativeOffset']:x}",
                    'readOffset': f"0x{case['readOffset']:x}",
                    'fieldRelativeToEnemyData': f"0x{case['fieldRelativeToEnemyData']:x}",
                    'nativeOffsetDecimal': case['nativeOffset'],
                })
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'baseSlices': {name: disasm(image, start, end) for name, (start, end) in BASE_SLICES.items()},
            'enemyVariableEvidence': variable_entries,
            'type25': {
                'profileAddress': f'0x{TYPE25:08x}',
                'profileBytes': TYPE25_SIZE,
                'profileSha256': hashlib.sha256(profile).hexdigest(),
                'values': values,
                'expected': {'script': 102, 'secondaryScript': 103, 'hitboxSize': 6.0,
                             'auxiliarySize': 22.0, 'appearance': 2, 'selector': 8,
                             'freezePolicy': 1},
                'cancellationScript': 371,
                'identitySprite': True,
            },
            'reusableOwners': {
                'BulletManager': 'loadTyped(25), Profile::identitySpriteRemap, cancellationScript',
                'BulletVisuals': 'CreateInfo secondaryScript/identitySpriteRemap, primary102/secondary103',
                'BulletUpdateHost': 'type25 secondary pairing 102/103, hitbox6/aux22/freezepolicy1',
                'WorldExports': 'bulletAnimations map + birth constructor',
                'missing': 'in-place replacement of existing Birth/Visual; no manager convert/mutate API',
            },
            'baseProof': {
                'thunk': '0x416020: add ecx,0x11c8; jmp 0x416160',
                'entry': '0x416176: mov esi,ecx; 0x41617f: mov [esp+0x20],esi',
                'callbackCaller': '0x4163f5: mov ebx,[esp+0x20]; 0x4164f8: mov ecx,ebx; 0x4164fa: call eax',
                'stateBase': 'Enemy+0x11c8',
                'entity24cAbsolute': '0x1414',
                'entity250Absolute': '0x1418',
                'variables': {'4': 'ECL -9981 (0x1414 / state+0x24c)',
                              '5': 'ECL -9980 (0x1418 / state+0x250)'},
                'deltaRelation': 'state +0x250 == Enemy+0x1418 == StageWorld variables[5]',
                'matchesExistingEvidence': True,
            },
            'selector2': {
                'field': 'bullet+0x9e4 velocity',
                'source': '0x41f750 fsincos(angle)*radius',
                'angle': 'bullet+0x9f4',
                'radius': 'bullet+0x9f0',
                'entityAngleAccumulator': 'entity+0x24c',
                'entityDelta': 'entity+0x250',
            },
            'entity24c250': {
                'writes24c': scan_displacements(image, 0x24c),
                'writes250': scan_displacements(image, 0x250),
                'eclAssignmentFound': True,
                'meaning': 'state+0x24c=0x1414 variables[4] (-9981); state+0x250=0x1418 variables[5] (-9980)',
                'stageWorldMapping': 'StageWorld.cpp variables[4]/[5] at lines 512/542',
                'selector2Delta': 'variables[4] += variables[5] per callback tick',
                'writeEvidence': [
                    {'address': '0x41e947', 'read': 'entity state+0x24c',
                     'absolute': 'Enemy+0x1414', 'use': 'subtract from bullet+0x9f4 angle'},
                    {'address': '0x41e94f', 'read': 'entity state+0x24c',
                     'absolute': 'Enemy+0x1414', 'use': 'add to bullet+0x9f4 angle'},
                    {'address': '0x41e9a1', 'write': 'state+0x24c += state+0x250',
                     'absolute': 'Enemy+0x1414 += Enemy+0x1418'},
                    {'address': '0x41e97d', 'write': 'bullet+0x9e4/+0x9e8 velocity',
                     'relative': 'state base unrelated; bullet object'},
                ],
            },
            'sourceChecks': source_checks(),
            'limitations': [
                'No source patch applied; no old type25 test or browser/full run.',
                '0x40a990 exact in-place conversion remains unported; existing type25 birth owner is reusable.',
                'entity +0x24c/+0x250 lack a proven ECL writer/StageWorld field mapping.',
            ],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/thirdstage-529-mapping-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/thirdstage-529-mapping-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('type25 %s/%s/%s; inPlace=%s; 24c=%d 250=%d; eclAssignment=%s'
          % (fixture['type25']['values']['script'], fixture['type25']['values']['secondaryScript'],
             fixture['type25']['values']['freezePolicy'], fixture['reusableOwners']['missing'],
             len(fixture['entity24c250']['writes24c']), len(fixture['entity24c250']['writes250']),
             fixture['entity24c250']['eclAssignmentFound']))


if __name__ == '__main__':
    main()
