import argparse
import hashlib
import json
import os
import re
import stat
import subprocess
import sys
import tempfile
import uuid
import wave
from collections import Counter
from pathlib import Path

from .formats import (
    FormatError, decode_texture, encode_png, parse_anm, parse_ecl,
    parse_music, require, validate_archive_name,
)
from .ice_masks import read_ice_masks
from .bullet_profiles import read_bullet_profiles,read_boss_bullet_profiles
from .messages import parse_stage_messages


PROJECT = Path(__file__).resolve().parents[2]
TOOL_HASHES = {
    'thdat.exe': 'd49471473286c3460eb8214a3c94317655074b26a164148bfbc78398a5b9101c',
    'thecl.exe': '3533986771d918101170f208c383d7857e8db42329183178c9877354da951f64',
    'thtk.dll': '49e18c6cf7f5a082554d395c1d1b5138bf68fb361860bcb1f87828f5ef6fcee8',
}
VARIANTS = {
    'ja': {'archive': 'th128.dat', 'executable': 'th128.exe', 'title': '日本語 / original'},
    'zh': {'archive': 'th128cn.dat', 'executable': 'th128_CN.exe', 'title': '中文 / existing translation'},
}


def digest(path):
    with path.open('rb') as source:
        return hashlib.file_digest(source, 'sha256').hexdigest()


def identity(path):
    return {'bytes': path.stat().st_size, 'sha256': digest(path)}


def atomic_json(path, value):
    serialized = json.dumps(value, ensure_ascii=False, indent=2, allow_nan=False) + '\n'
    path.parent.mkdir(parents=True, exist_ok=True)
    descriptor, temporary = tempfile.mkstemp(prefix='.json-', suffix='.tmp', dir=path.parent)
    try:
        with os.fdopen(descriptor, 'w', encoding='utf-8', newline='\n') as output:
            output.write(serialized)
            output.flush()
            os.fsync(output.fileno())
        os.replace(temporary, path)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)


class ArchiveListing(dict):
    def __init__(self):
        super().__init__()
        self.duplicates = []
        self.physical_entries = 0
        self.expanded_bytes = 0


def parse_listing(text):
    rows = text.strip().splitlines()
    require(bool(rows) and re.fullmatch(r'Name\s+Size\s+Stored', rows[0].strip()), 'unexpected thdat listing header')
    listing, folded, stored_sizes = ArchiveListing(), set(), {}
    for row in rows[1:]:
        match = re.fullmatch(r'(\S+)\s+([0-9]+)\s+([0-9]+)', row.strip())
        require(match is not None, f'unrecognized thdat listing row: {row!r}')
        name, size_text, _ = match.groups()
        validate_archive_name(name)
        size = int(size_text)
        require(0 < size <= 128 * 1024 * 1024, f'unsupported archive entry size: {name}')
        listing.physical_entries += 1
        listing.expanded_bytes += size
        require(listing.physical_entries <= 4096 and listing.expanded_bytes <= 1024 * 1024 * 1024,
                'physical archive entries exceed decoded count/byte budget')
        if name.casefold() in folded:
            require(name in listing and listing[name] == size and stored_sizes[name] == int(match[3]),
                    f'conflicting or case-ambiguous archive name: {name}')
            if name not in listing.duplicates:
                listing.duplicates.append(name)
            continue
        folded.add(name.casefold())
        listing[name] = size
        stored_sizes[name] = int(match[3])
    require(0 < len(listing) <= 4096, 'invalid archive entry count')
    require(sum(listing.values()) <= 1024 * 1024 * 1024, 'archive expands beyond one GiB')
    return listing


def validate_locations(original, output):
    source, destination = original.resolve(), output.resolve()
    require(source != destination and not destination.is_relative_to(source)
            and not source.is_relative_to(destination), 'original and output directories must not overlap')


