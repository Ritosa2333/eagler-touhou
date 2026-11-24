import argparse
import json
import os
import pathlib
import shutil
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--emcc', type=pathlib.Path)
    parser.add_argument('--real', action='store_true')
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--probe-normalization', action='store_true')
    args = parser.parse_args()
    root = pathlib.Path(__file__).resolve().parents[1]
    emcc = args.emcc or root.parents[1] / 'work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'
    if not emcc.is_file():
        print(f'Toolchain not ready: {emcc}', file=sys.stderr)
        return 2
    build = root / 'tests/animation-build'
    build.mkdir(exist_ok=True)
    output = build / 'fixtures.cjs'
    sources = root / 'src/animation'
    compiler_python = emcc.parents[2] / 'python/3.13.3_64bit/python.exe'
    if not compiler_python.is_file():
        raise RuntimeError('expected bundled Emscripten Python 3.13.3')
    command = [compiler_python.as_posix(), emcc.as_posix(), '-std=c++17', '-O1', '-Wall', '-Wextra',
               '-Werror', '-fexceptions', '-sASSERTIONS=2', '-sALLOW_MEMORY_GROWTH=1',
               '-sENVIRONMENT=node', '-sNODERAWFS=1', '-sSTACK_SIZE=4194304',
               '-I', str(sources), str(root / 'tests/animation-vm.cpp'),
               str(sources / 'AnmLogic.cpp'), str(sources / 'AnmScene.cpp'), '-o', str(output)]
    if args.sanitize:
        command += ['-fsanitize=undefined', '-fno-sanitize-recover=all']
    print(subprocess.list2cmdline(command), flush=True)
    environment = os.environ.copy()
    config = emcc.parents[2] / '.emscripten'
    if config.is_file():
        environment['EM_CONFIG'] = config.as_posix()
    environment['EMSDK_PYTHON'] = compiler_python.as_posix()
    environment['EM_CACHE'] = (build / 'cache').as_posix()
    environment['EMCC_CORES'] = '2'
    temporary = build / 'tmp'
    temporary.mkdir(exist_ok=True)
    environment['TEMP'] = environment['TMP'] = environment['TMPDIR'] = temporary.as_posix()
    subprocess.run(command, check=True, env=environment)
    command = [shutil.which('node') or 'node', str(output)]
    if args.real:
        catalog_path = root / 'local/catalog.json'
        catalog = json.loads(catalog_path.read_text(encoding='utf-8'))
        manifest_path = catalog_path.parent / catalog['variants']['ja']['manifest']
        manifest = json.loads(manifest_path.read_text(encoding='utf-8'))
        for animation in manifest['animations']:
            command += ['--bank', str(manifest_path.parent / 'raw' / animation['name'])]
        command += ['--title', str(manifest_path.parent / 'raw/title.anm')]
    probe = command[:2] + ['--normalization-probe']
    if args.probe_normalization:
        command = command[:2] + ['--normalization-probe']
    try:
        if not args.probe_normalization:
            subprocess.run(probe, timeout=10, check=True)
        return subprocess.run(command, timeout=5 if args.probe_normalization else 60).returncode
    except subprocess.TimeoutExpired:
        print('FAIL animation fixture exceeded execution time budget', file=sys.stderr)
        return 124


if __name__ == '__main__':
    raise SystemExit(main())
