"""Compile the supplied TH12c translation to a static, data-only language ZIP.

Requires Python 3.10+, Pillow and local thtk thdat. Only thdat is executed;
the two original executables are read as PE data, never loaded or run.
"""
import argparse
import hashlib
import io
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import tempfile
import zipfile

ROOT = Path(__file__).resolve().parents[1]
PREFIX = '/thcrap/th12/'
SPELL_OPS = {422, 428, 437, 438, 439}


def digest(data):
    return hashlib.sha256(data).hexdigest()


def json_bytes(value):
    return (json.dumps(value, ensure_ascii=False, sort_keys=True,
                       separators=(',', ':'), allow_nan=False) + '\n').encode('utf-8')


def u16(data, at):
    return struct.unpack_from('<H', data, at)[0]


def u32(data, at):
    return struct.unpack_from('<I', data, at)[0]


def plain_text(payload):
    key, step, output = 0x77, 7, bytearray()
    for value in payload:
        decoded = value ^ key
        key = (key + step) & 255
        step = (step + 16) & 255
        if not decoded:
            return bytes(output)
        output.append(decoded)
    raise ValueError('Encrypted text has no terminator')


def text_value(raw, encoding, formatted=False):
    result = {'text': raw.decode(encoding, errors='strict'), 'byteLength': len(raw)}
    if formatted and raw.startswith(b'|'):
        fields = raw[1:].split(b',', 2)
        if len(fields) != 3:
            raise ValueError('Unsupported formatted dialogue text')
        result.update(text=fields[2].decode(encoding, errors='strict'),
                      byteLength=len(fields[2]), offset=int(fields[0]), spacing=int(fields[1]))
        raw = fields[2]
    result['byteWidths'] = [len(character.encode(encoding)) for character in result['text']]
    return raw, result


def message_entries(data):
    if len(data) < 12:
        raise ValueError('Truncated MSG')
    count = u32(data, 0)
    if not 0 < count <= 256 or 4 + 8 * count > len(data):
        raise ValueError('Unsupported MSG table')
    entries = []
    for entry in range(count):
        start = u32(data, 4 + 8 * entry)
        end = u32(data, 12 + 8 * entry) if entry + 1 < count else len(data)
        if not 4 + 8 * count <= start < end <= len(data):
            raise ValueError('Invalid MSG entry range')
        commands, cursor = [], start
        while cursor + 4 <= end:
            time, op, size = struct.unpack_from('<HBB', data, cursor)
            if cursor + 4 + size > end:
                raise ValueError('MSG command exceeds entry')
            commands.append({'time': time, 'op': op, 'offset': cursor,
                             'payload': data[cursor + 4:cursor + 4 + size]})
            cursor += size + 4
            if op == 0:
                break
        if not commands or commands[-1]['op'] != 0:
            raise ValueError('MSG entry has no terminator')
        entries.append((u32(data, 8 + 8 * entry), commands))
    return entries