def validate_report_destination(report, protected_roots):
    destination = report.resolve()
    require(destination.is_relative_to(PROJECT.resolve()), 'report must stay inside the deliverable project')
    for protected in [PROJECT / 'local', *protected_roots]:
        root = protected.resolve()
        require(destination != root and not destination.is_relative_to(root),
                f'report must not overwrite a protected input directory: {root}')
    return destination


def generation_directory(output):
    require(output.resolve() == output.absolute(), 'output root changed or is redirected')
    root = output.resolve()
    destination = root / 'generations'
    require(destination.resolve() == destination, 'generation directory must not be redirected by a link or junction')
    destination.mkdir(exist_ok=True)
    require(destination.resolve() == destination and not is_reparse_point(destination),
            'generation directory changed during validation')
    return destination


def verify_extracted(root, listing):
    actual = list(root.iterdir())
    require(all(path.is_file() and not path.is_symlink() for path in actual), 'unexpected extracted directory/link')
    require({path.name for path in actual} == set(listing), 'extracted files differ from archive listing')
    for path in actual:
        require(path.stat().st_size == listing[path.name], f'extracted size mismatch: {path.name}')


def validate_duplicate_audit(audit, listing):
    require(isinstance(audit, list) and {record['name'] for record in audit} == set(listing.duplicates),
            'duplicate audit name mismatch')
    for record in audit:
        entries = record['entries']
        require(len(entries) >= 2 and record['identical'], 'unverified duplicate archive entries')
        require(all(entry['bytes'] == listing[record['name']] for entry in entries),
                'duplicate audit size differs from listing')
        hashes = {entry['sha256'] for entry in entries}
        require(len(hashes) == 1 and all(re.fullmatch(r'[a-f0-9]{64}', checksum) for checksum in hashes),
                'duplicate entries have different or invalid hashes')


