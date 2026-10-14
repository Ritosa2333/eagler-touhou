"""Prepare only local retail title-flow resources; never bundle them in Git.

The shared ANM decoder is reused without executing its game-asset export loop.
Ending and staff MSG records are retained losslessly as well as annotated with
their native 411420 meanings. This is extraction, not a redraw of retail art.
"""
import ast
import hashlib
import json
import pathlib
import shutil
import struct

ROOT = pathlib.Path(__file__).resolve().parents[1]
RETAIL = ROOT / 'local/retail'
DEST = ROOT / 'site/assets'
PINNED_EXE_SHA256 = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'


def shared_anm_decoder():
    source_path = ROOT / 'scripts/prepare-assets.py'
    tree = ast.parse(source_path.read_text('utf-8'), filename=str(source_path))
    definitions = []
    for node in tree.body:
        if isinstance(node, ast.Assign) and any(isinstance(t, ast.Name) and t.id == 'chosen' for t in node.targets):
            break
        definitions.append(node)
    else:
        raise ValueError('Shared asset decoder export boundary changed')
    scope = {'__file__': str(source_path), '__name__': 'menu_asset_decoder'}
    exec(compile(ast.Module(body=definitions, type_ignores=[]), str(source_path), 'exec'), scope)
    if not callable(scope.get('anm')):
        raise ValueError('Shared ANM decoder is unavailable')
    return scope['anm']


def decrypt_text(payload):
    key, step, output = 0x77, 7, bytearray()
    for value in payload:
        decoded = value ^ key
        key = (key + step) & 255
        step = (step + 0x10) & 255
        if not decoded:
            # Browser TextDecoder('shift-jis') and native Windows map815f/
            # 8160 through CP932; Python's strict JIS codec differs for waves.
            return output.decode('cp932')
        output.append(decoded)
    raise ValueError('Ending text is missing its encrypted terminator')


