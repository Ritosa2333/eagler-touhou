"""Verify a fresh source snapshot's real MSBuild dependency closure."""
import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET

parser = argparse.ArgumentParser()
parser.add_argument('snapshot', type=Path)
args = parser.parse_args()
root = args.snapshot.resolve()
manifest_path = root / 'SOURCE_SNAPSHOT.json'
manifest = json.loads(manifest_path.read_text(encoding='utf-8'))
for relative, expected in manifest['files'].items():
    path = (root / relative).resolve()
    if not path.is_relative_to(root):
        raise SystemExit(f'Out-of-snapshot path: {relative}')
    if hashlib.sha256(path.read_bytes()).hexdigest() != expected:
        raise SystemExit(f'Snapshot hash mismatch: {relative}')

projects, sources = set(), set()
def inspect(project):
    project = project.resolve()
    if not project.is_relative_to(root / 'build_clean'):
        raise SystemExit(f'External build project: {project}')
    if project in projects:
        return
    projects.add(project)
    for element in ET.parse(project).iter():
        kind = element.tag.rsplit('}', 1)[-1]
        value = element.get('Include')
        if not value or kind not in ('ClCompile', 'ResourceCompile', 'ProjectReference'):
            continue
        path = (project.parent / value).resolve()
        if kind == 'ProjectReference':
            inspect(path)
        else:
            if not path.is_relative_to(root):
                raise SystemExit(f'External compilation input: {path}')
            relative = path.relative_to(root).as_posix()
            if relative not in manifest['files']:
                raise SystemExit(f'Compilation input missing from snapshot manifest: {relative}')
            sources.add(relative)

inspect(root / 'build_clean/link_probe/th20_source.vcxproj')
executable = root / 'build_clean/source_game/RelWithDebInfo/th20_source.exe'
if not executable.is_file() or executable.read_bytes()[:2] != b'MZ':
    raise SystemExit('Independent executable missing')
result = {'status': 'fresh_source_build_verified', 'configuration': 'Win32 RelWithDebInfo',
          'compiled_input_count': len(sources), 'project_count': len(projects),
          'compiled_sources': sorted(sources),
          'executable_sha256': hashlib.sha256(executable.read_bytes()).hexdigest(),
          'all_compilation_inputs_inside_snapshot': True,
          'full_game_equivalence_verified': False,
          'scope': 'Fresh build and exact source-file hashes; not whole-game or replay equivalence'}
report = root / 'SOURCE_BUILD_VERIFICATION.json'
report.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
manifest['files'][report.name] = hashlib.sha256(report.read_bytes()).hexdigest()
manifest_path.write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
print(json.dumps({k: v for k, v in result.items() if k != 'compiled_sources'}))
