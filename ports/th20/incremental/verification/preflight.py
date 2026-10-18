"""Read-only inventory and isolated save copies for TH20 runtime verification.

Run with --prepare once before launching either verification process. The original
game and original APPDATA saves are only read. Source snapshots are never replaced.
"""
from __future__ import annotations
import argparse
import csv
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
REPORT = ROOT / 'reports' / 'playable_preflight.json'
SOURCE_MANIFEST = ROOT / 'reports' / 'source_manifest.json'


def sha256(path):
    with open(path, 'rb') as file:
        return hashlib.file_digest(file, 'sha256').hexdigest()


def inventory(directory):
    directory = Path(directory)
    return [dict(path=str(path.relative_to(directory)), size=path.stat().st_size,
                 sha256=sha256(path))
            for path in sorted(directory.rglob('*')) if path.is_file()]


def pe_machine(path):
    with open(path, 'rb') as file:
        header = file.read(64)
        if header[:2] != b'MZ':
            raise ValueError(f'Not an MZ file: {path}')
        file.seek(struct.unpack_from('<I', header, 0x3c)[0])
        pe = file.read(6)
        if pe[:4] != b'PE\0\0':
            raise ValueError(f'Not a PE file: {path}')
        return hex(struct.unpack_from('<H', pe, 4)[0])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--prepare', action='store_true')
    parser.add_argument('--verify-originals', action='store_true')
    args = parser.parse_args()
    manifest = json.loads(SOURCE_MANIFEST.read_text(encoding='utf-8'))
    source = Path(manifest['source_directory'])
    save = Path(os.environ['APPDATA']) / 'ShanghaiAlice' / 'th20'
    system32 = Path(os.environ['WINDIR']) / 'SysWOW64'
    dependencies = []
    with (ROOT / 'analysis' / 'binary' / 'imports.csv').open(newline='', encoding='utf-8') as file:
        names = sorted({item['dll'] for item in csv.DictReader(file)}, key=str.casefold)
    for name in names:
        path = system32 / name
        entry = dict(name=name, path=str(path), exists=path.is_file())
        if path.is_file():
            entry.update(machine=pe_machine(path), sha256=sha256(path), size=path.stat().st_size)
        dependencies.append(entry)
    current_source = inventory(source)
    expected_source = sorted([dict(path=f['name'], size=f['size'], sha256=f['sha256'])
                              for f in manifest['files']], key=lambda f: f['path'])
    current_source = sorted(current_source, key=lambda f: f['path'])
    original_save = inventory(save)
    backup_manifest = HERE / 'source_save_manifest.json'
    if args.prepare:
        HERE.mkdir(parents=True, exist_ok=True)
        if backup_manifest.exists():
            old = json.loads(backup_manifest.read_text(encoding='utf-8'))
            if original_save != old['files']:
                raise RuntimeError('Original save changed since first snapshot; old backup preserved.')
        else:
            backup = HERE / 'source_save_backup'
            backup.mkdir(exist_ok=True)
            shutil.copytree(save, backup, dirs_exist_ok=True)
            backup_manifest.write_text(json.dumps(dict(directory=str(save), files=original_save),
                                                  indent=2), encoding='utf-8')
        for label in ('baseline_appdata', 'rebuilt_appdata'):
            target = HERE / label / 'ShanghaiAlice' / 'th20'
            if not target.exists():
                shutil.copytree(HERE / 'source_save_backup', target)
    save_unchanged = None
    if backup_manifest.exists():
        save_unchanged = original_save == json.loads(backup_manifest.read_text(encoding='utf-8'))['files']
    report = dict(source_directory=str(source), source_matches_manifest=current_source == expected_source,
                  original_save_directory=str(save), original_save_files=original_save,
                  original_save_unchanged=save_unchanged, dependencies=dependencies,
                  appdata_isolation_evidence=dict(function='0x0041b4f0', call_va='0x0041b5ca',
                       api='GetEnvironmentVariableW', variable='APPDATA',
                       source='analysis/ghidra/pseudocode/0041b4f0.c:46'),
                  scope='Dependency and source-preservation check; runtime evidence is reported separately.')
    REPORT.write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps(dict(report=str(REPORT), source_matches_manifest=report['source_matches_manifest'],
         save_unchanged=save_unchanged, missing_dependencies=[x['name'] for x in dependencies if not x['exists']],
         prepared=args.prepare), indent=2))
    if not report['source_matches_manifest'] or (args.verify_originals and save_unchanged is not True):
        raise SystemExit(1)


if __name__ == '__main__':
    main()
