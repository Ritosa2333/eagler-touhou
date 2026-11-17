"""Compare TH14 1.00b ANM VM update traces and retail archive opcode usage."""
import argparse
from collections import Counter, defaultdict
import importlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESP

from_ui = importlib.import_module('th14-ui-oracle')
Retail, ROOT, RUNTIME, digest, f = (from_ui.Retail, from_ui.ROOT, from_ui.RUNTIME,
                                    from_ui.digest, from_ui.float_word)
VM, CODE = from_ui.VM, from_ui.DATA
RATE = 0x4d8f58
ARCHIVE_SHA = '57d84a5cbe82460931cca26f090a0fde7aef180889ce5bfe8315851fc97b0233'
FIELDS = ('status instruction timer.previous timer.frame timer.fraction age.frame age.fraction '
          'flags.low flags2 rotation.x rotation.y rotation.z scale.x scale.y uv.offsetX '
          'uv.offsetY scroll.x scroll.y position.x position.y uv0.x uv0.y uv1.x uv1.y '
          'uv2.x uv2.y uv3.x uv3.y primary secondary '
          'scriptRng.seed scriptRng.calls visualRng.seed visualRng.calls').split()
OFFSETS = (0x548, 0x54c, 0x550, 0x560, 0x564, 0x18, 0x1c, 0x48, 0x4c, 0x50, 0x60,
           0x64, 0x88, 0x8c, 0x3d8, 0x3dc, 0x3c, 0x40, *range(0x3b8, 0x3d8, 4), 0x534, 0x538,
           0x4db508 - VM, 0x4db50c - VM, 0x4db510 - VM, 0x4db514 - VM)
VELOCITY = 0x800000


def instruction(op, *args, time=0, refs=0):
    return struct.pack('<hHhH', op, 8 + 4 * len(args), time, refs) + struct.pack(
        '<' + 'I' * len(args), *(arg & 0xffffffff for arg in args))


def scenarios():
    cases = {}

    def add(name, *ops, rate=1, flags=0, scale=(1, 1), size_velocity=(0, 0),
            rotation=(0, 0, 0), angle_velocity=(0, 0, 0), uv=(0, 0), scroll=(0, 0),
            slowdown=0, frames=5, interrupt=0xffffffff):
        words = [f(rate), flags, *map(f, (*scale, *size_velocity, *rotation, *angle_velocity,
                                          *uv, *scroll, slowdown)), frames, interrupt]
        cases[name] = words, b''.join((*ops, instruction(0, time=256), instruction(-1, time=32767)))

    for rate in (.6, .99, 1, 1.01, 1.25):
        add(f'clock/{rate}', rate=rate)
        add(f'clock/slow/{rate}', rate=rate, flags=0x100, slowdown=.375)
        add(f'clock/delay/{rate}', instruction(6, 2), rate=rate)
        add(f'clock/hold/{rate}', instruction(3), rate=rate)
        add(f'clock/interrupt/{rate}', instruction(4, time=1),
            instruction(5, 42, time=10), instruction(403, 127, time=10),
            instruction(7, time=10), rate=rate, interrupt=3)
    add('lifecycle/delete', instruction(1))
    add('lifecycle/stop', instruction(2))
    add('velocity/flag-clear', size_velocity=(.25, -.125), angle_velocity=(0, 0, .25),
        scroll=(.125, -.5))
    add('velocity/flag-set', flags=VELOCITY, size_velocity=(.25, -.125),
        angle_velocity=(0, 0, .25), scroll=(.125, -.5))
    add('velocity/dirty-x', instruction(416, f(.25), f(0)))
    add('velocity/dirty-angular-zero', instruction(415, f(0), f(0), f(0)))
    add('velocity/round-y', instruction(416, f(0), f(-.114719)), scale=(1, .2713), rate=.6)
    add('velocity/wrap', instruction(415, f(0), f(0), f(.7)),
        rotation=(0, 0, 3.1), rate=.6)
    add('velocity/zero-scroll', flags=VELOCITY, uv=(2, -1))
    add('velocity/vertical', instruction(426, f(-.25)), uv=(0, .1))

    for axis in (0, 1):
        for mode in range(32):
            for seed in (False, True):
                op = 427 + axis
                before = (instruction(425 + axis, f(.107)),) if seed else ()
                add(f'scroll/{op}/{mode}/' + ('with-velocity' if seed else 'alone'),
                    *before, instruction(op, 6, mode, f(.631), time=1), rate=.6, frames=13)

    for mode in range(32):
        add(f'interpolation/{mode}', instruction(407, 7, mode, f(9.3), f(28.7), f(0)),
            instruction(408, 7, mode, 21, 109, 221), instruction(409, 7, mode, 21),
            instruction(410, 7, mode, f(-.29), f(2.18), f(.17)),
            instruction(412, 7, mode, f(2.13), f(-.56)), instruction(413, 7, mode, 23, 147, 8),
            instruction(414, 7, mode, 7), rate=.75, rotation=(.25, .5, -.75), frames=12)

    for refs in (0, 1, 3, 7):
        for op, length in [(401, 3), (402, 2), (415, 3), (416, 2), (429, 2), (434, 2)]:
            if refs >= (1 << length):
                continue
            operands = [f(10011) if refs & (1 << i) else f(.25 + .25 * i) for i in range(length)]
            add(f'references/{op}/{refs}', instruction(op, *operands, refs=refs), frames=2)

    add('screen-uv/flag', flags=0x1000)
    add('screen-uv/flag-and-movement', instruction(407, 4, 0, f(64), f(32), f(0)), flags=0x1000)
    for mode in range(32):
        for start, end in ((.75, -.5), (3.05, -3.05), (-3.05, 3.05), (7.5, -12.1)):
            add(f'rotation-z/{mode}/{start}/{end}', instruction(411, 5, mode, f(end)),
                rotation=(.25, -.5, start), rate=.6, frames=10)
    add('rotation-z/overrides-vector', instruction(410, 5, 0, f(.5), f(.75), f(2)),
        instruction(411, 5, 0, f(-1)), frames=7)
    add('rotation-z/zero-duration', instruction(411, 0, 0, f(.75)))
    add('screen-uv/418-once', instruction(418), instruction(400, f(80), f(24), f(0), time=1))
    add('screen-uv/419-toggle', instruction(419, 1), instruction(400, f(80), f(24), f(0), time=1),
        instruction(419, 0, time=2), instruction(400, f(26), f(30), f(0), time=3))
    return cases


