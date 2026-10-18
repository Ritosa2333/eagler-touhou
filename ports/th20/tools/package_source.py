"""Create a source-only, hashed snapshot without generated binaries or assets."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import zipfile

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('name', help='New snapshot directory name under dist')
parser.add_argument('--zip', action='store_true', help='Zip the existing snapshot after build verification')
parser.add_argument('--refresh', action='store_true', help='Refresh this tool\'s unsealed snapshot before rebuilding and verification')
args = parser.parse_args()
if not args.name or Path(args.name).name != args.name or args.name in ('.', '..'):
    raise SystemExit('Expected a single directory name')
destination = (root / 'dist' / args.name).resolve()
if destination.parent != (root / 'dist').resolve():
    raise SystemExit('Snapshot must be directly under the project dist directory')

if args.zip:
    manifest = json.loads((destination / 'SOURCE_SNAPSHOT.json').read_text(encoding='utf-8'))
    verification = json.loads((destination / 'SOURCE_BUILD_VERIFICATION.json').read_text(encoding='utf-8'))
    if verification.get('status') != 'fresh_source_build_verified' or 'SOURCE_BUILD_VERIFICATION.json' not in manifest['files']:
        raise SystemExit('Verify the snapshot build before creating the archive')
    files = [*manifest['files'], 'SOURCE_SNAPSHOT.json']
    archive = destination.parent / (destination.name + '.zip')
    with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED, compresslevel=6) as output:
        for relative in sorted(files):
            path = (destination / relative).resolve()
            if not path.is_relative_to(destination):
                raise SystemExit(f'Out-of-snapshot archive input: {relative}')
            if relative in manifest['files']:
                actual = hashlib.sha256(path.read_bytes()).hexdigest()
                if actual != manifest['files'][relative]:
                    raise SystemExit(f'Snapshot changed: {relative}')
            output.write(path, f'{args.name}/{relative}')
    print(json.dumps({'archive': str(archive), 'bytes': archive.stat().st_size,
                      'sha256': hashlib.sha256(archive.read_bytes()).hexdigest()}))
    raise SystemExit(0)

if destination.exists() and not args.refresh:
    raise SystemExit('Use a new snapshot name; existing snapshots are never overwritten')
if args.refresh:
    old = json.loads((destination / 'SOURCE_SNAPSHOT.json').read_text(encoding='utf-8'))
    if old.get('status') != 'independent_source_candidate' or (destination.parent / (destination.name + '.zip')).exists():
        raise SystemExit('Only an unsealed source snapshot from this tool can be refreshed')
extensions = {'.cpp', '.hpp', '.h', '.c', '.inc', '.inl', '.cmake', '.rc', '.ico',
              '.md', '.py', '.ps1', '.json'}
selected = []
for directory in ['source_reconstruction', 'native_recovered', 'src', 'include', 'tests', 'scripts/recovered', 'tools/asset_index']:
    for current, dirs, names in os.walk(root / directory):
        dirs[:] = [d for d in dirs if not d.startswith('build') and d not in
                   {'__pycache__', '.git', 'evidence', 'frame_evidence', 'iterator_hazard_evidence', 'compile_probe', 'roundtrip'}]
        if Path(current) == root / 'source_reconstruction' / 'diagnostics':
            dirs[:] = []  # Saved executions include dumps, binaries, and user data.
        for name in names:
            path = Path(current) / name
            script_text = directory == 'scripts/recovered' and path.suffix.lower() == '.txt'
            if path.suffix.lower() in extensions or name == 'CMakeLists.txt' or script_text:
                selected.append(path)
selected += [root / p for p in ['SOURCE_BUILD.md', 'build_source_game.ps1', 'AGENTS.md',
    'reports/source_manifest.json', 'reports/SOURCE_GAME_RUNTIME.md', 'tools/package_source.py',
    'tools/verify_source_snapshot.py', 'CMakeLists.txt', 'tools/asset_build_thtk.ps1',
    'tools/asset_extract_index.c', 'tools/asset_recover.py', 'tools/asset_thtk_th20.patch',
    'tools/verify_artifacts.py', 'tools/recover_bgm.py', 'reports/assets_recovery.json',
    'reports/artifact_verification.json', 'reports/bgm_recovery.json', 'reports/ecl_binary_scan.json']]
manifest = {'status': 'independent_source_candidate', 'full_game_equivalence_verified': False,
            'original_executable_included': False, 'game_assets_included': False, 'files': {}}
for path in sorted(set(selected)):
    relative = path.relative_to(root)
    target = destination / relative
    target.parent.mkdir(parents=True, exist_ok=True)
    if not target.exists() or path.read_bytes() != target.read_bytes():
        shutil.copyfile(path, target)
    manifest['files'][relative.as_posix()] = hashlib.sha256(target.read_bytes()).hexdigest()
(destination / 'SOURCE_SNAPSHOT.json').write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
print(json.dumps({'snapshot': str(destination), 'files': len(manifest['files']),
                  'bytes': sum((destination / p).stat().st_size for p in manifest['files'])}))
