import argparse
import hashlib
import importlib.metadata
import math
import random
import struct
import sys
from fractions import Fraction
from pathlib import Path

from th128_assets.formats import require
from th128_assets.pipeline import atomic_json, identity, validate_report_destination


EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
FUNCTION_ADDRESS = 0x410C60
FUNCTION_SIZE = 0x3B


def float32(value):
    try:
        normalized = struct.unpack('<f', struct.pack('<f', value))[0]
    except (OverflowError, struct.error) as error:
        raise ValueError('value is not a finite float32') from error
    if not math.isfinite(normalized):
        raise ValueError('value is not a finite float32')
    return normalized


def emulate_pure_function(code, address, arguments):
    from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_PROT_READ, UC_PROT_EXEC
    from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP, UC_X86_REG_FPCW

    require(len(arguments) == 6 and 0 < len(code) <= 4096 - (address & 4095), 'invalid pure-function probe')
    normalized = [float32(value) for value in arguments]
    stack_base, stack_pointer, return_trap = 0x100000, 0x108000, 0x200000
    emulator = Uc(UC_ARCH_X86, UC_MODE_32)
    emulator.mem_map(address & ~4095, 4096)
    emulator.mem_write(address, code)
    emulator.mem_protect(address & ~4095, 4096, UC_PROT_READ | UC_PROT_EXEC)
    emulator.mem_map(stack_base, 0x10000)
    emulator.mem_map(return_trap, 4096, UC_PROT_READ | UC_PROT_EXEC)
    emulator.mem_write(stack_pointer, struct.pack('<I6f', return_trap, *normalized))
    emulator.reg_write(UC_X86_REG_ESP, stack_pointer)
    emulator.reg_write(UC_X86_REG_FPCW, 0x037F)
    try:
        emulator.emu_start(address, return_trap, count=256)
    except UcError as error:
        raise RuntimeError(f'pure-function sandbox stopped: {error}') from error
    require(emulator.reg_read(UC_X86_REG_EIP) == return_trap, 'function did not return within 256 instructions')
    require(emulator.reg_read(UC_X86_REG_ESP) == stack_pointer + 28, 'unexpected original calling convention')
    return emulator.reg_read(UC_X86_REG_EAX)


def expected_overlap(arguments):
    first_x, first_y, first_radius, second_x, second_y, second_radius = map(Fraction.from_float, arguments)
    distance = (first_x - second_x) ** 2 + (first_y - second_y) ** 2
    return int(distance <= (first_radius + second_radius) ** 2)


def probe(executable):
    import pefile

    before = identity(executable)
    require(before['sha256'] == EXE_SHA256, 'this address is pinned to the supplied TH128 Japanese 1.00a executable')
    image = pefile.PE(str(executable))
    code = image.get_data(FUNCTION_ADDRESS - image.OPTIONAL_HEADER.ImageBase, FUNCTION_SIZE)
    image.close()
    require(len(code) == FUNCTION_SIZE and code[-3:] == b'\xc2\x18\x00', 'original function boundary mismatch')
    samples = [
        [0, 0, 1, 2, 0, 1], [0, 0, 1, 2.000000238418579, 0, 1],
        [0, 0, 1, 1.9999998807907104, 0, 1], [0, 0, 3, 3, 4, 2],
        [0, 0, 0, 0, 0, 0], [0, 0, 0, 0.000001, 0, 0],
        [-120, 400, 2.5, -120, 400, 0.5],
    ]
    generator = random.Random(128)
    for _ in range(512):
        first_x, first_y = generator.uniform(-192, 192), generator.uniform(0, 448)
        samples.append([first_x, first_y, generator.uniform(0, 32),
                        first_x + generator.uniform(-64, 64), first_y + generator.uniform(-64, 64),
                        generator.uniform(0, 32)])
    records = []
    for sample in samples:
        arguments = [float32(value) for value in sample]
        native = emulate_pure_function(code, FUNCTION_ADDRESS, arguments)
        expected = expected_overlap(arguments)
        require(native == expected, f'candidate circle predicate mismatch: {arguments}, native={native}, expected={expected}')
        records.append({'inputs': arguments, 'originalFunctionResult': native})
    require(identity(executable) == before, 'original executable changed during probe')
    return {
        'schema': 'th128-emulated-pure-function-fixtures/1', 'executable': before,
        'address': f'0x{FUNCTION_ADDRESS:08x}', 'codeBytes': FUNCTION_SIZE,
        'codeSha256': hashlib.sha256(code).hexdigest(),
        'method': 'Read original function bytes; emulate only this function with isolated code/stack mappings and a 256-instruction cap.',
        'emulator': {'name': 'Unicorn', 'version': importlib.metadata.version('unicorn'), 'x87ControlWord': '0x037f'},
        'windowsGameProcessLaunched': False, 'windowsApisAvailable': False,
        'candidateSemantics': 'inclusive overlap of two float32-input circles for the tested nonnegative-radius domain',
        'independentCheck': 'exact rational squared-distance comparison of the float32 inputs',
        'cases': records, 'mismatches': 0,
        'limitations': ['This is instruction emulation, not a native Windows CPU trace.',
                        'Game call-site role, original FPU control state and entire floating-point domain are not proven.',
                        'This does not implement player collision, freezing, gameplay or a browser core.'],
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description='Emulate a pinned pure x86 function; no Windows game process or APIs.')
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--report', type=Path, default=Path(__file__).resolve().parents[1] / 'docs/native-circle-fixtures.json')
    arguments = parser.parse_args(argv)
    try:
        protected = [arguments.exe.resolve().parent]
        validate_report_destination(arguments.report, protected)
        report = probe(arguments.exe)
        atomic_json(validate_report_destination(arguments.report, protected), report)
        print(f'{len(report["cases"])} original-byte emulation cases match the candidate circle predicate; no game process launched.')
        print(f'Written: {arguments.report}')
    except (OSError, ValueError, RuntimeError, ImportError) as error:
        print(f'Native pure-function probe failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
