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


def prepare_host_shell(source, identity):
    """Specialize host paths without renaming the shared native C exports."""
    for before, after in [('./th11-sdl.', './th14-sdl.'), ("game='th11'", "game='th14'"),
                          ('/savesth11', '/savesth14'), ('/thcrap/th11/', '/thcrap/th14/'),
                          ('scoreth11', 'scoreth14'), ('th11.cfg', 'th14.cfg'),
                          (r'th11\.cfg', r'th14\.cfg'), ('th11_(?:', 'th14_(?:'),
                          ('TH11', 'TH14'), ('地灵殿', '辉针城')]:
        source = source.replace(before, after)
    anchor = 'const runtimeBuild=/*TH14_BUILD_INFO*/'
    assert source.count(anchor) == 1
    start = source.index(anchor) + len(anchor)
    end = source.index(';', start)
    source = source[:start] + json.dumps(identity, ensure_ascii=False) + source[end:]
    anchor = ' await mountData();'
    assert source.count(anchor) == 1
    source = source.replace(anchor, anchor + "\n Module.FS.symlink('/th14.dat','/th11.dat');", 1)
    assert "game='th14'" in source and "core?.th11_key(" in source
    return source


def main():
    repo = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--emsdk', type=Path, default=os.environ.get('EMSDK', repo / 'tools/emsdk'))
    parser.add_argument('--common', type=Path, default=os.environ.get('EAGLER_COMMON', repo.parents[1] / 'common'))
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--jobs', type=int, choices=range(1, 17), default=3)
    args = parser.parse_args()
    sdk = args.emsdk.resolve()
    compiler = sdk / 'upstream/emscripten/emcc.py'
    if not compiler.is_file():
        compiler = sdk / 'install/emscripten/emcc.py'
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
    flags = profile['flags'][:]
    common = args.common.resolve()
    imgui = common / 'third_party/imgui'
    shared = common / 'include'
    if not (imgui / 'imgui.cpp').is_file() or not (shared / 'eagler/thprac/PracticeMobilePanels.hpp').is_file():
        parser.error('Pass --common pointing to the launcher common directory')
    sources += [imgui / name for name in ('imgui.cpp','imgui_draw.cpp','imgui_tables.cpp','imgui_widgets.cpp','imgui_freetype.cpp')]
    flags += ['-I' + str(imgui), '-I' + str(shared), '-DIMGUI_DISABLE_WIN32_FUNCTIONS']
    tracked = set(source for source in sources if source.is_relative_to(repo))
    common_files = [*sorted(imgui.glob('*.h')), *sorted(imgui.glob('*.inl')), *sorted(imgui.glob('*.cpp')),
                    *sorted((shared / 'eagler/thprac').glob('Practice*')),
                    shared / 'eagler/input/DialogueGesture.hpp']
    common_bindings = {p.relative_to(common).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest() for p in common_files}
    for directory in (cpp, runtime / 'portable'):
        tracked.update(p for p in directory.rglob('*')
                       if p.is_file() and p.suffix in {'.hpp', '.h', '.inc', '.js', '.mjs'})
    host = runtime / 'th12_web/sdl-runtime/eagler-host.mjs'
    host_shell = host.with_name('shell.mjs')
    tracked.update((Path(__file__).resolve(), repo / 'tools/th14-gles-profile.json', host, host_shell))
    source_bindings = {p.relative_to(repo).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
                       for p in sorted(tracked)}

    def run(flags):
        subprocess.run([sys.executable, str(compiler), *flags], check=True, cwd=runtime, env=env)

    def compile_source(source):
        obj = objects / ((source.relative_to(runtime).as_posix().replace('/', '_') if source.is_relative_to(runtime) else source.name) + '.o')
        # Profiling function names are a linker option.
        run([f for f in flags if f != '--profiling-funcs'] + ['-c', str(source), '-o', str(obj)])
        return str(obj)

    link_flags = [str(portable / 'Th14WebGpuLibrary.js') if f == 'portable/sdl/Th14WebGpuLibrary.js' else f
                  for f in profile['linkFlags']]
    with tempfile.TemporaryDirectory(dir=output, prefix='emsdk-') as temp:
        env.update(TMP=temp, TMPDIR=temp, TEMP=temp)
        with ThreadPoolExecutor(max_workers=args.jobs) as pool:
            compiled = list(pool.map(compile_source, sources))
        response = Path(temp) / 'link.rsp'
        response.write_text('\n'.join(json.dumps(x.replace('\\','/')) for x in [*flags, *compiled, *link_flags, '-o', str(output / 'th14-sdl.mjs')]),encoding='utf8')
        run(['@'+str(response)])
    for helper in profile['helpers']:
        shutil.copyfile(portable / helper, output / helper)
    shutil.copyfile(host, output / host.name)
    wasm_hash = hashlib.sha256((output / 'th14-sdl.wasm').read_bytes()).hexdigest()
    shell = prepare_host_shell(host_shell.read_text(encoding='utf8'), dict(version='1.00b-thp-' + wasm_hash[:12], completeGame=False, nativeEquivalent=False, sha256=wasm_hash))
    (output / 'shell.mjs').write_text(shell, encoding='utf8', newline='\n')
    for name, expected in source_bindings.items():
        if hashlib.sha256((repo / name).read_bytes()).hexdigest() != expected:
            raise RuntimeError('Source changed during build: ' + name)
    for name, expected in common_bindings.items():
        if hashlib.sha256((common / name).read_bytes()).hexdigest() != expected:
            raise RuntimeError('Shared source changed during build: ' + name)
    wasm = output / 'th14-sdl.wasm'
    runtime_files = {}
    for name in ['th14-sdl.mjs', 'th14-sdl.wasm', host.name, 'shell.mjs', *profile['helpers']]:
        data = (output / name).read_bytes()
        runtime_files[name] = dict(bytes=len(data), sha256=hashlib.sha256(data).hexdigest())
    report = dict(module=str(wasm), units=len(compiled), sourceBindings=source_bindings,
                  objects={str(Path(p).relative_to(output)): hashlib.sha256(Path(p).read_bytes()).hexdigest()
                           for p in compiled}, flags=flags, linkFlags=link_flags, commonBindings=common_bindings,
                  runtimeFiles=runtime_files, sha256=runtime_files[wasm.name]['sha256'],
                  loaderSha256=runtime_files['th14-sdl.mjs']['sha256'])
    (output / 'build.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(dict(module=str(wasm), units=len(compiled), sha256=report['sha256'])))


if __name__ == '__main__':
    main()
