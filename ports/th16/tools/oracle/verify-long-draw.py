#!/usr/bin/env python3
"""Recollect long drawing references from the original executable.

Only validated original streams can produce golden segments. Captures store
per-frame digests, not screenshots or raw D3D vertices. Game state snapshots
are disabled because this verifier checks drawing, not the state digests.
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
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def covers_frame_range(segments, first, last):
    if first > last:
        return False
    next_frame = first
    for start, stop, _, _ in segments:
        if start != next_frame or stop < start:
            return False
        next_frame = stop + 1
    return next_frame == last + 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=2)
    parser.add_argument('--case-regex', required=True)
    parser.add_argument('--harness', default='ports/th16/tests/build/harness')
    parser.add_argument('--output', default='artifacts/th16/merge-readiness/long-draw')
    parser.add_argument('--resume', action='store_true')
    parser.add_argument('--update', action='store_true')
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    os.chdir(Path(__file__).resolve().parents[4])
    output = Path(args.output)
    output.mkdir(parents=True, exist_ok=True)
    golden_path = Path('ports/th16/tests/golden/draw-long.json')
    golden = json.loads(golden_path.read_text(encoding='utf-8'))
    cases = [c for c in golden['cases'] if re.search(args.case_regex, c['name'])]
    if not cases:
        parser.error('No cases selected')
    harness = Path(args.harness).resolve()
    exe = Path('games/th16/original/th16.exe')
    dat = exe.with_suffix('.dat')
    tool = Path(__file__).parent
    oracle = tool / 'game-oracle.py'
    compare = tool / 'draw-compare.py'
    identity = {'exe_sha256': sha256(exe), 'dat_sha256': sha256(dat),
                'harness_sha256': sha256(harness),
                'verifier_sha256': sha256(__file__),
                'oracle_sources': {str(p): sha256(p) for p in
                                   [oracle, tool / 'anm-oracle.py', tool / 'd3d-recorder.py', compare]}}
    env = {k: v for k, v in os.environ.items() if not k.startswith('TH16_')}

    def run(case):
        started = time.monotonic()
        stem = output / case['name']
        result_path = stem.with_suffix('.result.json')
        config = {k: v for k, v in case.items() if k not in ('segments', 'verified')}
        if case['input'].startswith('@'):
            config['input_sha256'] = sha256(case['input'][1:])
        original_stream = stem.with_suffix('.original.jsonl')
        port_stream = stem.with_suffix('.candidate.jsonl')
        state = stem.with_suffix('.scene.json')
        cached = None
        if args.resume and result_path.exists():
            cached = json.loads(result_path.read_text())
            reference_id = cached.get('reference_identity', cached['identity'])
            same_reference = all(reference_id[key] == identity[key]
                                 for key in ['exe_sha256', 'dat_sha256', 'oracle_sources'])
            if not (same_reference and cached['config'] == config and state.exists() and original_stream.exists()
                    and sha256(state) == cached.get('scene_sha256')
                    and sha256(original_stream) == cached['original_sha256']):
                cached = None
        first = case.get('from', 1)
        flags = []
        for key in ['stage', 'shot', 'season', 'difficulty', 'replay_slot', 'lives', 'mode']:
            if key in case:
                flags += ['--' + key.replace('_', '-'), str(case[key])]
        flags += ['--input', case['input'], '--draw', '--draw-from', str(first),
                  '--draw-digest', str(original_stream), '--every', str(2**31 - 1)]
        if case.get('refill', 1):
            flags += ['--refill-lives']
        if not cached:
            with stem.with_suffix('.original.log').open('w') as log:
                subprocess.run([sys.executable, '-X', 'utf8', str(oracle), str(exe), 'artifacts/th16/dat',
                                str(case['frames']), str(state), *flags],
                               env=env, stdout=log, stderr=log, check=True)
        log = stem.with_suffix('.original.log').read_bytes()
        if not re.search(rb'errors=\[\]\s*$', log):
            raise RuntimeError(f"{case['name']}: original oracle reported errors")
        exit_frame = json.loads(state.read_text())['exit_frame']
        last = case['frames'] if exit_frame is None else min(case['frames'], exit_frame - 1)
        if (cached and cached['identity'] == identity and cached['verified']
                and covers_frame_range(cached['segments'], first, last)):
            return case, cached
        with stem.with_suffix('.candidate.log').open('w') as log:
            subprocess.run([
                str(harness), 'game-draw', str(dat), str(case['stage']), str(case['shot']),
                str(case['season']), str(case['frames']), case['input'], '', '',
                str(case['difficulty']), str(case.get('replay_slot', -1)),
                str(case.get('lives', 8)), str(case.get('refill', 1)), str(case.get('mode', 0)),
                'digest:' + str(port_stream), str(first),
            ], env=env, stdout=subprocess.DEVNULL, stderr=log, check=True)
        segments = stem.with_suffix('.segments.json')
        comparison = subprocess.run([sys.executable, '-X', 'utf8', str(compare), str(original_stream), str(port_stream),
                                     '--compare-digests', '--golden-segments', str(segments)],
                                    env=env, capture_output=True, text=True, encoding='utf-8')
        diagnostics = comparison.stdout + comparison.stderr
        matched = json.loads(segments.read_text())['segments'] if comparison.returncode == 0 else []
        verified = comparison.returncode == 0 and covers_frame_range(matched, first, last)
        if comparison.returncode == 0 and not verified:
            diagnostics += f'Matched segments do not cover expected frames {first}–{last}.\n'
        stem.with_suffix('.compare.log').write_text(diagnostics, encoding='utf-8')
        result = {'identity': identity, 'config': config, 'verified': verified,
                  'reference_identity': cached.get('reference_identity', cached['identity']) if cached else identity,
                  'scene_sha256': sha256(state),
                  'original_sha256': sha256(original_stream), 'candidate_sha256': sha256(port_stream),
                  'comparison': diagnostics.strip(),
                  'seconds': round(time.monotonic() - started, 3)}
        if result['verified']:
            result['segments'] = matched
        result_path.write_text(json.dumps(result, indent=2) + '\n')
        print(case['name'], result['comparison'], 'seconds=' + str(result['seconds']), flush=True)
        return case, result

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        results = list(pool.map(run, cases))
    report = {'identity': identity, 'cases': len(results),
              'verified': sum(r['verified'] for _, r in results),
              'results': [r for _, r in results]}
    (output / 'summary.json').write_text(json.dumps(report, indent=2) + '\n')
    if report['verified'] != len(results):
        print('Drawing differs. Goldens unchanged.', file=sys.stderr)
        return 1
    if args.update:
        for case, result in results:
            case['segments'] = result['segments']
            case['verified'] = True
        golden_path.write_text(json.dumps(golden, ensure_ascii=False, indent=1) + '\n')
    print(f"Verified {len(results)} long drawing cases; updated={args.update}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