def message_overlay(original, translated, ending=False, omitted_times=(), time_mapping=None):
    old, new = message_entries(original), message_entries(translated)
    if len(old) != len(new):
        raise ValueError('Translation changed MSG entry count')
    texts, control_differences = {}, 0
    for entry, ((old_flags, a), (new_flags, b)) in enumerate(zip(old, new)):
        if ending:
            if old_flags != new_flags:
                raise ValueError('Translation changed ending entry flags')
            # TH12c e10 merges the two Kanako lines at times3/4 into one
            # translated line at3 and removes wait4. Retain the original wait
            # and blank only the explicitly recorded omitted Japanese line.
            translated_lines = {}
            for command in b:
                if command['op'] == 3:
                    translated_lines.setdefault(command['time'], []).append(command)
            for command in a:
                if command['op'] != 3:
                    continue
                translated_time = (time_mapping or {}).get(str(command['time']), command['time'])
                candidates = translated_lines.get(translated_time, [])
                if translated_time != command['time']:
                    control_differences += 1
                target = plain_text(candidates.pop(0)['payload']) if candidates else None
                if target is None and command['time'] not in omitted_times:
                    raise ValueError(f'Unmapped ending text at time {command["time"]}; source manifest needs explicit coverage')
                raw = plain_text(command['payload'])
                if raw == target:
                    continue
                _, value = text_value(target if target is not None else b'', 'cp936')
                texts[str(command['offset'])] = dict(value, sourceHex=raw.hex())
            if any(translated_lines.values()):
                raise ValueError('Translation has additional unmatched ending text')
            control_a = [(c['time'], c['op'], c['payload']) for c in a if c['op'] != 3]
            control_b = [(c['time'], c['op'], c['payload']) for c in b if c['op'] != 3]
            control_differences += sum(item not in control_b for item in control_a)
            control_differences += sum(item not in control_a for item in control_b)
            continue
        if old_flags != new_flags or len(a) != len(b) or [r['op'] for r in a] != [r['op'] for r in b]:
            raise ValueError('Cannot pair translated MSG command sequence')
        for left, right in zip(a, b):
            text_op = left['op'] == 3 if ending else left['op'] in (15, 16, 17)
            if left['time'] != right['time'] or (not text_op and left['payload'] != right['payload']):
                control_differences += 1
            if not text_op:
                continue
            raw, target = plain_text(left['payload']), plain_text(right['payload'])
            source_bytes, _ = text_value(raw, 'cp932', formatted=not ending and left['op'] == 17)
            _, value = text_value(target, 'cp936', formatted=not ending and right['op'] == 17)
            if raw == target:
                continue
            key = str(left['offset']) if ending else f'{entry}:{source_bytes.hex()}'
            if key in texts and texts[key] != value:
                raise ValueError('Ambiguous translated line within one MSG entry')
            texts[key] = dict(value, sourceHex=source_bytes.hex())
    return texts, control_differences


def dialogue_overlay(original, translated):
    """Pair non-text anchors; compile only presentation text, never controls.

    TH12c changes line counts. Rebuilding just the text commands lets the
    existing text VM own slot clearing/alternation and its animation handles.
    """
    old, new = message_entries(original), message_entries(translated)
    if len(old) != len(new):
        raise ValueError('Translation changed dialogue entry count')
    records, patches, ignored = {}, [], 0

    def groups(commands):
        result, pending = [], []
        for command in commands:
            if command['op'] in (15, 16, 17):
                pending.append(command)
            else:
                result.append((pending, command))
                pending = []
        if pending:
            raise ValueError('Dialogue text follows its terminator')
        return result

    for entry, ((old_flags, a), (new_flags, b)) in enumerate(zip(old, new)):
        if len(a) >= 4096 or len(b) >= 4096:
            raise ValueError('Dialogue entry must contain fewer than 4096 commands')
        ga, gb = groups(a), groups(b)
        if len(ga) != len(gb) or [c['op'] for _, c in ga] != [c['op'] for _, c in gb]:
            raise ValueError('Translation changed non-text dialogue command sequence')
        ignored += int(old_flags != new_flags)
        anchors = []
        for (source_texts, source_control), (translated_texts, translated_control) in zip(ga, gb):
            # Ctrl skip reads the next command's time directly. Retaining a
            # nonempty group at its one original time preserves that node.
            if bool(source_texts) != bool(translated_texts):
                raise ValueError('Translation inserted or removed a complete dialogue text group')
            source_time = source_texts[0]['time'] if source_texts else None
            if any(command['time'] != source_time for command in source_texts):
                raise ValueError('Original dialogue text group has multiple times')
            if any(command['time'] != source_time for command in translated_texts):
                raise ValueError('Translated dialogue text time differs from its original group')
            if source_control['time'] != translated_control['time'] or source_control['payload'] != translated_control['payload']:
                ignored += 1
            target = []
            for command in translated_texts:
                raw = plain_text(command['payload'])
                render_raw, value = text_value(raw, 'cp936', formatted=command['op'] == 17)
                utf8 = raw.decode('cp936').encode('utf-8')
                render_hex = render_raw.decode('cp936').encode('utf-8').hex()
                if len(utf8) + 1 > 252:
                    raise ValueError('Translated UTF-8 line exceeds the MSG text payload limit')
                # Source controls remain byte-for-byte Japanese. Every text
                # command keeps its group's checked original presentation time.
                value.update(sourceHex=render_hex, op=command['op'], utf8Hex=utf8.hex(), time=source_time)
                key = f'{entry}:{render_hex}'
                if key in records and records[key] != value:
                    # The same visible line may occur at several times. Those
                    # times belong to anchors, not to the rendering lookup.
                    prior = {k: v for k, v in records[key].items() if k not in ('time', 'op', 'utf8Hex')}
                    current = {k: v for k, v in value.items() if k not in ('time', 'op', 'utf8Hex')}
                    if prior != current:
                        raise ValueError('Ambiguous translated dialogue layout')
                records[key] = {k: v for k, v in value.items() if k not in ('time', 'op', 'utf8Hex')}
                target.append(value)
            anchors.append({'offset': source_control['offset'], 'texts': target})
        patches.append({'anchors': anchors})
    return records, {'sourceSha256': digest(original), 'entries': patches}, ignored


