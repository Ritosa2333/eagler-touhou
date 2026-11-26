"""用固定原始输入检查 WASM 输出，逐字节比较既有不可变状态摘要。"""
from pathlib import Path
import argparse
import hashlib
import json
import shutil
import subprocess
import time
import zipfile

ROOT = Path(__file__).resolve().parents[1]

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, required=True)
    parser.add_argument('--case', action='append')
    parser.add_argument('--all', action='store_true')
    args = parser.parse_args()
    archive = args.archive.resolve()
    index = json.loads((archive / 'index.json').read_text('utf-8'))
    cases = [row for row in index['cases'] if args.all or row['id'] in (args.case or ['corpus-05', 'omniverse-01', 'ex-return'])]
    build = ROOT / 'build/web'
    run = archive / 'work' / ('wasm-verify-' + str(time.time_ns()))
    run.mkdir(parents=True)
    report = {'schema': 'thkkd/wasm-regression/1', 'result': 'running', 'cases': [],
              'build': json.loads((build / 'probe-build.json').read_text('utf-8'))}
    start = time.monotonic()
    for case in cases:
        input_path = run / (case['id'] + '.input.bin')
        with zipfile.ZipFile(archive / case['archive']) as package:
            names = {name.casefold(): name for name in package.namelist()}
            with package.open(names[case['input'].casefold()]) as source, input_path.open('wb') as target:
                shutil.copyfileobj(source, target, 1024 * 1024)
        digest = hashlib.sha256()
        size = 0
        with input_path.open('rb') as source, (run / (case['id'] + '.stderr.txt')).open('wb') as errors:
            process = subprocess.Popen(['node', str(build / 'core-probe.cjs'), case['mode']], stdin=source, stdout=subprocess.PIPE, stderr=errors)
            while data := process.stdout.read(1024 * 1024):
                digest.update(data)
                size += len(data)
            code = process.wait()
        actual = {'bytes': size, 'sha256': digest.hexdigest()}
        passed = code == 0 and actual == case['expected']
        row = {'case': case['id'], 'result': 'passed' if passed else 'failed', 'actual': actual, 'expected': case['expected'], 'exit_code': code}
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
