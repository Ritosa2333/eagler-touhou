#!/usr/bin/env python3
"""Verify window goldens against fresh original drawing, then update structure hashes.

Both sides are compared on the full drawing stream, including vertices.
Only original structure hashes at the fixture's existing frame numbers are
written. Raw captures stay outside Git and are removed after verification.
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import time


def sha256(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=2)
    parser.add_argument('--harness', default='ports/th16/tests/build/harness')
    parser.add_argument('--output', default='artifacts/th16/merge-readiness/draw-windows')
    parser.add_argument('--file', action='append')
    parser.add_argument('--resume', action='store_true')
    parser.add_argument('--update', action='store_true')
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    os.chdir(Path(__file__).resolve().parents[4])
    directory = Path('ports/th16/tests/golden')
    files = [directory / f for f in args.file] if args.file else sorted(directory.glob('draw-*.json'))
    fixtures = []
    for file in files:
        fixture = json.loads(file.read_text(encoding='utf-8'))
        if 'config' in fixture and 'stage' in fixture['config'] and 'frames' in fixture:
            fixtures.append((file, fixture))
    if not fixtures:
        parser.error('No window fixtures selected')
    output = Path(args.output)
    output.mkdir(parents=True, exist_ok=True)
    harness = Path(args.harness).resolve()
    exe = Path('games/th16/original/th16.exe')
    dat = exe.with_suffix('.dat')
    tool = Path(__file__).parent
    oracle = tool / 'game-oracle.py'
    compare = tool / 'draw-compare.py'
    identity = {'exe_sha256': sha256(exe), 'dat_sha256': sha256(dat),
                'harness_sha256': sha256(harness),
                'oracle_sources': {str(p): sha256(p) for p in
                                   [oracle, tool / 'anm-oracle.py', tool / 'd3d-recorder.py', compare]}}
    env = {k: v for k, v in os.environ.items() if not k.startswith('TH16_')}

    def run(job):
        file, fixture = job
        started = time.monotonic()
        stem = output / file.stem
        record = stem.with_suffix('.result.json')
        config = fixture['config']
        wanted = sorted(map(int, fixture['frames']))
        settings = {'config': config, 'frames': wanted, 'texture_slots': fixture.get('texture_slots')}
        if args.resume and record.exists():
            cached = json.loads(record.read_text())
            if cached['identity'] == identity and cached['settings'] == settings and cached['verified']:
                return file, fixture, cached
        first, last = wanted[0], wanted[-1]
        original = stem.with_suffix('.original.jsonl')
        candidate = stem.with_suffix('.candidate.jsonl')
        flags = []
        for key in ['stage', 'shot', 'season', 'difficulty', 'replay_slot', 'lives', 'mode']:
            if key in config:
                flags += ['--' + key.replace('_', '-'), str(config[key])]
        flags += ['--input', config['input'], '--draw', '--draw-from', str(first),
                  '--draw-out', str(original), '--every', str(2**31 - 1)]
        if config.get('refill', 0):
            flags += ['--refill-lives']
        with stem.with_suffix('.original.log').open('w') as log:
            subprocess.run([sys.executable, str(oracle), str(exe), 'artifacts/th16/dat',
                            str(last + 1), str(stem.with_suffix('.scene.json')), *flags],
                           env=env, stdout=log, stderr=log, check=True)
        if 'errors=[]' not in stem.with_suffix('.original.log').read_text():
            raise RuntimeError(f'{file.name}: original oracle reported errors')
        with stem.with_suffix('.candidate.log').open('w') as log:
            subprocess.run([str(harness), 'game-draw', str(dat), str(config['stage']), str(config['shot']),
                            str(config['season']), str(last + 1), config['input'], '', '',
                            str(config['difficulty']), str(config.get('replay_slot', -1)),
                            str(config.get('lives', 8)), str(config.get('refill', 0)), str(config.get('mode', 0)),
                            str(candidate), str(first)],
                           env=env, stdout=subprocess.DEVNULL, stderr=log, check=True)
        comparison = subprocess.run([sys.executable, str(compare), str(original), str(candidate),
                                     '--frames', f'{first}-{last}', '--eps', '0'],
                                    env=env, capture_output=True, text=True)
        stem.with_suffix('.compare.log').write_text(comparison.stdout + comparison.stderr)
        result = {'identity': identity, 'settings': settings, 'verified': comparison.returncode == 0,
                  'original_sha256': sha256(original), 'candidate_sha256': sha256(candidate),
                  'comparison': comparison.stdout.strip()}
        if result['verified']:
            hashes = stem.with_suffix('.hashes.json')
            flags = ['--frames', f'{first}-{last}', '--structure-golden', str(hashes)]
            if settings['texture_slots'] is not None:
                flags += ['--only-textures', ','.join(map(str, settings['texture_slots']))]
            subprocess.run([sys.executable, str(compare), str(original), '-', *flags],
                           env=env, stdout=subprocess.DEVNULL, check=True)
            all_frames = json.loads(hashes.read_text())['frames']
            result['frames'] = {str(n): all_frames[str(n)] for n in wanted}
            original.unlink()
            candidate.unlink()
        result['seconds'] = round(time.monotonic() - started, 3)
        record.write_text(json.dumps(result, indent=2) + '\n')
        print(file.name, 'MATCH' if result['verified'] else 'DIFF', result['seconds'], flush=True)
        return file, fixture, result

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        results = list(pool.map(run, fixtures))
    report = {'identity': identity, 'cases': len(results), 'verified': sum(r['verified'] for _, _, r in results),
              'results': [r for _, _, r in results]}
    (output / 'summary.json').write_text(json.dumps(report, indent=2) + '\n')
    if report['verified'] != len(results):
        print('Drawing differs. Goldens unchanged.', file=sys.stderr)
        return 1
    if args.update:
        for file, fixture, result in results:
            fixture['frames'] = result['frames']
            file.write_text(json.dumps(fixture, ensure_ascii=False, indent=1) + '\n')
    print(f'Verified {len(results)} drawing windows; updated={args.update}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