def spell_records(data, extra=False):
    if len(data) < 36 or data[:4] != b'SCPT':
        raise ValueError('Unsupported ECL container')
    count, table = u16(data, 16), 36 + u16(data, 6)
    if not 0 < count <= 8192 or table + count * 4 > len(data):
        raise ValueError('Invalid ECL subroutine table')
    offsets = [u32(data, table + i * 4) for i in range(count)]
    result = {}
    for start in offsets:
        end = min([other for other in offsets if other > start] + [len(data)])
        if start + 16 > end:
            raise ValueError('Invalid ECL subroutine range')
        cursor = start + 16
        while cursor + 16 <= end:
            op, size = struct.unpack_from('<HH', data, cursor + 4)
            if size < 16 or cursor + size > end:
                raise ValueError('Invalid ECL instruction size')
            if op in SPELL_OPS and size >= 33:
                base = struct.unpack_from('<i', data, cursor + 16)[0]
                length = struct.unpack_from('<i', data, cursor + 28)[0]
                if not 0 < length <= size - 32:
                    raise ValueError('Invalid spell title length')
                raw = plain_text(data[cursor + 32:cursor + 32 + length])
                # Normal stages are never loaded as Extra. Treating bit4 of
                # an all-ranks mask as another normal variant invents IDs.
                for difficulty in ([4] if extra else range(4)):
                    if not data[cursor + 10] & (1 << difficulty):
                        continue
                    card = base + (difficulty if op == 437 else difficulty - 1 if op == 438 else difficulty - 2 if op == 439 else 0)
                    if 0 <= card <= 112:
                        if card in result and result[card] != raw:
                            raise ValueError(f'Conflicting spell ID {card}')
                        result[card] = raw
            cursor += size
    return result


def texture_entries(data):
    base, entries = 0, []
    while True:
        if base + 64 > len(data) or u32(data, base) != 7:
            raise ValueError('Expected ANM v7')
        td, following = u32(data, base + 28), u32(data, base + 36)
        end = base + following if following else len(data)
        if not base + 64 <= end <= len(data):
            raise ValueError('Invalid ANM entry size')
        metadata = {'width': u16(data, base + 10), 'height': u16(data, base + 12),
                    'textureFormat': u16(data, base + 14)}
        if td and u16(data, base + 32):
            at = base + td
            if at + 16 > end or data[at:at + 4] != b'THTX':
                raise ValueError('Invalid THTX header')
            fmt, w, h, size = struct.unpack_from('<HHHI', data, at + 6)
            start, stop = at + 16, at + 16 + size
            if stop > end or not 0 < w <= 8192 or not 0 < h <= 8192:
                raise ValueError('Invalid THTX bounds')
            metadata.update(format=fmt, pixelWidth=w, pixelHeight=h,
                            raw=data[start:stop], before=data[base:start], after=data[stop:end])
        else:
            metadata.update(raw=None, before=data[base:end], after=b'')
        entries.append(metadata)
        if not following:
            return entries
        base = end
        if len(entries) > 256:
            raise ValueError('Too many ANM entries')


