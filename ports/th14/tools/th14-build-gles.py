"""Build the TH14 SDL3/WebGL2 module from repository sources with Emscripten."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


def main():
    repo = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--emsdk', type=Path, default=os.environ.get('EMSDK', repo / 'tools/emsdk'))
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--jobs', type=int, choices=range(1, 17), default=3)
    args = parser.parse_args()
    sdk = args.emsdk.resolve()
    compiler = sdk / 'upstream/emscripten/emcc.py'
    if not compiler.is_file():
        parser.error('An activated Emscripten SDK is required (--emsdk or EMSDK).')
    output = args.out.resolve()
    objects = output / 'objects'
    objects.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, EMSDK=str(sdk), EMSDK_PYTHON=sys.executable, EMCC_CORES=str(args.jobs))
    if (sdk / '.emscripten').is_file():
        env['EM_CONFIG'] = str(sdk / '.emscripten')
    runtime = repo / 'runtimes/th12'
    portable = runtime / 'portable/sdl'
    cpp = runtime / 'th12_web/cpp'
    profile = json.loads((repo / 'tools/th14-gles-profile.json').read_bytes())
    sources = [portable / 'Renderer.cpp', *sorted((cpp / 'game').glob('*.cpp'), key=lambda p: p.name),
               *sorted((cpp / 'sdl').glob('*.cpp'), key=lambda p: p.name)]
    tracked = set(sources)
    for directory in (cpp, runtime / 'portable'):
        tracked.update(p for p in directory.rglob('*')
                       if p.is_file() and p.suffix in {'.hpp', '.h', '.inc', '.js', '.mjs'})
    tracked.update((Path(__file__).resolve(), repo / 'tools/th14-gles-profile.json'))
    source_bindings = {p.relative_to(repo).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
                       for p in sorted(tracked)}

    def run(flags):
        subprocess.run([sys.executable, str(compiler), *flags], check=True, cwd=runtime, env=env)

    def compile_source(source):
        obj = objects / (source.relative_to(runtime).as_posix().replace('/', '_') + '.o')
        # Profiling function names are a linker option.
        run([f for f in profile['flags'] if f != '--profiling-funcs'] + ['-c', str(source), '-o', str(obj)])
        return str(obj)

    link_flags = [str(portable / 'Th14WebGpuLibrary.js') if f == 'portable/sdl/Th14WebGpuLibrary.js' else f
                  for f in profile['linkFlags']]
    with tempfile.TemporaryDirectory(dir=output, prefix='emsdk-') as temp:
        env.update(TMP=temp, TMPDIR=temp, TEMP=temp)
        with ThreadPoolExecutor(max_workers=args.jobs) as pool:
            compiled = list(pool.map(compile_source, sources))
        run([*profile['flags'], *compiled, *link_flags, '-o', str(output / 'th14-sdl.mjs')])
    for helper in profile['helpers']:
        shutil.copyfile(portable / helper, output / helper)
    for name, expected in source_bindings.items():
        if hashlib.sha256((repo / name).read_bytes()).hexdigest() != expected:
            raise RuntimeError('Source changed during build: ' + name)
    wasm = output / 'th14-sdl.wasm'
    report = dict(module=str(wasm), units=len(compiled), sourceBindings=source_bindings,
                  objects={str(Path(p).relative_to(output)): hashlib.sha256(Path(p).read_bytes()).hexdigest()
                           for p in compiled}, flags=profile['flags'], linkFlags=link_flags,
                  sha256=hashlib.sha256(wasm.read_bytes()).hexdigest(),
                  loaderSha256=hashlib.sha256((output / 'th14-sdl.mjs').read_bytes()).hexdigest())
    (output / 'build.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(dict(module=str(wasm), units=len(compiled), sha256=report['sha256'])))


if __name__ == '__main__':
    main()
