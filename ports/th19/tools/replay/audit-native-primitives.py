"""Emulate two isolated instruction ranges; never execute or launch th19.exe.

Requires pefile and unicorn==2.1.4. The input EXE is opened read-only.
The RNG mutex calls are replaced with successful no-ops; no game imports,
entry point, files, graphics, network or other game routines are executed.
"""
import argparse
import hashlib
import json
import random
import struct
from pathlib import Path
import pefile
import unicorn
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_XMM0, UC_X86_REG_XMM1

EXPECTED = '8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9'
OBJECT, STACK, STOP = 0x100000, 0x200800, 0x300000


def emulator(pe, start, end):
    uc = unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32)
    for address, size in [(OBJECT, 0x1000), (0x200000, 0x1000), (STOP, 0x1000),
                          (0x402000, 0x1000), (0x404000, 0x1000), (0x462000, 0x1000),
                          (0x596000, 0x1000), (0x5aa000, 0x1000),
                          (0x5c6000, 0x1000), (0x62a000, 0x1000)]:
        uc.mem_map(address, size)
    uc.mem_write(start, pe.get_data(start - 0x400000, end - start))
    uc.mem_write(STACK, struct.pack('<II', STOP, 0))
    uc.reg_write(UC_X86_REG_ESP, STACK)
    uc.reg_write(UC_X86_REG_ECX, OBJECT)
    return uc


def timer(pe, state, speed):
    uc = emulator(pe, 0x4026c0, 0x402752)
    uc.mem_write(OBJECT, struct.pack('<IIfI', state['previous'], state['frame'], state['fraction'], state['selector']))
    uc.mem_write(0x596da0, struct.pack('<I', 0 if speed is None else 0x5c6418))
    uc.mem_write(0x5aa22c, pe.get_data(0x1aa22c, 12))
    if speed is not None:
        uc.mem_write(0x5c6418, struct.pack('<f', speed))
    uc.emu_start(0x4026c0, STOP, count=200)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP, 'Timer did not return'
    previous, frame, fraction, selector = struct.unpack('<IIfI', uc.mem_read(OBJECT, 16))
    return dict(previous=previous, frame=frame, fraction=fraction, selector=selector)


def rng(pe, state, span):
    uc = emulator(pe, 0x402540, 0x402617)
    for address in [0x46230c, 0x46231d]:
        uc.mem_write(address, bytes.fromhex('31c0c3'))  # xor eax,eax; ret
    uc.mem_write(OBJECT, struct.pack('<6I', 0, state, 0, span, span, 0))
    uc.emu_start(0x402540, STOP, count=300)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP, 'RNG did not return'
    words = struct.unpack('<6I', uc.mem_read(OBJECT, 24))
    return dict(state=words[1], shadow=words[5], value=uc.reg_read(UC_X86_REG_EAX))


def random_float(pe, state, scale, signed):
    start, end = (0x404bd0, 0x404c3d) if signed else (0x404c90, 0x404cea)
    uc = emulator(pe, start, end)
    uc.mem_write(0x402540, pe.get_data(0x2540, 0xd7))
    uc.mem_write(0x5aa000, pe.get_data(0x1aa000, 0x1000))
    for address in [0x46230c, 0x46231d]:
        uc.mem_write(address, bytes.fromhex('31c0c3'))
    uc.mem_write(OBJECT, struct.pack('<6I', 0, state, 0, 0xffff00, 0xffff00, 0))
    uc.reg_write(UC_X86_REG_XMM1, struct.unpack('<I', struct.pack('<f', scale))[0])
    uc.emu_start(start, STOP, count=400)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP, 'Float RNG did not return'
    bits = uc.reg_read(UC_X86_REG_XMM0) & 0xffffffff
    return dict(bits=bits, state=struct.unpack('<I', uc.mem_read(OBJECT+4,4))[0])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    data = args.exe.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if digest != EXPECTED:
        raise SystemExit('Unsupported EXE SHA256; instruction offsets not applied')
    pe = pefile.PE(data=data)
    f32 = lambda value: struct.unpack('<f', struct.pack('<f', value))[0]
    clocks = []
    for frame, fraction in [(0, 0), (30, 30), (1, 1.75), (0xffffffff, 3),
                            (0x7fffffff, 2147483520), (20, -1.5)]:
        for speed in [None, 0, 0.5, f32(0.99), 0.995, 1, 1.005, f32(1.01), 1.5, 2]:
            for selector in [0, 7]:
                state = dict(previous=0xffffffff, frame=frame, fraction=fraction, selector=selector)
                clocks.append(dict(input=state, speed=speed, output=timer(pe, state, speed)))
    samples = random.Random(190110)
    seeds = [0, 1, 48271, 2147483646, 2147483647, 0x80000000, 0xffffffff]
    seeds += [samples.randrange(0x100000000) for _ in range(32)]
    randoms = [dict(state=seed, range=span, output=rng(pe, seed, span))
               for seed in seeds for span in [1, 256, 0xffff00, 0xffffffff]]
    floats = [dict(state=seed, scale=scale, signed=signed, output=random_float(pe, seed, scale, signed))
              for seed in seeds for scale in [1, f32(3.1415927410125732), 4.25] for signed in [False, True]]
    report = dict(exeSha256=digest, unicornVersion=unicorn.__version__,
                  gameLaunched=False, mutexCallsStubbed=True, timer=clocks, rng=randoms, randomFloats=floats)
    args.output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(f'Isolated instructions: {len(clocks)} timer vectors, {len(randoms)} RNG vectors, {len(floats)} float RNG vectors')


if __name__ == '__main__':
    main()