def ending_message(path):
    data = path.read_bytes()
    if len(data) < 12:
        raise ValueError(f'{path.name}: incomplete MSG header')
    count = struct.unpack_from('<I', data)[0]
    if count != 1 or struct.unpack_from('<I', data, 4)[0] != 12:
        raise ValueError(f'{path.name}: unsupported ending entry table')
    commands, cursor, terminated = [], 12, False
    while cursor + 4 <= len(data):
        time, opcode, length = struct.unpack_from('<HBB', data, cursor)
        if cursor + 4 + length > len(data):
            raise ValueError(f'{path.name}: MSG command exceeds file')
        payload = data[cursor + 4:cursor + 4 + length]
        record = {'time': time, 'op': opcode, 'offset': cursor, 'payloadHex': payload.hex()}
        if len(payload) % 4 == 0:
            record['ints'] = list(struct.unpack('<' + 'i' * (len(payload) // 4), payload))
        if opcode == 3:
            record.update(kind='text', text=decrypt_text(payload))
        elif opcode == 7:
            record.update(kind='load-bank', bankSlot=struct.unpack_from('<i', payload)[0],
                          file=payload[4:].split(b'\0')[0].decode('ascii'))
        elif opcode in (8, 15, 16, 17):
            slot, bank_slot, script = struct.unpack('<iii', payload)
            record.update(kind='animation', slot=slot, bankSlot=bank_slot, script=script)
            if opcode >= 15:
                record['difficulty'] = opcode - 14  # 4b0ca8, Normal/Hard/Lunatic.
        elif opcode in (5, 6):
            record.update(kind='wait', frames=struct.unpack('<i', payload)[0], clearText=opcode == 6)
        elif opcode == 9:
            record.update(kind='text-color', colorRGB=struct.unpack('<I', payload)[0])
        elif opcode == 10:
            file = payload.split(b'\0')[0].decode('ascii')
            record.update(kind='music', file=file, track=pathlib.PurePosixPath(file).stem)
        elif opcode == 12:
            record.update(kind='next-message', file=payload.split(b'\0')[0].decode('ascii'))
        elif opcode in (13, 14):
            record.update(kind='fade', frames=struct.unpack('<i', payload)[0], fadeOut=opcode == 14)
        else:
            record['kind'] = {0: 'end', 4: 'hide-text', 11: 'fade-music'}.get(opcode, 'native-command')
        commands.append(record)
        cursor += length + 4
        if opcode == 0:
            terminated = True
            break
    if not terminated or any(data[cursor:]):
        raise ValueError(f'{path.name}: unsupported MSG termination')
    return {'file': path.name, 'bank': path.stem, 'commands': commands,
            'sha256': hashlib.sha256(data).hexdigest(), 'nativeInterpreter': '0x411420'}


def music_comments(path):
    lines, result, cursor = path.read_bytes().decode('cp932').splitlines(), [], 0
    while cursor < len(lines):
        line = lines[cursor]
        cursor += 1
        if not line.startswith('@'):
            continue
        file = line[1:]
        if cursor + 9 > len(lines):
            raise ValueError('Incomplete Music Room comment record')
        label, body = lines[cursor], lines[cursor + 1:cursor + 9]
        cursor += 9
        if not label.startswith('No.') or not body[0].startswith('♪'):
            raise ValueError('Unsupported Music Room comment record')
        result.append({'index': len(result), 'id': pathlib.PurePosixPath(file).stem,
                       'file': file, 'label': label, 'title': body[0][1:],
                       'lines': body, 'comment': '\n'.join(body),
                       'nativeParser': '0x449444..0x449489'})
    return result


def main():
    DEST.mkdir(parents=True, exist_ok=True)
    decoder = shared_anm_decoder()
    atlases = {}
    for bank in ['sig', 'staff', *[f'e{i:02d}' for i in range(12)]]:
        atlases[bank] = decoder(RETAIL / (bank + '.anm'))
        print(f'{bank}: {len(atlases[bank]["entries"])} textures, {len(atlases[bank]["scripts"])} scripts', flush=True)
    endings = [dict(ending_message(RETAIL / f'e{i:02d}.msg'), id=i,
                    character=(i % 6) // 2, shot=i % 2, continued=i < 6,
                    endingNumber=i + 7 if i < 6 else i - 5) for i in range(12)]
    credits = ending_message(RETAIL / 'staff.msg')
    # Use the checked replay decoder rather than duplicating its compression.
    import subprocess
    parsed = subprocess.run(['node', '--input-type=module', '-e',
        "import fs from 'node:fs';import {parseReplay} from './src/replay.mjs';"
        "console.log(JSON.stringify(Array.from({length:4},(_,i)=>{"
        "const r=parseReplay(fs.readFileSync('local/retail/demo'+i+'.rpy'));"
        "return {id:i,file:'demo'+i+'.rpy',character:r.character,shot:r.shot,difficulty:r.difficulty,"
        "stage:r.stages[0].number,frames:r.stages[0].frames};})));"],
        cwd=ROOT, check=True, capture_output=True, text=True)
    demos = json.loads(parsed.stdout)
    for item in demos:
        source = RETAIL / item['file']
        shutil.copyfile(source, DEST / item['file'])
        item['sha256'] = hashlib.sha256(source.read_bytes()).hexdigest()
    resource = {'schema': 'th12-menu-resources/1', 'originalExeSha256': PINNED_EXE_SHA256,
                'atlasDependencies': ['title', 'title_v', 'ascii'], 'atlases': atlases,
                'musicComments': music_comments(RETAIL / 'musiccmt.txt'),
                'endings': endings, 'credits': credits, 'demos': demos,
                'manual': {'available': False, 'pages': [],
                           'reason': 'No original manual document is present in supplied retail files.'},
                'provenance': {'originalResourcePayload': True, 'localOnly': True,
                               'wholePresentationOracle': False}}
    (DEST / 'menu-resources.json').write_text(json.dumps(resource, ensure_ascii=False, separators=(',', ':'), allow_nan=False), 'utf-8')
    print(f'Prepared {len(resource["musicComments"])} music comments, 12 endings, credits and four retail demos.', flush=True)


if __name__ == '__main__':
    main()