def native_rows(retail, args, code):
    rate, flags, *values, frames, interrupt = args
    retail.machine.mem_write(VM, bytes(0x5cc))
    retail.machine.mem_write(CODE, code)
    retail.put(RATE, rate)
    retail.put(VM + 0x18, 3, flags)
    retail.put(VM + 0x28, CODE)
    retail.put(VM + 0x38, CODE, f(120), f(140), 0)
    retail.put(VM + 0x48, *values[4:10])
    retail.put(VM + 0x60, *values[0:2], f(1), f(1), *values[2:4], f(1), f(1), f(16), f(24),
               *values[10:12])
    retail.put(VM + 0x3d8, *values[12:14])
    retail.put(VM + 0x4e0, f(1), f(3.141592653589793), 65536)
    retail.put(VM + 0x534, 0xffffffff, 0x88334455)
    retail.put(VM + 0x548, 0xffffffff, 0, 0, RATE, 1, 0xffffffff, 0, 0, RATE, 1)
    retail.put(VM + 0x598, values[14])
    retail.put(0x4f7a6c, f(1))
    retail.put(0x4f7a80, 320, 16, 320, 16)
    retail.put(0x4db508, 12345, 0, 54321, 0)
    result = []
    for frame in range(frames):
        if frame == interrupt:
            retail.put(VM + 0x4a0, 42)
        retail.run(0x46fe50)
        status = retail.machine.reg_read(UC_X86_REG_EAX)
        address, = retail.words(VM + 0x28, 1)
        state = [retail.words(VM + offset, 1)[0] for offset in OFFSETS]
        state[5] &= 0xf
        result.append([status, address - CODE if address else 0xffffffff, *state])
        if status:
            break
    return result


def native_atlas_names(retail, image):
    names = {}
    for mode in range(6):
        retail.machine.mem_write(0x4d9153, bytes([mode]))
        retail.run(0x40b530, stop=0x40b564)
        resource_id, address = retail.words(retail.machine.reg_read(UC_X86_REG_ESP), 2)
        if resource_id != 2:
            raise RuntimeError('Unexpected ASCII resource ID')
        names[mode] = image.cstring(address)
    return names


