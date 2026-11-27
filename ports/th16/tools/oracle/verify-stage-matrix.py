#!/usr/bin/env python3
"""Recollect matrix goldens from the original EXE, then verify the candidate.

Run from the repository root inside the development container. No golden is
written unless every requested case agrees with the original on every field.
Full oracle snapshots are retained for disagreements; matching cases retain
the oracle log, snapshot SHA-256, candidate output and build/input identities.
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=2)
    parser.add_argument('--harness', default='ports/th16/tests/build/harness')
    parser.add_argument('--python', default=sys.executable)
    parser.add_argument('--output', default='artifacts/th16/merge-readiness/matrix')
    parser.add_argument('--file', action='append', help='Golden filenames; default: all stage matrices')
    parser.add_argument('--case', help='Select one case by name')
    parser.add_argument('--resume', action='store_true')
    parser.add_argument('--stream', action='store_true', help='Bounded-memory original oracle')
    parser.add_argument('--update', action='store_true')
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    root = Path(__file__).resolve().parents[4]
    os.chdir(root)
    output = Path(args.output)
    output.mkdir(parents=True, exist_ok=True)
    harness = Path(args.harness).resolve()
    oracle_dir = Path('ports/th16/tools/oracle')
    oracle = oracle_dir / ('game-digest-oracle.py' if args.stream else 'game-oracle.py')
    exe = Path('games/th16/original/th16.exe')
    dat = exe.with_suffix('.dat')
    golden_dir = Path('ports/th16/tests/golden')
    files = [golden_dir / name for name in args.file] if args.file else sorted(golden_dir.glob('stage-matrix-st*.json'))
    sources = [oracle_dir / 'game-oracle.py', oracle_dir / 'anm-oracle.py']
    if args.stream:
        sources.append(oracle)
    identity = {
        'exe_sha256': sha256(exe), 'dat_sha256': sha256(dat),
        'harness_sha256': sha256(harness),
        'oracle_sources': {str(p): sha256(p) for p in sources},
    }
    documents = {p: json.loads(p.read_text(encoding='utf-8')) for p in files}
    jobs = [(p, c) for p, doc in documents.items() for c in doc['cases']
            if not args.case or c['name'] == args.case]
    if not jobs:
        parser.error('No cases selected')
    env = {k: v for k, v in os.environ.items() if not k.startswith('TH16_')}

    def run(job):
        path, case = job
        stem = output / case['name']
        result_path = stem.with_suffix('.result.json')
        aliases = {'player_digest': 'digest', 'enemy_digest': 'enemies',
                   'enemy_vm_digest': 'enemy_vms', 'bullet_digest': 'bullets',
                   'item_digest': 'items', 'spell_digest': 'spell',
                   'laser_digest': 'lasers', 'bomb_digest': 'bombs',
                   'menu_digest': 'menu', 'sound_digest': 'sound'}
        if 'digests' in case:
            expected = case['digests']
            field_names = {k: k for k in expected}
        else:
            field_names = {k: aliases.get(k, k) for k in case
                           if k in aliases or k in aliases.values()}
            expected = {v: case[k] for k, v in field_names.items()}
        config_keys = ['name', 'stage', 'shot', 'season', 'frames', 'input', 'difficulty',
                       'replay_slot', 'lives', 'refill', 'mode']
        config = {key: case[key] for key in config_keys if key in case}
        if case['input'].startswith('@'):
            config['input_sha256'] = sha256(case['input'][1:])
        cached = None
        if args.resume and result_path.exists():
            cached = json.loads(result_path.read_text())
            cached_config = {key: value for key, value in cached['config'].items()
                             if key in config_keys or key == 'input_sha256'}
            if cached['identity'] == identity and cached_config == config and cached['verified']:
                cached['config'] = config
                return path, case, cached
            reference_id = cached.get('reference_identity', cached['identity'])
            same_inputs = cached_config == config and all(
                reference_id[k] == identity[k] for k in ('exe_sha256', 'dat_sha256'))
            same_sources = all(Path(p).exists() and sha256(p) == digest
                               for p, digest in reference_id['oracle_sources'].items())
            if not (same_inputs and same_sources):
                cached = None
        started = time.monotonic()
        snapshot = stem.with_suffix('.original.json')
        flags = []
        for key in ('stage', 'shot', 'season', 'difficulty', 'replay_slot', 'lives', 'mode'):
            if key in case:
                flags += ['--' + key.replace('_', '-'), str(case[key])]
        flags += ['--input', case['input'], '--enemies', '--bullets', '--items', '--lasers', '--bombs']
        if case.get('refill', 0):
            flags.append('--refill-lives')
        if cached:
            reference = {key: value['original'] for key, value in cached['fields'].items()}
            snapshot_hash = cached['snapshot_sha256']
            reference_identity = cached.get('reference_identity', cached['identity'])
        else:
            original = subprocess.run([args.python, str(oracle), str(exe), 'artifacts/th16/dat',
                                       str(case['frames']), str(snapshot), *flags],
                                      env=env, capture_output=True, text=True)
            stem.with_suffix('.original.log').write_text(original.stdout + original.stderr)
            original.check_returncode()
            if not re.search(r'errors=\[\]\s*$', original.stdout):
                raise RuntimeError(f"{case['name']}: oracle reported errors; inspect original log")
            reference = dict(re.findall(r'\b([a-z_]+)=([0-9a-f]{16})\b', original.stdout))
            snapshot_hash = sha256(snapshot)
            reference_identity = identity
        candidate = subprocess.check_output([
            str(harness), 'game-digest', str(dat), str(case['stage']), str(case['shot']),
            str(case['season']), str(case['frames']), case['input'], '', '',
            str(case['difficulty']), str(case['replay_slot']),
            str(case['lives']) if 'lives' in case else '', str(case.get('refill', 0)), str(case.get('mode', 0)),
        ], env=env, text=True)
        stem.with_suffix('.candidate.txt').write_text(candidate)
        actual = dict(re.findall(r'\b([a-z_]+)=([0-9a-f]{16})\b', candidate))
        fields = {key: {'old': value, 'original': reference[key], 'candidate': actual[key]}
                  for key, value in expected.items()}
        result = {'identity': identity, 'config': config, 'fields': fields,
                  'field_names': field_names,
                  'reference_identity': reference_identity,
                  'snapshot_sha256': snapshot_hash,
                  'verified': all(v['original'] == v['candidate'] for v in fields.values()),
                  'seconds': round(time.monotonic() - started, 3)}
        result_path.write_text(json.dumps(result, indent=2) + '\n')
        if result['verified'] and snapshot.exists():
            snapshot.unlink()
        print(case['name'], 'MATCH' if result['verified'] else 'DIFF', result['seconds'], flush=True)
        return path, case, result

    results = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for result in pool.map(run, jobs):
            results.append(result)
    report = {'identity': identity, 'cases': len(results),
              'verified': sum(r['verified'] for _, _, r in results),
              'changed': sum(any(v['old'] != v['original'] for v in r['fields'].values()) for _, _, r in results),
              'results': [r for _, _, r in results]}
    (output / 'summary.json').write_text(json.dumps(report, indent=2) + '\n')
    if report['verified'] != report['cases']:
        print('Candidate differs from original. Goldens unchanged.', file=sys.stderr)
        return 1
    if args.update:
        for path, case, result in results:
            if 'digests' in case:
                case['digests'] = {k: v['original'] for k, v in result['fields'].items()}
            else:
                for field, key in result['field_names'].items():
                    case[field] = result['fields'][key]['original']
        for path, doc in documents.items():
            path.write_text(json.dumps(doc, ensure_ascii=False, indent=1) + '\n')
    print(f"Verified {report['verified']}/{report['cases']}; changed {report['changed']}; updated={args.update}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
