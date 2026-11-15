import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, EXE_SHA256, datum
from enemy_probe import emulate
from freeze_probe import STACK_POINTER

MANAGER, FRAME, COMMAND = 0x380000, 0x390000, 0x3a0000


def probe(executable):
    native = NativeBullet(executable)
    try:
        index = native.read(0x41c7c0 + 420 - 256, 1)[0]
        if struct.unpack('<I', native.read(0x41c554 + index * 4, 4))[0] != 0x41b3dd:
            raise ValueError('unexpected original420 handler dispatch')
        handlers, cores = [], []
        chunks = [native.chunk(0x41b3dd, 0x41b400), native.chunk(0x41c52c, 0x41c535)]
        vectors = [tuple([0] * 8)]
        vectors.extend(tuple(0x12345678 if slot == selected else 0 for slot in range(8)) for selected in range(8))
        vectors.append((1, 2, 3, 4, 5, 6, 7, 8))
        for owners in vectors:
            manager = bytearray(0x80)
            struct.pack_into('<8I', manager, 0x5c, *owners)
            result = emulate(chunks, 0x41b3dd,
                [datum('manager', MANAGER, bytes(manager), writable=False),
                 datum('managerGlobal', 0x4b8948, struct.pack('<I', MANAGER), writable=False)],
                stops=(0x41c52e,), budget=32)
            if result['stop'] != {'reason': 'boundary', 'address': '0x0041c52e'}:
                raise ValueError(result['stop'])
            result.update(owners=list(owners), result=struct.unpack('<i', struct.pack('<I', result['registers']['eax']))[0])
            handlers.append(result)
        core_chunks = [native.chunk(0x46fbe0, 0x46fc12)]
        for host_result in (-1, 0):
            for clock in (0.0, 100.0, 23.5):
                frame = struct.pack('<fI', clock, COMMAND)
                commands = struct.pack('<iHHHBBI', 0, 420, 16, 0, 255, 0, 0)
                commands += struct.pack('<iHHHBBI', 1000, 0, 16, 0, 255, 0, 0)
                result = emulate(core_chunks, 0x46fbe0,
                    [datum('frame', FRAME, frame), datum('commands', COMMAND, commands, writable=False)],
                    registers={'ebx': FRAME, 'eax': host_result & 0xffffffff, 'ebp': STACK_POINTER + 0x100},
                    stack={0x108: struct.pack('<f', 1)}, stops=(0x46fc0c,), budget=32)
                if result['stop'] != {'reason': 'boundary', 'address': '0x0046fc0c'}:
                    raise ValueError(result['stop'])
                after = bytes.fromhex(result['memory']['frame']['after'])
                result.update(result=host_result, before={'time': clock, 'pc': COMMAND},
                    after={'time': struct.unpack_from('<f', after)[0], 'pc': struct.unpack_from('<I', after, 4)[0]},
                    inputScope='declared -1/0 scalar input matching measured handler prefixes; host call not executed or replaced')
                cores.append(result)
        return {'schema': 'th128-original-boss-wait420/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'handlerCases': handlers, 'coreCases': cores, 'code': native.code,
            'apiReturnsAssigned': False, 'hostCallExecutedInCoreTail': False, 'windowsGameProcessLaunched': False,
            'nativeCpuTrace': False, 'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'scope': 'original420 reads/branches through common return-value epilogue boundary; independent original core tails with declared scalar results prove PC/clock handling, not a full indirect host call or whole manager trace'}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    report = probe(arguments.exe)
    (ROOT / 'local/research/boss-wait-fixtures.json').write_text(
        json.dumps(report, indent=2) + '\n', encoding='utf-8', newline='\n')
    print('ten original420 boss-owner prefixes and six independent original core held/normal tails')
