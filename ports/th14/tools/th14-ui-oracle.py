"""Compare the TH14 Web UI against Japanese 1.00b executable components.

The oracle executes the original capture rectangle, curve strip, sprite quad,
and credit initialization code in Unicorn. It reads the original menu sound
call sites and sample table. The candidate is linked from the current GLES
build objects. Render surfaces, menu state and input geometry are controlled.
"""
import argparse
import hashlib
import importlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import (
    UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_EIP,
    UC_X86_REG_ESP, UC_X86_REG_FPCW, UC_X86_REG_FPSW, UC_X86_REG_FPTAG,
    UC_X86_REG_MXCSR,
)

ROOT = Path(__file__).resolve().parents[1]
RUNTIME = ROOT / 'runtimes/th12'
BASE = 0x70000000
VM = BASE + 0x1000
DATA = BASE + 0x4000
OUTPUT = BASE + 0x8000
STACK = BASE + 0xff000
STOP = BASE + 0xfe000


def digest(data):
    return hashlib.sha256(data).hexdigest()


def float_word(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


class Retail:
    def __init__(self, image):
        self.machine = Uc(UC_ARCH_X86, UC_MODE_32)
        for _, address, size, offset, raw_size in image.sections:
            first = address & ~4095
            self.machine.mem_map(first, ((address + size + 4095) & ~4095) - first)
            self.machine.mem_write(address, image.data[offset:offset + raw_size])
        self.machine.mem_map(BASE, 0x100000)
        self.stub(0x4854a0, 0)

    def words(self, address, count):
        return struct.unpack('<' + 'I' * count, self.machine.mem_read(address, count * 4))

    def put(self, address, *values):
        self.machine.mem_write(address, struct.pack('<' + 'I' * len(values), *values))

    def stub(self, address, stack_bytes, callback=None):
        instruction = b'\xc2' + struct.pack('<H', stack_bytes) if stack_bytes else b'\xc3'
        self.machine.mem_write(address, instruction)
        if callback is not None:
            def hook(machine, _address, _size, _user):
                callback(machine.reg_read(UC_X86_REG_ESP))
            self.machine.hook_add(UC_HOOK_CODE, hook, begin=address, end=address)

    def run(self, entry, *args, ecx=VM, edx=0, stop=STOP):
        self.put(STACK, STOP, *args)
        for register, value in (
            (UC_X86_REG_ESP, STACK), (UC_X86_REG_ECX, ecx), (UC_X86_REG_EDX, edx),
            (UC_X86_REG_FPCW, 0x37f), (UC_X86_REG_FPSW, 0), (UC_X86_REG_FPTAG, 0xffff),
            (UC_X86_REG_MXCSR, 0x1f80),
        ):
            self.machine.reg_write(register, value)
        self.machine.emu_start(entry, stop, count=100000)
        if self.machine.reg_read(UC_X86_REG_EIP) != stop:
            raise RuntimeError(f'Retail component {entry:#x} did not reach {stop:#x}')


def capture_cases(retail):
    # 448bf0 builds the rectangles and calls 47e070. ANM creation/lookup
    # and the final surface copy are declared services.
    copies = []
    sprite, resource, gui, handle = (DATA + 0x1000 * i for i in range(4))
    retail.put(VM + 0x2c, sprite, resource)
    retail.put(sprite + 0x34, float_word(448), float_word(384))
    retail.put(0x4db550, gui)
    retail.put(0x4d9110, 1, 0)
    retail.stub(0x44c1a0, 8, lambda _: retail.machine.reg_write(UC_X86_REG_EAX, handle))
    retail.stub(0x47f0a0, 4, lambda _: retail.machine.reg_write(UC_X86_REG_EAX, VM))

    def observe_copy(stack):
        _, _, _, surface, _, rectangle = retail.words(stack, 6)
        copies.append([surface, *retail.words(rectangle, 4)])

    retail.stub(0x47e070, 20, observe_copy)
    result = {}
    for width in (640, 960, 1280):
        scale = width / 640
        retail.put(0x4f7a6c, float_word(scale))
        retail.put(0x4f7a80, width // 2, int(16 * scale))
        for dialogue in (0, 1):
            copies.clear()
            retail.put(gui + 0x194, dialogue)
            retail.run(0x448bf0, ecx=OUTPUT)
            if len(copies) != 1:
                raise RuntimeError('Retail pause did not deliver one surface copy')
            result[f'capture_{width}_{dialogue}'] = copies[0]
    return result


def geometry_cases(retail):
    result = {}
    # Give both engines the same authored size from a synthetic atlas at
    # each display width. Position-only modes must not scale the bitmap again.
    for width in (640, 960, 1280):
        scale = width / 640
        for label in ('queued_glyph', 'popup_glyph', 'spell_labels', 'boss_timer', 'spell_award', 'other_sprite'):
            retail.machine.mem_write(VM, bytes(0x5cc))
            retail.put(0x4f7a6c, float_word(scale))
            retail.put(VM + 0x18, 3, 0 if label == 'queued_glyph' else 3 << 19)
            retail.put(VM + 0x60, *([float_word(1)] * 4))
            factor = 1 if label == 'other_sprite' else scale
            retail.put(VM + 0x80, float_word(10 * factor), float_word(16 * factor))
            retail.run(0x476100, VM, OUTPUT, OUTPUT + 12, OUTPUT + 24, OUTPUT + 36)
            quad = struct.unpack('<12f', retail.machine.mem_read(OUTPUT, 48))
            result[f'{label}_{width}'] = [float_word(quad[3] - quad[0]), float_word(quad[7] - quad[1])]

    retail.machine.mem_write(VM, bytes(0x1600))
    retail.put(VM + 0x14ac, DATA, OUTPUT)
    retail.put(VM + 0x5d0, float_word(14))
    retail.put(VM + 0x5e0, 2)
    retail.put(VM + 0xcd0, float_word(.25))
    retail.put(VM + 0xce0, float_word(.5))
    retail.put(DATA, float_word(-50), float_word(80), 0, 0, 0, 0, 0, 0)
    retail.put(DATA + 32, float_word(50), float_word(80), 0, 0, 0, 0, 0, 0)

    for width in (640, 960, 1280):
        # Supply both engines the intermediate playfield origin. The browser
        # composites that surface into the final window after laser drawing.
        retail.put(0x4f7a88, width // 2)
        retail.put(0x4f7a74, (width * 3 // 4 - 448) // 2)
        retail.machine.mem_write(OUTPUT, bytes(112))
        retail.run(0x4418e0, stop=0x441dd3)
        result[f'curve_{width}'] = [
            word for i in range(4)
            for n, word in enumerate(retail.words(OUTPUT + i * 28, 7))
            if n in (0, 1, 5, 6)
        ]
    return result


def menu_cases(retail, image):
    call_sites = {
        'score_move': (0x44aba8,),
        'score_accept': (0x44a778,),
        'pause_save': (0x4499f1,),
        'game_over_save': (0x44aa22,),
        'practice_save': (0x44b93b,),
        'title_save': (0x461780, 0x4617f8),
    }
    voices = {}
    for n in range(76):
        voice, sample = struct.unpack('<2I', image.read(0x4d61b8 + 20 * n, 8))
        voices[voice] = image.cstring(image.u32(0x4d60b0 + 4 * sample))

    result = {}
    for label, sites in call_sites.items():
        result[label] = []
        for site in sites:
            push, voice, call, relative = struct.unpack('<BBBi', image.read(site - 2, 7))
            if push != 0x6a or call != 0xe8 or site + 5 + relative != 0x46e3d0:
                raise RuntimeError(f'Unexpected menu sound call at {site:#x}')
            result[label].append(voice)
    for difficulty in range(5):
        retail.run(0x435cef, edx=difficulty, stop=0x435cfb)
        limit, = retail.words(0x4f58c8, 1)
        for used in range(6):
            allowed = int(used < limit)
            result[f'continue_{difficulty}_{used}'] = [allowed, allowed]
    return result, {label: [voices[voice] for voice in result[label]] for label in call_sites}


def observe_candidate(build, sdk):
    build_data = json.loads((build / 'build.json').read_bytes())
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
    fixture = RUNTIME / 'th12_web/tests/cpp/th14-ui-oracle.cpp'
    with tempfile.TemporaryDirectory(prefix='ui-oracle-', dir=build) as temp:
        env = {**os.environ, 'EMSDK': str(sdk), 'EM_CONFIG': str(sdk / '.emscripten'),
               'TEMP': temp, 'TMP': temp, 'TMPDIR': temp}
        module = Path(temp) / 'ui-oracle.mjs'
        subprocess.run([
            sys.executable, str(sdk / 'upstream/emscripten/emcc.py'), *flags,
            '-Ith12_web/cpp/game', '-Ith12_web/cpp/sdl', str(fixture), *map(str, objects),
            '-sDEFAULT_TO_CXX=1', '-sENVIRONMENT=node', '-sINITIAL_MEMORY=33554432',
            '-sALLOW_MEMORY_GROWTH=1', '-o', str(module),
        ], check=True, cwd=RUNTIME, env=env)
        runner = module.with_name('run.mjs')
        runner.write_text("import make from './ui-oracle.mjs';\nawait make();\n", encoding='utf-8')
        observed = subprocess.check_output(['node', str(runner)], text=True)
    return {label: list(map(int, values)) for label, *values in map(str.split, observed.splitlines())}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', required=True, type=Path, help='Japanese TH14 v1.00b th14.exe')
    parser.add_argument('--build', required=True, type=Path, help='Output of tools/th14-build-gles.py')
    parser.add_argument('--emsdk', required=True, type=Path, help='Activated SDK used for the GLES build')
    parser.add_argument('--report', type=Path, help='Write expected and observed values as JSON')
    args = parser.parse_args()

    image = importlib.import_module('th12-pe').Image(str(args.exe))
    executable_sha = digest(image.data)
    target = json.loads((ROOT / 'runtimes/th14/target.json').read_bytes())
    if executable_sha != target['sha256']:
        parser.error('Executable must be the Japanese TH14 1.00b listed in runtimes/th14/target.json')

    retail = Retail(image)
    expected = capture_cases(retail)
    expected.update(geometry_cases(retail))
    menus, samples = menu_cases(retail, image)
    expected.update(menus)
    for mode in range(6):
        expected[f'display_{mode}'] = [1280, 960, mode]

    actual = observe_candidate(args.build.resolve(), args.emsdk.resolve())
    differences = {
        case: {'expected': expected.get(case), 'actual': actual.get(case)}
        for case in sorted(expected.keys() | actual.keys()) if expected.get(case) != actual.get(case)
    }
    if args.report:
        args.report.write_text(json.dumps({
            'nativeExecutableSha256': executable_sha, 'samples': samples,
            'expected': expected, 'actual': actual, 'differences': differences,
        }, indent=2) + '\n', encoding='utf-8')
    if differences:
        print(json.dumps(differences, indent=2))
        raise SystemExit(1)
    print(f'TH14 UI oracle passed {len(expected)} cases')


if __name__ == '__main__':
    main()