def run_tool(tool, arguments, cwd):
    environment = {**os.environ, 'OMP_NUM_THREADS': '2'}
    completed = subprocess.run([str(tool), *map(str, arguments)], cwd=cwd, env=environment,
                               capture_output=True, timeout=180,
                               creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    output = completed.stdout.decode('utf-8', errors='replace')
    errors = completed.stderr.decode('utf-8', errors='replace')
    require(completed.returncode == 0, f'{tool.name} failed ({completed.returncode}): {errors[-4000:]}')
    return output, errors


def is_reparse_point(path):
    return path.is_symlink() or bool(getattr(path.lstat(), 'st_file_attributes', 0)
                                    & getattr(stat, 'FILE_ATTRIBUTE_REPARSE_POINT', 0x400))


def snapshot(root):
    require(not is_reparse_point(root), f'original root is a link or reparse point: {root}')
    root = root.resolve()
    records = {}
    directories = [root]
    while directories:
        directory = directories.pop()
        for path in sorted(directory.iterdir()):
            require(not is_reparse_point(path), f'original directory contains link or reparse point: {path}')
            require(path.resolve().is_relative_to(root), f'original path escapes root: {path}')
            if path.is_dir():
                directories.append(path)
            elif path.is_file():
                records[path.relative_to(root).as_posix()] = identity(path)
            else:
                raise FormatError(f'unsupported original filesystem entry: {path}')
    return records


def prepare_variant(original, tools, destination, variant, source_snapshot, tool_info):
    specification = VARIANTS[variant]
    archive = original / specification['archive']
    require(archive.is_file(), f'missing original archive: {archive}')
    require((original / specification['executable']).is_file(), 'missing variant executable for provenance')
    require((original / 'thbgm.dat').is_file(), 'missing thbgm.dat')
    raw = destination / 'raw'
    raw.mkdir(parents=True)
    text, errors = run_tool(tools / 'thdat.exe', ['-l128', archive], raw)
    listing = parse_listing(text)
    atomic_json(destination / 'archive-listing.json', listing)
    duplicate_audit = []
    if listing.duplicates:
        powershell = Path(os.environ.get('WINDIR', 'C:/Windows')) / 'SysWOW64/WindowsPowerShell/v1.0/powershell.exe'
        require(powershell.is_file(), 'x86 PowerShell required to audit duplicate DAT entries')
        audit_directory = destination / 'duplicate-audit'
        audit_directory.mkdir()
        audit_path = audit_directory / 'audit.json'
        run_tool(powershell, ['-NoLogo', '-NoProfile', '-NonInteractive', '-File',
                             PROJECT / 'tools/audit-duplicates.ps1', '-LibraryDirectory', tools,
                             '-Archive', archive, '-OutputDirectory', audit_directory], destination)
        duplicate_audit = json.loads(audit_path.read_text(encoding='utf-8-sig'))
        validate_duplicate_audit(duplicate_audit, listing)
    print(f'[{variant}] extracting {len(listing)} original files', flush=True)
    extracted_log, extracted_errors = run_tool(tools / 'thdat.exe', ['-x128', archive, *listing], raw)
    verify_extracted(raw, listing)
    for record in duplicate_audit:
        require(digest(raw / record['name']) == record['entries'][0]['sha256'], 'duplicate extraction checksum mismatch')
    (destination / 'thdat.log').write_text(text + errors + extracted_log + extracted_errors, encoding='utf-8')
    for directory in ('animations', 'textures', 'ecl', 'music', 'sounds'):
        (destination / directory).mkdir()
    mask_source=original / 'th128.exe'
    require(0<mask_source.stat().st_size<=1048576,'ice masks require bounded original th128.exe')
    masks=read_ice_masks(mask_source.read_bytes())
    atomic_json(destination / 'animations/ice-masks.json',masks)
    atomic_json(destination / 'animations/bullet-native-profiles.json',read_bullet_profiles(mask_source.read_bytes()))
    atomic_json(destination / 'animations/bullet-boss-profiles.json',read_boss_bullet_profiles(mask_source.read_bytes()))
    animations, programs, files, sounds = [], [], [], []
    anm_opcodes, ecl_opcodes = Counter(), Counter()
    for position, (name, size) in enumerate(listing.items()):
        path = raw / name
        file_identity = {'name': name, **identity(path)}
        files.append(file_identity)
        suffix = path.suffix.lower()
        if suffix == '.anm':
            parsed = parse_anm(path.read_bytes())
            for entry in parsed['entries']:
                texture = entry['texture']
                if texture is None:
                    continue
                rgba = decode_texture(texture['format'], texture['width'], texture['height'], texture.pop('data'))
                png = encode_png(texture['width'], texture['height'], rgba)
                relative = f'textures/{path.stem}-{entry["index"]:03}.png'
                (destination / relative).write_bytes(png)
                texture['file'] = relative
                texture['sha256'] = hashlib.sha256(png).hexdigest()
            for script in parsed['scripts']:
                anm_opcodes.update(str(command['opcode']) for command in script['commands'])
            parsed['provenance'] = file_identity
            parsed['animationExecuted'] = False
            metadata = f'animations/{path.stem}.json'
            atomic_json(destination / metadata, parsed)
            animations.append({'name': name, 'file': metadata,
                               'entries': len(parsed['entries']), 'sprites': len(parsed['sprites']),
                               'scripts': len(parsed['scripts']),
                               'textures': sum(entry['texture'] is not None for entry in parsed['entries'])})
        elif suffix == '.ecl':
            parsed = parse_ecl(path.read_bytes())
            parsed['provenance'] = file_identity
            ecl_opcodes.update(parsed['opcodeHistogram'])
            metadata, decoded = f'ecl/{path.stem}.json', f'ecl/{path.stem}.txt'
            atomic_json(destination / metadata, parsed)
            _, errors = run_tool(tools / 'thecl.exe', ['-d128', path, destination / decoded], destination)
            require((destination / decoded).stat().st_size > 0, f'empty ECL dump: {name}')
            if errors:
                (destination / f'ecl/{path.stem}.warnings.txt').write_text(errors, encoding='utf-8')
            programs.append({'name': name, 'file': metadata, 'text': decoded,
                             'subroutines': len(parsed['subroutines']),
                             'instructions': sum(len(sub['commands']) for sub in parsed['subroutines']),
                             'opcodes': len(parsed['opcodeHistogram']), 'toolWarnings': errors})
        elif suffix == '.msg' and name.startswith('st_'):
            atomic_json(destination / f'animations/message-{path.stem}.json',
                parse_stage_messages(path.read_bytes(),encoding='cp932' if variant=='ja' else 'gbk'))
        elif suffix == '.wav':
            with wave.open(str(path), 'rb') as source:
                require(source.getcomptype() == 'NONE', f'compressed WAV not supported: {name}')
                sound = {'name': name, 'channels': source.getnchannels(), 'sampleRate': source.getframerate(),
                         'duration': source.getnframes() / source.getframerate(), 'file': f'sounds/{name}'}
            (destination / sound['file']).write_bytes(path.read_bytes())
            sounds.append(sound)
        if position % 25 == 0:
            print(f'[{variant}] parsed {position + 1}/{len(listing)}: {name}', flush=True)
    available = set(listing)
    for program in programs:
        parsed = json.loads((destination / program['file']).read_text(encoding='utf-8'))
        for reference in (*parsed['includes']['ANIM'], *parsed['includes']['ECLI']):
            require(reference in available, f'missing ECL dependency: {program["name"]} -> {reference}')
    music_table = raw / 'thbgm.fmt'
    require(music_table.is_file(), 'missing TH128 music descriptor')
    tracks = parse_music(music_table.read_bytes(), (original / 'thbgm.dat').stat().st_size)
    with (original / 'thbgm.dat').open('rb') as music_source:
        for track in tracks:
            music_source.seek(track['offset'])
            pcm = music_source.read(track['pcmBytes'])
            require(len(pcm) == track['pcmBytes'], f'truncated music {track["name"]}')
            track['file'] = f'music/{track["name"]}'
            with wave.open(str(destination / track['file']), 'wb') as output:
                output.setnchannels(track['channels'])
                output.setsampwidth(track['bits'] // 8)
                output.setframerate(track['sampleRate'])
                output.writeframes(pcm)
            track['sha256'] = digest(destination / track['file'])
    known_types = {'.anm', '.ecl', '.wav', '.fmt'}
    unopened = Counter(Path(name).suffix.lower() for name in listing if Path(name).suffix.lower() not in known_types)
    routes = [program['name'] for program in programs if re.fullmatch(r'st_(?:[abc][12]_[123]|ex)\.ecl', program['name'])]
    manifest = {
        'schema': 'th128-prepared-assets/1', 'game': 'th128', 'milestone': 'M0', 'playable': False,
        'variant': variant, 'title': specification['title'],
        'sourceFiles': {name: source_snapshot[name] for name in
                        (specification['archive'], specification['executable'], 'thbgm.dat')},
        'tools': tool_info, 'duplicateEntries': duplicate_audit, 'files': files, 'animations': animations, 'ecl': programs,
        'archiveDirectory': {'physicalEntries': listing.physical_entries, 'physicalDecodedBytes': listing.expanded_bytes},
        'routes': routes, 'music': tracks, 'sounds': sounds,
        'opcodeCoverage': {'anmObserved': dict(anm_opcodes), 'eclObserved': dict(ecl_opcodes),
                           'anmExecuted': [], 'eclExecuted': []},
        'uninterpretedResourceTypes': dict(unopened),
        'boundaries': ['Textures and scripts are inspected, not executed.',
                       'STD/MSG/SHT and other raw resources are preserved but not interpreted.',
                       'Native gameplay, freezing rules, menus and replay compatibility are not implemented.'],
    }
    manifest['totals'] = {
        'files': len(files), 'extractedBytes': sum(record['bytes'] for record in files),
        'anmArchives': len(animations), 'textures': sum(record['textures'] for record in animations),
        'sprites': sum(record['sprites'] for record in animations),
        'anmScripts': sum(record['scripts'] for record in animations),
        'eclFiles': len(programs), 'eclSubroutines': sum(record['subroutines'] for record in programs),
        'eclInstructions': sum(record['instructions'] for record in programs),
        'musicTracks': len(tracks), 'soundEffects': len(sounds), 'routeScripts': len(routes),
    }
    atomic_json(destination / 'manifest.json', manifest)
    print(f'[{variant}] ready: {json.dumps(manifest["totals"])}', flush=True)
    return manifest


def prepare(original, tools, output, variants):
    original, tools, output = original.resolve(), tools.resolve(), output.resolve()
    require(original.is_dir(), f'original directory does not exist: {original}')
    validate_locations(original, output)
    require(not tools.is_relative_to(original), 'do not install tools in the original directory')
    tool_info = {'release': 12, 'source': 'https://github.com/thpatch/thtk/releases/tag/12', 'files': {}}
    for name, expected in TOOL_HASHES.items():
        path = tools / name
        require(path.is_file(), f'missing thtk 12 tool: {path}')
        require(digest(path) == expected, f'tool identity mismatch: {name}; use official Windows thtk release 12')
        tool_info['files'][name] = identity(path)
    print('Hashing original installation (read-only)...', flush=True)
    before = snapshot(original)
    output.mkdir(parents=True, exist_ok=True)
    generations = generation_directory(output)
    stage = Path(tempfile.mkdtemp(prefix='.staging-', dir=output))
    manifests = {}
    for variant in variants:
        manifests[variant] = prepare_variant(original, tools, stage / variant, variant, before, tool_info)
    after = snapshot(original)
    require(before == after, 'original directory changed during preparation; results not published')
    atomic_json(stage / 'source-immutability.json', {'unchanged': True, 'before': before, 'after': after})
    validate_locations(original, output)
    require(generation_directory(output) == generations, 'generation destination changed before publication')
    generation = uuid.uuid4().hex
    destination = generations / generation
    os.replace(stage, destination)
    catalog_path = output / 'catalog.json'
    previous = json.loads(catalog_path.read_text(encoding='utf-8')) if catalog_path.is_file() else {}
    catalog_variants = previous.get('variants', {})
    for variant, manifest in manifests.items():
        catalog_variants[variant] = {'title': manifest['title'],
                                     'manifest': f'generations/{generation}/{variant}/manifest.json',
                                     'totals': manifest['totals']}
    catalog = {'schema': 'th128-asset-catalog/1', 'playable': False, 'variants': catalog_variants}
    atomic_json(catalog_path, catalog)
    print(f'Published {catalog_path}', flush=True)
    return catalog


def main(argv=None):
    parser = argparse.ArgumentParser(description='Prepare actual TH128 assets; this does not build a playable game.')
    parser.add_argument('--original', type=Path, required=True)
    parser.add_argument('--thtk', type=Path, required=True, help='Official Windows thtk 12 binary directory')
    parser.add_argument('--output', type=Path, default=PROJECT / 'local')
    parser.add_argument('--variant', choices=('ja', 'zh', 'both'), default='both')
    arguments = parser.parse_args(argv)
    variants = list(VARIANTS) if arguments.variant == 'both' else [arguments.variant]
    try:
        prepare(arguments.original, arguments.thtk, arguments.output, variants)
    except (FormatError, OSError, subprocess.SubprocessError, wave.Error, json.JSONDecodeError) as error:
        print(f'Preparation failed: {error}', file=sys.stderr)
        print('No new catalog was published. Staging is retained for diagnosis.', file=sys.stderr)
        return 1
    return 0
