"""使用固定原程序采样比较 WASM 输出，并报告首个采样差异帧和字段。"""
from pathlib import Path
import argparse
import hashlib
import importlib.util
import json
import subprocess
import time
import zipfile

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('replay_reference', ROOT / 'scripts/replay-reference.py')
reference_module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(reference_module)

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, required=True)
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument('--case', action='append')
    selection.add_argument('--all', action='store_true')
    parser.add_argument('--output', type=Path)
    parser.add_argument('--inject', help='比较器反例：逻辑帧:区域名:区域内字节偏移；只修改待比较的内存副本')
    args = parser.parse_args()
    archive = args.archive.resolve()
    index = json.loads((archive / 'index.json').read_text('utf-8'))
    selected = args.case or ['corpus-05', 'omniverse-01', 'ex-return']
    unknown = set(selected) - {row['id'] for row in index['cases']}
    if unknown and not args.all:
        parser.error('未知用例：' + ', '.join(sorted(unknown)))
    cases = [row for row in index['cases'] if args.all or row['id'] in selected]
    injection = None
    if args.inject:
        if len(cases) != 1:
            parser.error('差异注入需选择单个用例')
        frame, region, offset = args.inject.split(':')
        injection = {'frame': int(frame), 'region': region, 'offset': int(offset)}
        if injection['frame'] < 0 or injection['offset'] < 0:
            parser.error('差异注入偏移无效')
    build = ROOT / 'build/web'
    run = args.output.resolve() if args.output else ROOT / 'build/verification' / ('wasm-' + str(time.time_ns()))
    run.mkdir(parents=True, exist_ok=False)
    report = {'schema': 'thkkd/wasm-regression/1', 'result': 'running', 'cases': [],
              'build': json.loads((build / 'probe-build.json').read_text('utf-8')),
              'scope': 'fixed native snapshots and termination stream', 'injection': injection}
    start = time.monotonic()
    original_files = {}
    with zipfile.ZipFile(archive / 'original-baseline.zip') as package:
        for name in package.namelist():
            if name.lower().endswith(('.exe', '.dxa')):
                with package.open(name) as source:
                    original_files[Path(name).name] = hashlib.file_digest(source, 'sha256').hexdigest()
    for case in cases:
        input_path = run / (case['id'] + '.input.bin')
        digest, reference_digest = hashlib.sha256(), hashlib.sha256()
        size = reference_size = 0
        difference = None
        injected = False
        with zipfile.ZipFile(archive / case['archive']) as package:
            input_bytes = package.read(case['input'])
            input_path.write_bytes(input_bytes)
            reference = reference_module.NativeReference(package, case, input_bytes)
            identity = reference.identity()
            if identity['exe'] not in original_files.values() or any(original_files.get(name) != sha for name, sha in identity['resources'].items()):
                raise ValueError('原程序或资源与采样身份不一致：' + case['id'])
            with input_path.open('rb') as source, (run / (case['id'] + '.stderr.txt')).open('wb') as errors:
                process = subprocess.Popen(['node', str(build / 'core-probe.cjs'), case['mode']], stdin=source, stdout=subprocess.PIPE, stderr=errors)
                try:
                    for frame, regions, expected in reference.chunks():
                        data = process.stdout.read(len(expected))
                        if injection and not injected and frame == injection['frame']:
                            at = 4
                            for region in regions:
                                if region['region'] == injection['region']:
                                    if injection['offset'] >= region['bytes'] or at + injection['offset'] >= len(data):
                                        raise ValueError('注入偏移超出候选数据')
                                    data = bytearray(data)
                                    data[at + injection['offset']] ^= 1
                                    injected = True
                                    break
                                at += region['bytes']
                        if difference is None:
                            difference = reference_module.first_difference(expected, data, frame, regions, size)
                        digest.update(data)
                        reference_digest.update(expected)
                        size += len(data)
                        reference_size += len(expected)
                    while data := process.stdout.read(1024 * 1024):
                        if difference is None:
                            difference = {'frame': reference.end, 'field': 'unexpected-output', 'streamOffset': size}
                        digest.update(data)
                        size += len(data)
                    code = process.wait()
                finally:
                    if process.poll() is None:
                        process.kill()
                        process.wait()
                    process.stdout.close()
        actual = {'bytes': size, 'sha256': digest.hexdigest()}
        baseline = {'bytes': reference_size, 'sha256': reference_digest.hexdigest()}
        if baseline != case['expected']:
            raise ValueError('原始采样重建流与固定基准不一致：' + case['id'] + ' ' + json.dumps(baseline))
        if injection and not injected:
            raise ValueError('指定的差异注入位置不存在')
        passed = code == 0 and actual == case['expected'] and difference is None
        row = {'case': case['id'], 'result': 'passed' if passed else 'failed', 'actual': actual,
               'expected': case['expected'], 'exit_code': code, 'firstDifference': difference,
               'nativeIdentity': identity, 'nativeRegionsVerified': reference.regions, 'injected': injected}
        report['cases'].append(row)
        print(json.dumps(row), flush=True)
        if not passed:
            report['result'] = 'failed'
            break
    else:
        report['result'] = 'passed'
    report['seconds'] = round(time.monotonic() - start, 3)
    (run / 'result.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(str(run / 'result.json'))
    if report['result'] != 'passed':
        raise SystemExit(1)

if __name__ == '__main__':
    main()