def observe_candidate(build, sdk, cases, archive):
    build_data = json.loads((build / 'build.json').read_bytes())
    if digest((build / 'th14-sdl.wasm').read_bytes()) != build_data['sha256']:
        raise RuntimeError('Wasm differs from build.json')
    for name, expected in build_data['sourceBindings'].items():
        if digest((ROOT / name).read_bytes()) != expected:
            raise RuntimeError(f'Build is stale for {name}')
    objects = []
    for name, expected in build_data['objects'].items():
        if name.replace('\\', '/').startswith('objects/th12_web_cpp_game_'):
            obj = build / name
            if digest(obj.read_bytes()) != expected:
                raise RuntimeError(f'Build object differs from build.json: {name}')
            objects.append(obj)
    if not objects:
        raise RuntimeError('No game objects in the GLES build')
    flags = [flag for flag in build_data['flags'] if flag not in ('--profiling-funcs', '--use-port=sdl3')]
    fixture = RUNTIME / 'th12_web/tests/cpp/th14-anm-vm-oracle.cpp'
    with tempfile.TemporaryDirectory(prefix='anm-vm-oracle-', dir=build) as temp:
        env = {**os.environ, 'EMSDK': str(sdk), 'EM_CONFIG': str(sdk / '.emscripten'),
               'TEMP': temp, 'TMP': temp, 'TMPDIR': temp}
        module = Path(temp) / 'probe.mjs'
        input_path = Path(temp) / 'cases.bin'
        with input_path.open('wb') as output:
            for args, code in cases.values():
                output.write(struct.pack('<20I', len(code), *args))
                output.write(code)
        subprocess.run([
            sys.executable, str(sdk / 'upstream/emscripten/emcc.py'), *flags,
            '-Ith12_web/cpp/game', str(fixture), *map(str, objects), '-sDEFAULT_TO_CXX=1',
            '-sENVIRONMENT=node', '-sNODERAWFS=1', '-sEXIT_RUNTIME=1',
            '-sSTACK_SIZE=1048576', '-sINITIAL_MEMORY=134217728', '-sALLOW_MEMORY_GROWTH=1', '-o', str(module),
        ], check=True, cwd=RUNTIME, env=env)
        runner = module.with_name('run.mjs')
        runner.write_text("import make from './probe.mjs';\nawait make({arguments: process.argv.slice(2)});\n",
                          encoding='utf-8')
        return subprocess.check_output(['node', str(runner), str(input_path),
                                        *([str(archive)] if archive else [])], text=True)


