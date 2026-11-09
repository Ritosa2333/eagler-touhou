from pathlib import Path
import re
import subprocess
import sys

root = Path(__file__).resolve().parents[2]
native = Path(sys.argv[1]).resolve()
output = Path(sys.argv[2]).resolve()
output.mkdir(parents=True, exist_ok=True)
cache = (native / 'CMakeCache.txt').read_text(encoding='utf-8')
clang = re.search(r'^CMAKE_CXX_COMPILER:STRING=(.+)$', cache, re.M)[1]
line = next(line for line in (native / 'build.ninja').read_text(encoding='utf-8').splitlines()
            if line.startswith('build th17-team-spirits.exe:'))
objects = line.split(' | ')[0].split()[3:]
assert objects[0].endswith('team-spirits.cpp.obj')
game = root / 'ports/th17-mp/web/cpp/game'
multiplayer = game.parent / 'multiplayer'
compile_flags = ['-std=c++17', '-O2', '-DNDEBUG', '-ffp-contract=off',
                 '-I' + str(multiplayer), '-I' + str(game), '-I' + str(root / 'common/include')]


def run(args):
    subprocess.run([clang, '-fms-runtime-lib=dll', *map(str, args)], cwd=native, check=True)


driver = output / 'allocator-bench.obj'
run([*compile_flags, '-c', Path(__file__).with_name('allocator-bench.cpp').resolve(), '-o', driver])
sources = ('AnimationState.cpp', 'ProjectileState.cpp')
remaining = [obj for obj in objects[1:] if not any(obj.endswith(source + '.obj') for source in sources)]
for name in ('baseline', 'reused'):
    variant = []
    for source in sources:
        compiled = output / f'{name}-{source}.obj'
        local = multiplayer / source
        if name == 'baseline':
            local = output / f'baseline-{source}'
            local.write_bytes(subprocess.check_output(
                ['git', 'show', f'5f6a4b47f:ports/th17-mp/web/cpp/multiplayer/{source}'], cwd=root))
        run([*compile_flags, '-c', local, '-o', compiled])
        variant.append(compiled)
    run(['-nostartfiles', '-nostdlib', '-Xlinker', '/subsystem:console',
         driver, *variant, *remaining, '-lkernel32', '-loldnames', '-o', output / f'{name}-allocator-bench.exe'])