def png_texture(entry):
    from PIL import Image
    raw, w, h, fmt = entry['raw'], entry['pixelWidth'], entry['pixelHeight'], entry['format']
    expected = w * h * (4 if fmt == 1 else 2 if fmt in (3, 5) else 1 if fmt == 7 else 0)
    if not expected or len(raw) != expected:
        raise ValueError('Unsupported THTX pixel format or byte count')
    if fmt == 1:
        pixels = Image.frombytes('RGBA', (w, h), raw, 'raw', 'BGRA')
    else:
        rgba = bytearray(w * h * 4)
        values = raw if fmt == 7 else (item[0] for item in struct.iter_unpack('<H', raw))
        for i, value in enumerate(values):
            color = (0, 0, 0, value) if fmt == 7 else (
                (((value >> 8) & 15) * 17, ((value >> 4) & 15) * 17,
                 (value & 15) * 17, ((value >> 12) & 15) * 17) if fmt == 5 else
                (((value >> 11) & 31) * 255 // 31, ((value >> 5) & 63) * 255 // 63,
                 (value & 31) * 255 // 31, 255))
            rgba[i * 4:i * 4 + 4] = bytes(color)
        pixels = Image.frombytes('RGBA', (w, h), bytes(rgba))
    width, height = max(w, entry['width']), max(h, entry['height'])
    if width > 8192 or height > 8192:
        raise ValueError('Oversized logical texture')
    logical = Image.new('RGBA', (width, height), (0, 0, 0, 255 if entry['textureFormat'] == 3 else 0))
    logical.paste(pixels, (0, 0))
    output = io.BytesIO()
    logical.save(output, format='PNG', compress_level=9)
    return output.getvalue(), width, height


def music_comments(data, encoding):
    lines, result, cursor = data.decode(encoding, errors='strict').splitlines(), {}, 0
    while cursor < len(lines):
        line = lines[cursor]
        cursor += 1
        if not line.startswith('@'):
            continue
        if not re.fullmatch(r'@bgm/th12_\d\d\.wav', line) or cursor + 9 > len(lines):
            raise ValueError('Unsupported Music Room record')
        label, body = lines[cursor], lines[cursor + 1:cursor + 9]
        cursor += 9
        if not label.startswith('No.') or not body[0].startswith(('♪', '◇')):
            raise ValueError('Unsupported Music Room title')
        result[Path(line[1:]).stem] = {'label': label, 'title': body[0][1:], 'lines': body,
                                     'comment': '\n'.join(body)}
    return result


def pe_string_table(data, address, count, encoding):
    if data[:2] != b'MZ':
        raise ValueError('Expected PE executable as static data')
    pe = u32(data, 0x3c)
    if data[pe:pe + 4] != b'PE\0\0' or u16(data, pe + 24) != 0x10b:
        raise ValueError('Expected PE32')
    sections, optional = u16(data, pe + 6), u16(data, pe + 20)
    image_base, table = u32(data, pe + 52), pe + 24 + optional

    def offset(va):
        rva = va - image_base
        for i in range(sections):
            size, start, raw_size, raw = struct.unpack_from('<IIII', data, table + i * 40 + 8)
            if start <= rva < start + max(size, raw_size) and rva - start < raw_size:
                return raw + rva - start
        raise ValueError('Native string address is outside file-backed sections')

    at, result = offset(address), []
    for i in range(count):
        start = offset(u32(data, at + 4 * i))
        end = data.index(0, start, min(start + 4096, len(data)))
        result.append(data[start:end].decode(encoding, errors='strict'))
    return result


def expected_changes(source):
    declared = source['changedResources']
    files = set(declared['textures'] + declared['spellSources'] + declared['endings'])
    files.add(declared['musicComments'])
    d = declared['dialogue']
    files.update(d['pattern'].format(stage=s, character=c, shot=shot)
                 for s in d['stages'] for c in d['characters'] for shot in d['shots'])
    return files


def extract(thdat, archive, destination):
    # thtk thdat(1): -l12 / -x12, extraction destination is cwd.
    listing = subprocess.run([str(thdat), '-l12', str(archive)], check=True,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE).stdout
    for raw_line in listing.splitlines():
        line = raw_line.decode('ascii', errors='strict').strip()
        if not line or line.startswith('Name '):
            continue
        name = line.split()[0]
        if not re.fullmatch(r'[A-Za-z0-9_.-]+', name) or name in ('.', '..'):
            raise ValueError('Archive contains an unsafe or unexpected path')
    subprocess.run([str(thdat), '-x12', str(archive)], cwd=destination,
                   check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if not (destination / 'musiccmt.txt').is_file():
        raise ValueError('thdat did not extract the required resources')


def build(source, ja, zh, original):
    left = {p.name: p for p in ja.iterdir() if p.is_file()}
    right = {p.name: p for p in zh.iterdir() if p.is_file()}
    if set(left) != set(right):
        raise ValueError('The two archives have different unique resource sets')
    changed = {name for name in left if left[name].read_bytes() != right[name].read_bytes()}
    if changed != expected_changes(source):
        raise ValueError('Resource differences do not match the versioned source manifest')
    overlay = {'schema': 'th12-localization-overlay/1', 'game': 'th12',
               'language': source['language'], 'sourceRevision': source['sourceRevision'],
               'sourceExeSha256': source['files']['th12.exe'], 'fontFamily': 'Eagler Unicode',
               'dialogue': {}, 'dialoguePatches': {}, 'endings': {}, 'spells': {}, 'textures': {}, 'musicComments': {}}
    resources, provenance, controls = {}, [], {}
    for name in sorted(changed):
        a, b = left[name].read_bytes(), right[name].read_bytes()
        provenance.append({'path': name, 'sourceSha256': digest(a), 'translationSha256': digest(b)})
        if re.fullmatch(r'st\d\d_0[012][ab]\.msg', name):
            try:
                overlay['dialogue'][name], overlay['dialoguePatches'][name], controls[name] = dialogue_overlay(a, b)
            except ValueError as error:
                raise ValueError(f'{name}: {error}') from error
        elif re.fullmatch(r'e\d\d\.msg', name):
            try:
                overlay['endings'][name], controls[name] = message_overlay(
                    a, b, ending=True, omitted_times=source.get('endingTextOmissions', {}).get(name, []),
                    time_mapping=source.get('endingTextTimeMapping', {}).get(name, {}))
            except ValueError as error:
                raise ValueError(f'{name}: {error}') from error
        elif name.endswith('.ecl'):
            old, new = spell_records(a, extra=name.startswith('stage07')), spell_records(b, extra=name.startswith('stage07'))
            if old.keys() != new.keys():
                raise ValueError(f'{name}: translation changed spell IDs')
            for card in old:
                if old[card] != new[card]:
                    _, value = text_value(new[card], 'cp936')
                    value['sourceHex'] = old[card].hex()
                    key = str(card)
                    if key in overlay['spells'] and overlay['spells'][key] != value:
                        raise ValueError('Translated spell ID has conflicting definitions')
                    overlay['spells'][key] = value
        elif name.endswith('.anm'):
            old, new = texture_entries(a), texture_entries(b)
            if len(old) != len(new):
                raise ValueError('Translation changed ANM entry count')
            for i, (source_entry, translated_entry) in enumerate(zip(old, new)):
                # Complete THTX replacements, including transparent pixels.
                if source_entry['before'] != translated_entry['before'] or source_entry['after'] != translated_entry['after']:
                    raise ValueError(f'{name}: non-pixel ANM differences are unsupported')
                if source_entry['raw'] == translated_entry['raw']:
                    continue
                image = f'{Path(name).stem}-{i}.png'
                path = PREFIX + 'textures/' + image
                png, width, height = png_texture(translated_entry)
                resources[path] = png
                overlay['textures'][image] = {'path': path, 'width': width, 'height': height,
                                              'semantics': 'replace', 'source': name, 'entry': i}
    old = music_comments(left['musiccmt.txt'].read_bytes(), 'cp932')
    new = music_comments(right['musiccmt.txt'].read_bytes(), 'cp936')
    if old.keys() != new.keys():
        raise ValueError('Translation changed the Music Room track set')
    overlay['musicComments'] = {key: new[key] for key in new if old[key] != new[key]}
    spec = source['nativeText']
    old_warning = pe_string_table((original / 'th12.exe').read_bytes(), spec['musicWarningPointerTable'], spec['musicWarningLines'], 'cp932')
    new_warning = pe_string_table((original / 'th12c.exe').read_bytes(), spec['musicWarningPointerTable'], spec['musicWarningLines'], 'cp936')
    if old_warning != new_warning:
        overlay['musicWarning'] = {'sourceLines': old_warning, 'lines': new_warning}
    overlay['provenance'] = provenance
    overlay['coverage'] = {'changedSourceResources': len(changed),
                           'dialogueFiles': len(overlay['dialogue']), 'endingFiles': len(overlay['endings']),
                           'dialogueLines': sum(map(len, overlay['dialogue'].values())),
                           'endingLines': sum(map(len, overlay['endings'].values())),
                           'spellNames': len(overlay['spells']), 'textures': len(overlay['textures']),
                           'musicTracks': len(overlay['musicComments']),
                           'ignoredControlDifferences': {k: v for k, v in controls.items() if v},
                           'nativeExecutableApplied': False, 'fullThcrap': False,
                           'visualParity': 'unverified'}
    resources[PREFIX + 'overlay.json'] = json_bytes(overlay)
    if len(resources) > 256:
        raise ValueError('Language pack exceeds the platform file limit')
    files = [{'path': path, 'sourcePath': path.removeprefix(PREFIX),
              'format': 'th12-localization-overlay' if path.endswith('.json') else 'png',
              'bytes': len(data), 'sha256': digest(data)} for path, data in sorted(resources.items())]
    manifest = {'schema': 'eagler-touhou/thcrap-static-pack/1', 'game': 'th12',
                'language': source['language'], 'title': source['title'],
                'runtimeVersion': 'independent', 'sourceRevision': source['sourceRevision'],
                'dependencies': [], 'files': files}
    return manifest, resources, overlay['coverage']


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original', default=os.environ.get('TH12_ORIGINAL'), required=not os.environ.get('TH12_ORIGINAL'))
    parser.add_argument('--thdat', default=os.environ.get('TH12_THDAT') or os.environ.get('THDAT'))
    parser.add_argument('--output', default=str(ROOT / 'local/content'))
    parser.add_argument('--source-manifest', default=str(ROOT / 'localization/th12c-1.00b.json'))
    args = parser.parse_args()
    original, output = Path(args.original).resolve(), Path(args.output).resolve()
    if output == original or original in output.parents or output in original.parents:
        raise ValueError('Output and original installation must be separate directories')
    source = json.loads(Path(args.source_manifest).read_text('utf-8'))
    if source.get('schema') != 'th12-localization-source/1' or source.get('language') != 'lang_zh-hant':
        raise ValueError('Unsupported localization source manifest')
    for name, sha in source['files'].items():
        if name not in ('th12.dat', 'th12c.dat', 'th12.exe', 'th12c.exe') or digest((original / name).read_bytes()) != sha:
            raise ValueError(f'Source identity does not match: {name}')
    tool = args.thdat or shutil.which('thdat')
    if not tool or not Path(tool).is_file():
        raise ValueError('Supply a local thtk thdat executable via --thdat or TH12_THDAT')
    thdat = Path(tool).resolve()
    with tempfile.TemporaryDirectory(prefix='th12-localization-') as temporary:
        workspace = Path(temporary)
        ja, zh = workspace / 'ja', workspace / 'zh-hant'
        ja.mkdir()
        zh.mkdir()
        extract(thdat, original / 'th12.dat', ja)
        extract(thdat, original / 'th12c.dat', zh)
        manifest, resources, coverage = build(source, ja, zh, original)
        packed = io.BytesIO()
        with zipfile.ZipFile(packed, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
            entries = {'manifest.json': json_bytes(manifest), **{p.lstrip('/'): b for p, b in resources.items()}}
            for name, data in sorted(entries.items()):
                info = zipfile.ZipInfo(name, date_time=(1980, 1, 1, 0, 0, 0))
                info.compress_type = zipfile.ZIP_DEFLATED
                info.external_attr = 0o100644 << 16
                archive.writestr(info, data, compress_type=zipfile.ZIP_DEFLATED, compresslevel=6)
        payload = packed.getvalue()
    language = output / 'language'
    language.mkdir(parents=True, exist_ok=True)
    name = source['language'] + '.zip'
    catalog = {'schema': 'th12-localization-catalog/1', 'game': 'th12', 'defaultLanguage': 'ja',
               'languages': [{'id': source['language'], 'title': source['title'],
                              'pack': {'url': 'language/' + name, 'bytes': len(payload),
                                       'sha256': digest(payload), 'runtimeVersion': 'independent',
                                       'files': len(manifest['files'])}}]}
    (language / name).write_bytes(payload)
    (language / 'catalog.json').write_bytes(json_bytes(catalog))
    (language / (source['language'] + '.manifest.json')).write_bytes(json_bytes(manifest))
    (language / (source['language'] + '.coverage.json')).write_bytes(json_bytes(coverage))
    unpacked = output / 'files'
    for path, data in resources.items():
        destination = unpacked / path.lstrip('/')
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_bytes(data)
    (output / 'manifest.json').write_bytes(json_bytes(manifest))
    print(f'Prepared {name}: {len(manifest["files"])} data-only resources; sha256={digest(payload)}')


if __name__ == '__main__':
    main()
