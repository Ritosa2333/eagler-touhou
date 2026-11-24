#!/usr/bin/env python3
"""Read-only type25 cancellation evidence: bullet.anm programs 371/370."""
import argparse
import hashlib
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from th128_assets.formats import parse_anm  # noqa: E402

RNG_FLOAT = {10010, 10011, 10012}
RNG_INT = {10022}


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def command_record(command):
    raw = bytes.fromhex(command['raw'])
    words = [struct.unpack_from('<I', raw, offset)[0] for offset in range(0, len(raw) - 3, 4)]
    floats = [struct.unpack_from('<f', raw, offset)[0] for offset in range(0, len(raw) - 3, 4)]
    record = {'op': command['opcode'], 'time': command['time'], 'mask': command['mask'],
              'words': words, 'floats': [round(value, 6) for value in floats]}
    if command['opcode'] == 3:
        record['sprite'] = words[0]
    if command['opcode'] == 88:
        record['child'] = words[0]
    if command['opcode'] == 68:
        record['layer'] = words[0]
    return record


def rng_reads(commands):
    reads = []
    for index, command in enumerate(commands):
        for operand, word in enumerate(command['words']):
            if not command['mask'] & (1 << operand):
                continue
            value = word
            if command['mask'] & (1 << operand):
                try:
                    as_float = struct.unpack('<f', struct.pack('<I', word))[0]
                except (OverflowError, struct.error):
                    as_float = 0.0
                if command['op'] in range(6, 28):
                    value = int(as_float) if command['op'] & 1 else word
                else:
                    value = int(as_float)
            if value in RNG_FLOAT or value in RNG_INT:
                reads.append({'command': index, 'op': command['op'], 'operand': operand,
                              'builtin': value})
    return reads


