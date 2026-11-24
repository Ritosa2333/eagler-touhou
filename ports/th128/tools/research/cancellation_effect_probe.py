import argparse
import hashlib
import json
import math
from pathlib import Path
import struct

from bullet_probe import NativeBullet, ROOT, STACK_POINTER, datum, emulate

VM, TARGET, BULLET, ZERO = 0x390000, 0x3a0000, 0x3b0000, 0x4d2e54
VELOCITIES = ((0x3f800000, 0xc0000000, 0x3e800000),
              (0x3dcccccd, 0xbe4ccccd, 0x3eaaaaab),
              (0, 0x80000000, 0x3f000000),
              (0x3f123457, 0xc1123457, 0x3a83126f))


class NativeCancellationDrift(NativeBullet):
    def sample(self, words):
        if (len(words) != 3 or any(type(word) is not int or not 0 <= word < 2**32 for word in words)
                or not all(math.isfinite(value) for value in struct.unpack('<3f', struct.pack('<3I', *words)))):
            raise ValueError('three finite explicit f32 velocity words are required')
        bullet = bytearray(0xa00)
        struct.pack_into('<3I', bullet, 0x9e4, *words)
        prefix = emulate([self.chunk(0x408af8, 0x408b4b)], 0x408af8,
                         [datum('bullet', BULLET, bytes(bullet), writable=False),
                          datum('clock', 0x4b6f58, struct.pack('<f', 1), writable=False),
                          self.constant(0x4a6738, 8)],
                         registers={'eax': VM, 'ebx': BULLET, 'ebp': STACK_POINTER + 0x20},
                         stops=[0x408b4b], budget=64)
        if prefix['stop']['reason'] != 'boundary' or prefix['stop']['address'] != '0x00408b4b':
            raise ValueError('actual scalar prefix did not reach the declared pre-call boundary')
        frame = bytes.fromhex(prefix['stackHead'])
        step = list(struct.unpack_from('<3I', frame, 8))
        target = list(struct.unpack_from('<3I', frame, 28))
        if struct.unpack_from('<2I', frame) != (30, 6):
            raise ValueError('native duration/mode stack operands do not match the bounded context')
        prefix['memory'].pop('bullet')
        initial = bytearray([0xa5] * 0x4e8)
        struct.pack_into('<I', initial, 0xe4, 0)
        initialization = emulate([self.chunk(0x40d1a0, 0x40d286)], 0x40d1a0,
                                 [datum('vm', VM, bytes(initial)),
                                  datum('zero', ZERO, bytes(12), writable=False),
                                  datum('target', TARGET, struct.pack('<3I', *target), writable=False)],
                                 registers={'eax': VM, 'edx': ZERO, 'ecx': TARGET},
                                 arguments=struct.pack('<2I', 30, 6), budget=128)
        if initialization['stop']['reason'] != 'returned':
            raise ValueError('original native interpolation initializer did not return')
        final = bytes.fromhex(initialization['memory'].pop('vm')['after'])
        initialization.update(inputVmFill=0xa5, inputVmControlE4=0,
                              fromWords=list(struct.unpack_from('<3I', final, 0xa4)),
                              toWords=list(struct.unpack_from('<3I', final, 0xb0)),
                              slopeWords=list(struct.unpack_from('<6I', final, 0xbc)),
                              timerWords=list(struct.unpack_from('<5I', final, 0xd4)),
                              duration=struct.unpack_from('<i', final, 0xe8)[0],
                              mode=struct.unpack_from('<I', final, 0xec)[0],
                              unchangedOutsideInterpolator=(final[:0xa4] == initial[:0xa4] and final[0xf0:] == initial[0xf0:]))
        return {'velocityWords': list(words), 'clockWord': 0x3f800000,
                'globalStartWords': [0, 0, 0], 'stepWords': step, 'targetWords': target,
                'prefix': prefix, 'initializer': initialization}


def probe(executable):
    native = NativeCancellationDrift(executable)
    try:
        cases = [native.sample(words) for words in VELOCITIES]
        constant = native.constants['0x004a6738']
        value = struct.unpack('<d', bytes.fromhex(constant['bytes']))[0]
        if value != 10.0:
            raise ValueError('pinned native scale is 10.0, never an assumed half-step')
        before = hashlib.sha256(native.binary).hexdigest()
        after = hashlib.sha256(native.executable.read_bytes()).hexdigest()
        if after != before:
            raise ValueError('original executable changed')
        return {'schema': 'th128-original-byte-cancellation-drift/1',
                'nativeCpuTrace': False, 'x86Emulated': True, 'windowsGameProcessLaunched': False,
                'executable': {'sha256': before, 'sha256After': after, 'unchanged': True},
                'fpuControlWord': '0x037f', 'doubleConstant': dict(constant, value=value),
                'code': native.code, 'cases': cases,
                'limitations': ['Two separate original-byte bounded executions; not a full hit/frame trace.',
                               'Prefix stops before native call0x408b4b; initializer receives declared synthetic data.',
                               'Zero global/start, clock1, fresh script-local zero; no generic drift/curve FPU equivalence.']}
    finally:
        native.image.close()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', required=True)
    arguments = parser.parse_args()
    output = Path(arguments.output).resolve()
    if (output.parent != (ROOT / 'local/research').resolve() or output.suffix != '.json'
            or not output.name.startswith('cancellation') or output.drive.lower() == 'e:'):
        raise ValueError('output must be private local/research/cancellation*.json')
    report = probe(arguments.exe)
    output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(f'Wrote {len(report["cases"])} bounded native drift prefix/initializer fixtures: {output}')


if __name__ == '__main__':
    main()
