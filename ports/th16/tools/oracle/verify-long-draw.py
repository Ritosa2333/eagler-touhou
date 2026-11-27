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


def read_cached(path):
    try:
        return json.loads(path.read_text(encoding='utf-8'))
    except (FileNotFoundError, json.JSONDecodeError):
        return None


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
    draw_tools = tool.parent
    digest_script = draw_tools / 'draw-digest.mjs'
    identity = {'exe_sha256': sha256(exe), 'dat_sha256': sha256(dat),
                'harness_sha256': sha256(harness),
                'verifier_sha256': sha256(__file__),
                'oracle_sources': {str(p): sha256(p) for p in
                                   [oracle, tool / 'anm-oracle.py', tool / 'd3d-recorder.py',
                                    tool / 'music-oracle.py', tool / 'sound-oracle.py', compare,
                                    digest_script, draw_tools / 'draw-structure.mjs']}}
    env = {k: v for k, v in os.environ.items() if not k.startswith('TH16_')}

    def run(case):
        started = time.monotonic()
        stem = output / case['name']
        result_path = stem.with_suffix('.result.json')
        reference_path = stem.with_suffix('.reference.json')
        config = {k: v for k, v in case.items()
                  if k not in ('segments', 'state_digests', 'uninterrupted_bgm', 'verified', 'exit_frame')}
        if case['input'].startswith('@'):
            config['input_sha256'] = sha256(case['input'][1:])
        original_stream = stem.with_suffix('.original.jsonl')
        port_stream = stem.with_suffix('.candidate.jsonl')
        state = stem.with_suffix('.scene.json')
        cached = read_cached(result_path) if args.resume else None
        reference = None
        if args.resume:
            for metadata in (read_cached(reference_path), cached):
                if not metadata:
                    continue
                reference_id = metadata.get('reference_identity', metadata.get('identity', {}))
                same_reference = all(reference_id.get(key) == identity[key]
                                     for key in ['exe_sha256', 'dat_sha256', 'oracle_sources'])
                if (same_reference and metadata.get('config') == config and state.is_file() and original_stream.is_file()
                        and sha256(state) == metadata.get('scene_sha256')
                        and sha256(original_stream) == metadata.get('original_sha256')):
                    reference = {**metadata, 'reference_identity': reference_id}
                    break
        first = case.get('from', 1)
        flags = []
        for key in ['stage', 'shot', 'season', 'difficulty', 'replay_slot', 'lives', 'mode']:
            if key in case:
                flags += ['--' + key.replace('_', '-'), str(case[key])]
        flags += ['--input', case['input'], '--draw', '--draw-from', str(first),
                  '--draw-ops', '-', '--every', str(2**31 - 1)]
        if case.get('refill', 1):
            flags += ['--refill-lives']
        if case.get('track_music', False):
            flags += ['--track-music']
        original_log = stem.with_suffix('.original.log')
        if reference is None:
            with original_log.open('w') as log:
                command = [sys.executable, '-X', 'utf8', str(oracle), str(exe), 'artifacts/th16/dat',
                           str(case['frames']), str(state), *flags]
                with subprocess.Popen(command, env=env, stdout=subprocess.PIPE, stderr=log) as oracle_process:
                    try:
                        digest = subprocess.run(['node', str(digest_script), '-', str(original_stream)],
                                                env=env, stdin=oracle_process.stdout, stdout=log, stderr=log)
                    finally:
                        oracle_process.stdout.close()
                    status = oracle_process.wait()
                    digest.check_returncode()
                    if status:
                        raise subprocess.CalledProcessError(status, command)
        log = original_log.read_bytes()
        if not re.search(rb'errors=\[\]\s*$', log):
            raise RuntimeError(f"{case['name']}: original oracle reported errors")
        exit_frame = json.loads(state.read_text())['exit_frame']
        if reference is None:
            reference = {'reference_identity': identity, 'config': config,
                         'scene_sha256': sha256(state), 'original_sha256': sha256(original_stream)}
            temporary = reference_path.with_suffix('.tmp')
            temporary.write_text(json.dumps(reference, indent=2) + '\n', encoding='utf-8')
            temporary.replace(reference_path)
        last = case['frames'] if exit_frame is None else min(case['frames'], exit_frame - 1)
        if (cached and cached['identity'] == identity and cached['verified']
                and cached.get('exit_frame') == exit_frame
                and cached['config'] == config
                and cached['original_sha256'] == reference['original_sha256']
                and cached['scene_sha256'] == reference['scene_sha256']
                and covers_frame_range(cached['segments'], first, last)
                and port_stream.is_file() and sha256(port_stream) == cached.get('candidate_sha256')):
            return case, cached
        with stem.with_suffix('.candidate.log').open('w') as log:
            subprocess.run([
                str(harness), 'game-draw', str(dat), str(case['stage']), str(case['shot']),
                str(case['season']), str(case['frames']), case['input'], '', '',
                str(case['difficulty']), str(case.get('replay_slot', -1)),
                str(case.get('lives', '')), str(case.get('refill', 1)), str(case.get('mode', 0)),
                'digest:' + str(port_stream), str(first), '--draw-only',
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
        result = {'identity': identity, 'config': config, 'verified': verified, 'exit_frame': exit_frame,
                  'reference_identity': reference['reference_identity'],
                  'scene_sha256': reference['scene_sha256'],
                  'original_sha256': reference['original_sha256'], 'candidate_sha256': sha256(port_stream),
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
            if result['exit_frame'] is None:
                case.pop('exit_frame', None)
            else:
                case['exit_frame'] = result['exit_frame']
        golden_path.write_text(json.dumps(golden, ensure_ascii=False, indent=1) + '\n')
    print(f"Verified {len(results)} long drawing cases; updated={args.update}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