def audit(args):
    image, table, default = importlib.import_module('th-anm-handlers').handlers(14, args.exe)
    archive_sha = None
    if args.archive:
        with args.archive.open('rb') as stream:
            from hashlib import file_digest
            archive_sha = file_digest(stream, 'sha256').hexdigest()
        if archive_sha != ARCHIVE_SHA:
            raise ValueError('Archive must be the Japanese TH14 1.00b th14.dat')
    cases = scenarios()
    retail = Retail(image)
    native = {name: native_rows(retail, inputs, code) for name, (inputs, code) in cases.items()}
    atlas_names = native_atlas_names(retail, image)
    build = args.build.resolve()
    observed = observe_candidate(build, args.emsdk.resolve(), cases, args.archive)
    names = list(cases)
    assets, mapping, unmapped = {}, {}, {}
    frequency, modes = Counter(), defaultdict(Counter)
    locations, candidate = defaultdict(Counter), defaultdict(list)
    atlas_glyphs, atlas_by_width = defaultdict(dict), {}
    boot_atlas, zero_angular = None, []
    for line in observed.splitlines():
        kind, *words = line.split()
        if kind == 'map':
            op, value = map(int, words)
            mapping[op] = value
        elif kind == 'asset':
            asset, count = words
            assets[asset] = int(count)
        elif kind == 'boot-atlas':
            boot_atlas, = words
        elif kind == 'atlas-width':
            width, atlas = words
            atlas_by_width[int(width)] = atlas
        elif kind == 'glyph':
            atlas, slot, width, height = words
            atlas_glyphs[atlas][int(slot)] = [
                struct.unpack('<f', struct.pack('<I', int(value)))[0] for value in (width, height)
            ]
        elif kind == 'zero-angular':
            asset, slot = words
            zero_angular.append([asset, int(slot)])
        elif kind == 'op':
            asset, slot, opcode, mode = words
            op = int(opcode)
            frequency[op] += 1
            locations[op][asset] += 1
            if mode != '-1':
                modes[op][int(mode)] += 1
        elif kind == 'unmapped':
            index, op = map(int, words)
            unmapped[names[index]] = op
        elif kind == 'frame':
            index, frame, *values = map(int, words)
            rows = candidate[names[index]]
            if frame != len(rows) or len(values) != len(FIELDS):
                raise RuntimeError('Malformed candidate frame')
            rows.append(values)
        else:
            raise RuntimeError(f'Unexpected candidate output: {line}')
    if not boot_atlas or set(atlas_by_width) != {640, 960, 1280} or set(mapping) != set(range(635)) or candidate.keys() | unmapped.keys() != set(cases):
        raise RuntimeError('Incomplete candidate observations')
    missing = {op: hex(address) for op, address in table.items()
               if op >= 0 and address != default and mapping[op] == 65535}
    routed_defaults = sorted(op for op, value in mapping.items()
                             if table.get(op, default) == default and value not in (0, 65535) and op != 5)
    differences = {}
    groups = defaultdict(Counter)
    for name, reference in native.items():
        group = groups[name.split('/')[0]]
        group['cases'] += 1
        if name in unmapped:
            group['unmapped'] += 1
            differences[name] = {'unmapped': unmapped[name], 'native': reference}
            continue
        actual = candidate[name]
        result = {}
        for frame, (want, got) in enumerate(zip(reference, actual)):
            diff = {field: [a, b] for field, a, b in zip(FIELDS, want, got) if a != b}
            if diff:
                result[str(frame)] = diff
        if result or len(reference) != len(actual):
            group['different'] += 1
            differences[name] = {'frameCounts': [len(reference), len(actual)], 'frames': result}
        else:
            group['equal'] += 1

    return {
        'retailExecutableSha256': digest(image.data),
        'candidateWasmSha256': json.loads((build / 'build.json').read_bytes())['sha256'],
        'archiveSha256': archive_sha,
        'archiveAssets': assets, 'shippedOpcodes': dict(sorted(frequency.items())),
        'opcodeLocations': dict(sorted(locations.items())), 'interpolationModes': dict(sorted(modes.items())),
        'literalZeroAngularVelocity': zero_angular,
        'atlas': {'retailByMode': atlas_names, 'browserBootResource': boot_atlas, 'candidateByWidth': atlas_by_width,
                  'displayResourcesMatch': all(atlas_by_width[w] == atlas_names[mode]
                                               for mode, w in enumerate((640, 960, 1280))),
                  'retail1280ResourceMatchesBrowserBoot': atlas_names[2] == boot_atlas,
                  'decodedSpriteSizes': dict(atlas_glyphs)},
        'retailNonDefaultOpcodes': sum(op >= 0 and addr != default for op, addr in table.items()),
        'missingHandlers': missing, 'wronglyRoutedDefaults': routed_defaults,
        'groups': dict(groups), 'nativeFrames': sum(map(len, native.values())),
        'comparedFrames': sum(min(len(native[name]), len(rows)) for name, rows in candidate.items()),
        'fields': FIELDS, 'differences': differences,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', required=True, type=Path)
    parser.add_argument('--archive', type=lambda path: Path(path).resolve(), help='Japanese 1.00b th14.dat')
    parser.add_argument('--build', required=True, type=Path, help='Output of th14-build-gles.py')
    parser.add_argument('--emsdk', required=True, type=Path)
    parser.add_argument('--report', required=True, type=Path)
    parser.add_argument('--strict', action='store_true', help='Exit 1 if the oracle finds a difference')
    args = parser.parse_args()
    report = audit(args)
    args.report.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({key: report[key] for key in ('missingHandlers', 'wronglyRoutedDefaults', 'groups')}, indent=2))
    print(f"{len(report['archiveAssets'])} ANM files, {sum(report['archiveAssets'].values())} scripts, "
          f"{len(report['shippedOpcodes'])} shipped opcodes")
    if args.strict and (report['differences'] or report['missingHandlers'] or report['wronglyRoutedDefaults']
                        or not report['atlas']['displayResourcesMatch']
                        or not report['atlas']['retail1280ResourceMatchesBrowserBoot']):
        raise SystemExit(1)


if __name__ == '__main__':
    main()
