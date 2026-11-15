import argparse
import functools
import hashlib
import importlib.metadata
import struct
import sys
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]))

from th128_assets.formats import require
from th128_assets.pipeline import atomic_json, validate_report_destination


EXPECTED_EXE = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
ENTRY16, ENTRY32 = 0x46A3F0, 0x46A460


def validate_state(seed, calls):
    require(type(seed) is int and 0 <= seed <= 65535 and type(calls) is int and 0 <= calls <= 0xFFFFFFFF,
            'invalid original RNG state')


def rotate(value):
    return ((value << 2) | (value >> 14)) & 65535


def step16(seed, calls):
    validate_state(seed, calls)
    transformed = ((seed ^ 0x9630) - 0x6553) & 65535
    final_seed = rotate(transformed)
    return final_seed, final_seed, (calls + 1) & 0xFFFFFFFF


def step32(seed, calls):
    validate_state(seed, calls)
    first = ((seed ^ 0x9630) - 0x6553) & 65535
    second = ((rotate(first) ^ 0x9630) - 0x6553) & 65535
    return (first << 16) | second, rotate(second), (calls + 2) & 0xFFFFFFFF


@functools.lru_cache(maxsize=2)
def original_code(filename, modified, size):
    import pefile

    require(0 < size <= 16 * 1024 * 1024, 'unexpected original executable size')
    with Path(filename).open('rb') as source:
        binary = source.read(16 * 1024 * 1024 + 1)
    require(hashlib.sha256(binary).hexdigest() == EXPECTED_EXE, 'RNG addresses require pinned original executable')
    image = pefile.PE(data=binary)
    code = image.get_data(ENTRY16 - image.OPTIONAL_HEADER.ImageBase, 0x105)
    image.close()
    require(len(code) == 0x105 and code[-1] == 0xC3, 'unexpected original RNG function boundaries')
    return code


def sample_original(executable, bits, seed, calls):
    from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_PROT_READ, UC_PROT_EXEC
    from unicorn.x86_const import UC_X86_REG_ESI, UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_EAX

    validate_state(seed, calls)
    require(bits in (16,32), 'unsupported RNG width')
    executable = executable.resolve()
    information = executable.stat()
    code = original_code(str(executable), information.st_mtime_ns, information.st_size)
    machine = Uc(UC_ARCH_X86, UC_MODE_32)
    for address, size in ((0x46A000,4096),(0x4D3000,4096),(0x300000,4096),(0x100000,65536),(0x200000,4096)):
        machine.mem_map(address,size)
    machine.mem_write(ENTRY16,code)
    machine.mem_protect(0x46A000,4096,UC_PROT_READ|UC_PROT_EXEC)
    machine.mem_write(0x300000,struct.pack('<HHI',seed,0,calls))
    machine.mem_write(0x108000,struct.pack('<I',0x200000))
    machine.reg_write(UC_X86_REG_ESI,0x300000)
    machine.reg_write(UC_X86_REG_ESP,0x108000)
    try:
        machine.emu_start(ENTRY16 if bits==16 else ENTRY32,0x200000,count=200)
    except UcError as error:
        raise RuntimeError(f'original RNG sandbox failed: {error}') from error
    require(machine.reg_read(UC_X86_REG_EIP)==0x200000 and machine.reg_read(UC_X86_REG_ESP)==0x108004,
            'original RNG did not return within probe budget')
    result = machine.reg_read(UC_X86_REG_EAX)
    final_seed, padding, final_calls = struct.unpack('<HHI',machine.mem_read(0x300000,8))
    require(padding==0, 'RNG modified unexpected state bytes')
    return result & (65535 if bits==16 else 0xFFFFFFFF), final_seed, final_calls


def main(argv=None):
    parser = argparse.ArgumentParser(description='Sample actual TH128 RNG helpers in bounded instruction emulation.')
    parser.add_argument('--exe',type=Path,required=True)
    parser.add_argument('--report',type=Path,default=Path(__file__).resolve().parents[2]/'docs/native-rng-fixtures.json')
    arguments = parser.parse_args(argv)
    try:
        protected = [arguments.exe.resolve().parent]
        validate_report_destination(arguments.report,protected)
        samples = []
        for seed in [*range(0,65536,257),0x1234]:
            for bits, model in ((16,step16),(32,step32)):
                calls = 0xFFFFFFFE if seed==0xFFFF else seed*3
                observed = sample_original(arguments.exe,bits,seed,calls)
                require(observed==model(seed,calls), f'RNG mismatch for seed {seed}, width {bits}')
                samples.append({'seed':seed,'calls':calls,'bits':bits,'result':observed[0],
                                'finalSeed':observed[1],'finalCalls':observed[2]})
        information = arguments.exe.stat()
        code = original_code(str(arguments.exe.resolve()),information.st_mtime_ns,information.st_size)
        report = {'schema':'th128-rng-original-byte-fixtures/1','exeSha256':EXPECTED_EXE,
                  'entry16':hex(ENTRY16),'entry32':hex(ENTRY32),'codeSha256':hashlib.sha256(code).hexdigest(),
                  'emulator':importlib.metadata.version('unicorn'),'windowsGameProcessLaunched':False,
                  'threadLockFlag':0,'cases':samples,'mismatches':0,
                  'limitations':['Original-byte emulation, not native CPU execution.','Thread lock flag is disabled; no Windows lock calls are provided.']}
        atomic_json(validate_report_destination(arguments.report,protected),report)
        print(f'{len(samples)} native-byte RNG transitions match result, seed and counter; report: {arguments.report}')
    except (ValueError,OSError,RuntimeError,ImportError) as error:
        print(f'RNG probe failed: {error}',file=sys.stderr)
        return 1
    return 0


if __name__=='__main__':
    raise SystemExit(main())
