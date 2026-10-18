"""Prepare only local retail title-flow resources; never bundle them in Git.

The shared ANM decoder is reused without executing its game-asset export loop.
Ending and staff MSG records are retained losslessly as well as annotated with
their native 411420 meanings. This is extraction, not a redraw of retail art.
"""
import importlib.util
import hashlib
import json
import pathlib
import os
import shutil
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
ORIGINAL = pathlib.Path(os.environ.get('TH12_ORIGINAL', ROOT / 'local/original')).expanduser().resolve()
RETAIL = pathlib.Path(os.environ.get('TH12_RETAIL', ROOT / 'local/retail')).expanduser().resolve()
DEST = pathlib.Path(os.environ.get('TH12_ASSETS', ROOT / 'site/assets')).expanduser().resolve()
PINNED_EXE_SHA256 = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'


def shared_anm_decoder():
    source_path = ROOT / 'scripts/prepare-assets.py'
    spec = importlib.util.spec_from_file_location('th12_anm_decoder', source_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    if not callable(getattr(module, 'anm', None)):
        raise ValueError('Shared ANM decoder is unavailable')
    return module.anm


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


def spell_names():
    """Read ECL spell-start records as bytes, without running their scripts."""
    result = {}
    for path in sorted(RETAIL.glob('*.ecl')):
        data = path.read_bytes()
        count = struct.unpack_from('<H', data, 16)[0]
        table = 36 + struct.unpack_from('<H', data, 6)[0]
        offsets = [struct.unpack_from('<I', data, table + sub * 4)[0] for sub in range(count)]
        for start in offsets:
            cursor = start + 16
            end = min([other for other in offsets if other > start] + [len(data)])
            while cursor + 16 <= end:
                opcode, length = struct.unpack_from('<HH', data, cursor + 4)
                if length < 16 or cursor + length > end:
                    break
                if opcode in (422, 428, 437, 438, 439) and length >= 33:
                    base = struct.unpack_from('<i', data, cursor + 16)[0]
                    size = struct.unpack_from('<i', data, cursor + 28)[0]
                    if 0 < size <= length - 32:
                        title = decrypt_text(data[cursor + 32:cursor + 32 + size])
                        for difficulty in ([4] if path.name.startswith('stage07') else range(4)):
                            if not data[cursor + 10] & (1 << difficulty):
                                continue
                            card = base + (difficulty if opcode == 437 else difficulty - 1 if opcode == 438 else difficulty - 2 if opcode == 439 else 0)
                            if 0 <= card <= 112:
                                result[str(card)] = {'title': title, 'byteLength': len(title.encode('cp932'))}
                cursor += length
    return result


def music_warning():
    """Read MusicRoom's eight warning-line pointers from the pinned PE image."""
    data = (ORIGINAL / 'th12.exe').read_bytes()
    if hashlib.sha256(data).hexdigest() != PINNED_EXE_SHA256:
        raise ValueError('Menu strings support the pinned TH12 1.00b executable')
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    count, optional_size = struct.unpack_from('<H', data, pe + 6)[0], struct.unpack_from('<H', data, pe + 20)[0]
    image_base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
    sections = pe + 24 + optional_size

    def offset(address):
        rva = address - image_base
        for i in range(count):
            header = sections + i * 40
            size, start, raw_size, raw = struct.unpack_from('<IIII', data, header + 8)
            if start <= rva < start + max(size, raw_size):
                return raw + rva - start
        raise ValueError('Native Music Room string points outside the image')

    table = offset(0x4b33c0)
    lines = []
    for i in range(8):
        start = offset(struct.unpack_from('<I', data, table + i * 4)[0])
        lines.append(data[start:data.index(0, start)].decode('cp932'))
    return {'lines': lines, 'colorRGB': 0xff8080, 'nativeTable': '0x4b33c0'}


def add_menu_metadata(resource):
    resource['spellNames'] = spell_names()
    resource['musicWarning'] = music_warning()
    return resource


def prepare_loading_metadata(resource):
    #Small bootstrap manifest: the original splash/loading atlas can draw
    #before title/stage textures, the Wasm runtime or sound data are ready.
    atlas = json.loads((DEST / 'atlases.json').read_text('utf-8'))['ascii']
    output = {'schema': 'th12-loading-resources/1', 'atlases': {'ascii': atlas, 'sig': resource['atlases']['sig']},
              'originalExeSha256': PINNED_EXE_SHA256, 'localOnly': True}
    (DEST / 'loading-resources.json').write_text(json.dumps(output, ensure_ascii=False, separators=(',', ':'), allow_nan=False), 'utf-8')


def main():
    DEST.mkdir(parents=True, exist_ok=True)
    if '--metadata-only' in sys.argv:
        target = DEST / 'menu-resources.json'
        resource = add_menu_metadata(json.loads(target.read_text('utf-8')))
        target.write_text(json.dumps(resource, ensure_ascii=False, separators=(',', ':'), allow_nan=False), 'utf-8')
        prepare_loading_metadata(resource)
        print('Prepared local spell-name and Music Room warning metadata.', flush=True)
        return
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
    parsed = subprocess.run([os.environ.get('TH12_NODE', 'node'), '--input-type=module', '-e',
        "import fs from 'node:fs';import path from 'node:path';import {parseReplay} from './src/replay.mjs';"
        "console.log(JSON.stringify(Array.from({length:4},(_,i)=>{"
        "const r=parseReplay(fs.readFileSync(path.join(process.env.TH12_RETAIL,'demo'+i+'.rpy')));"
        "return {id:i,file:'demo'+i+'.rpy',character:r.character,shot:r.shot,difficulty:r.difficulty,"
        "stage:r.stages[0].number,frames:r.stages[0].frames};})));"],
        cwd=ROOT, env={**os.environ, 'TH12_RETAIL': str(RETAIL)}, check=True, capture_output=True, text=True)
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
    add_menu_metadata(resource)
    prepare_loading_metadata(resource)
    (DEST / 'menu-resources.json').write_text(json.dumps(resource, ensure_ascii=False, separators=(',', ':'), allow_nan=False), 'utf-8')
    print(f'Prepared {len(resource["musicComments"])} music comments, 12 endings, credits and four retail demos.', flush=True)


if __name__ == '__main__':
    main()