def main():
    parser = argparse.ArgumentParser(description='Read-only type25 cancellation fixture probe')
    parser.add_argument('--output', type=pathlib.Path,
                        default=ROOT / 'local/research/type25-cancellation-fixtures.json')
    args = parser.parse_args()

    catalog = json.loads((ROOT / 'local/catalog.json').read_text(encoding='utf-8'))
    manifest_path = pathlib.Path(ROOT / 'local' / catalog['variants']['ja']['manifest'])
    variant = manifest_path.parent
    bullet_path = variant / 'raw/bullet.anm'
    metadata_path = variant / 'animations/bullet.json'
    profile_path = variant / 'animations/bullet-type25-profile.json'
    bullet = bullet_path.read_bytes()
    metadata = json.loads(metadata_path.read_text(encoding='utf-8'))
    parsed = parse_anm(bullet)
    manifest = json.loads((variant / 'manifest.json').read_text(encoding='utf-8'))
    raw_entry = next(entry for entry in manifest['files'] if entry['name'] == 'bullet.anm')
    animation_entry = next(entry for entry in manifest['animations'] if entry['name'] == 'bullet.anm')

    assert sha256(bullet) == raw_entry['sha256'] == metadata['provenance']['sha256']
    assert len(bullet) == raw_entry['bytes']
    assert animation_entry['file'] == 'animations/bullet.json'
    assert (len(parsed['entries']), len(parsed['sprites']), len(parsed['scripts'])) == (
        animation_entry['entries'], animation_entry['sprites'], animation_entry['scripts'])
    assert len(metadata['entries']) == len(parsed['entries'])
    assert len(metadata['sprites']) == len(parsed['sprites'])
    for index, command in enumerate(parsed['scripts'][371]['commands']):
        expected = metadata['scripts'][371]['commands'][index]
        assert (command['opcode'], command['time'], command['mask'], command['raw']) == (
            expected['opcode'], expected['time'], expected['mask'], expected['raw'])
    for index, command in enumerate(parsed['scripts'][370]['commands']):
        expected = metadata['scripts'][370]['commands'][index]
        assert (command['opcode'], command['time'], command['mask'], command['raw']) == (
            expected['opcode'], expected['time'], expected['mask'], expected['raw'])

    links = []
    for script in parsed['scripts']:
        for command in script['commands']:
            if command['opcode'] == 88:
                child = struct.unpack_from('<I', bytes.fromhex(command['raw']), 0)[0]
                if child in (370, 371):
                    links.append({'parent': script['index'], 'child': child})

    programs = {}
    for script_id in (371, 370):
        commands = [command_record(command) for command in parsed['scripts'][script_id]['commands']]
        opcodes = {}
        for command in commands:
            opcodes[str(command['op'])] = opcodes.get(str(command['op']), 0) + 1
        programs[str(script_id)] = {
            'entry': parsed['scripts'][script_id]['entry'],
            'commands': commands,
            'opcodes': opcodes,
            'children': [command['child'] for command in commands if 'child' in command],
            'sprites': [command['sprite'] for command in commands if 'sprite' in command],
            'layers': [command['layer'] for command in commands if 'layer' in command],
            'rngReads': rng_reads(commands),
        }

    sprites = []
    for sprite_id in range(580, 588):
        sprite = metadata['sprites'][sprite_id]
        entry = metadata['entries'][sprite['entry']]
        sprites.append({'index': sprite_id, 'entry': sprite['entry'], 'x': sprite['x'], 'y': sprite['y'],
                        'width': sprite['width'], 'height': sprite['height'],
                        'texture': entry['texture']['file'], 'textureSha256': entry['texture']['sha256'],
                        'textureWidth': entry['texture']['width'], 'textureHeight': entry['texture']['height']})
    scale_commands = []
    for index, command in enumerate(programs['370']['commands']):
        if command['op'] != 60:
            continue
        targets = [int(struct.unpack('<f', struct.pack('<I', word))[0]) for word in command['words'][2:4]]
        scale_commands.append({'command': index, 'mask': command['mask'], 'duration': command['words'][0],
                               'mode': command['words'][1], 'targetVariables': targets})
    def root_signature(script_id):
        signature = []
        for command in parsed['scripts'][script_id]['commands']:
            raw = bytes.fromhex(command['raw'])
            words = [struct.unpack_from('<I', raw, offset)[0] for offset in range(0, len(raw) - 3, 4)]
            if command['opcode'] == 88:
                words[0] = 370
            signature.append((command['opcode'], command['time'], command['mask'], tuple(words)))
        return signature

    root_signature_371 = root_signature(371)
    assert root_signature_371 == root_signature(375)
    assert root_signature_371 == root_signature(381)
    child_signatures = []
    for script_id in (370, 374, 380):
        child_signatures.append(tuple((command['opcode'], command['time'], command['mask'], command['raw'])
                                      for command in parsed['scripts'][script_id]['commands']))
    assert len(set(child_signatures)) == 3
    profile = json.loads(profile_path.read_text(encoding='utf-8'))
    fixture = {
        'schema': 'th128-type25-cancellation-fixtures/1',
        'sourceGeneration': manifest_path.parent.name,
        'bulletAnm': {'path': 'raw/bullet.anm', 'bytes': len(bullet), 'sha256': sha256(bullet)},
        'metadata': {'path': 'animations/bullet.json', 'sha256': sha256(metadata_path.read_bytes()),
                     'entries': len(parsed['entries']), 'sprites': len(parsed['sprites']),
                     'scripts': len(parsed['scripts'])},
        'profile': {key: profile[key] for key in ('type', 'script', 'secondaryScript', 'selector',
                                                  'cancellationScript', 'freezePolicy')},
        'programs': programs,
        'links': links,
        'siblingRoots': {'identicalInstructionFlow': [375, 381],
                         'childTargets': {'371': 370, '375': 374, '381': 380},
                         'childSignaturesDiffer': True},
        'sprites': sprites,
        'childScaleCommands': scale_commands,
        'lifecycle': {'root': 371, 'child': 370, 'children': 3, 'maxReservation': 4,
                      'rootWaitFrames': 40, 'childEndFrame': 40,
                      'zeroRng': {'rootClockAfterBind': -39, 'childClockAfterBind': 1,
                                  'birthRngCalls': 21, 'totalRngCalls': 24, 'retirePass': 40},
                      'rootSpawns': 'three op88 child370 with op26/op75 random delays',
                      'rootTerminates': 'op75 40 then op1', 'childTerminates': 'op1 at time 40'},
        'unsupportedSiblingRoots': [369, 373, 377, 379],
        'observedFailure': {'frame': 1170, 'script': 371, 'type': 25, 'color': 0,
                            'error': 'bullet cancellation script=371 type=25 color=0: '
                                     'unsupported cancellation script/color/pose/layer'},
    }
    args.output.write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf-8')
    print(f'wrote {args.output.relative_to(ROOT)} ({len(links)} links, {len(programs["371"]["commands"])}+'
          f'{len(programs["370"]["commands"])} commands)')


if __name__ == '__main__':
    main()
