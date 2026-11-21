import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
DISPATCH = (0x4232a3, 0x4232bd)
ADVANCE = (0x4248be, 0x4248de)
TABLE = 0x42497c
DEFAULT = 0x4248be
HANDLERS = {21: 0x42454e, 22: 0x4247f6, 30: 0x42486a}
REFERENCE = {10: 0x4241e0, 12: 0x42445e, 28: 0x423769, 29: 0x424852}
CONSTANTS = {'msg22_fast': 0x4a674c, 'msg22_slow': 0x4a682c}
OUTPUT = ROOT / 'local/research/postboss-transition-fixtures.json'


def digest(payload):
    return hashlib.sha256(payload).hexdigest()


def main(executable):
    try:
        import unicorn  # noqa: F401
        unicorn_available = True
    except ImportError:
        unicorn_available = False
    path = Path(executable).resolve()
    binary = path.read_bytes()
    if hashlib.sha256(binary).hexdigest() != EXE_SHA256:
        raise ValueError('pinned TH128 executable hash mismatch')
    pe = pefile.PE(data=binary, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    if base != 0x400000:
        raise ValueError('unexpected image base')

    def read(address, size):
        payload = bytes(pe.get_data(address - base, size))
        if len(payload) != size:
            raise ValueError(f'truncated original slice at {address:#x}')
        return payload

    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

    def disassemble(start, end):
        code = read(start, end - start)
        return code, [{'address': f'{item.address:#010x}', 'bytes': item.bytes.hex(),
                       'text': f'{item.mnemonic} {item.op_str}'.strip()}
                      for item in decoder.disasm(code, start)]

    dispatch_code, dispatch = disassemble(*DISPATCH)
    advance_code, advance = disassemble(*ADVANCE)
    dispatch_text = [item['text'] for item in dispatch]
    expected = ['movzx ecx, byte ptr [eax + 2]', 'cmp ecx, 0x1e',
                'ja 0x4248be', 'jmp dword ptr [ecx*4 + 0x42497c]']
    for text in expected:
        if text not in dispatch_text:
            raise ValueError(f'dispatch decode changed: missing {text!r}')
    table_bytes = read(TABLE, 31 * 4)
    table = list(struct.unpack('<31I', table_bytes))
    for opcode, target in {**HANDLERS, **REFERENCE}.items():
        if table[opcode] != target:
            raise ValueError(f'opcode {opcode} target changed: {table[opcode]:#x}')
    if table[21] >= table[22] or table[22] >= table[27] or table[30] != 0x42486a:
        raise ValueError('unexpected handler ordering')

    targets = sorted(set(table) | {DEFAULT})
    handlers = {}
    for opcode, start in HANDLERS.items():
        end = min(target for target in targets if target > start)
        code, instructions = disassemble(start, end)
        calls = sorted({int(match, 16) for text in (item['text'] for item in instructions)
                        for match in re.findall(r'call 0x([0-9a-f]+)', text)})
        globals_ = sorted({int(match, 16) for text in (item['text'] for item in instructions)
                           for match in re.findall(r'\[0x(4b[0-9a-f]+)', text)})
        fields = sorted({int(match, 16) for text in (item['text'] for item in instructions)
                         for match in re.findall(r'\[esi \+ 0x([0-9a-f]+)\]', text)})
        compared = sorted({int(match, 0) for text in (item['text'] for item in instructions)
                           for match in re.findall(r'cmp eax, (0x[0-9a-f]+|\d+)$', text)})
        handlers[opcode] = {
            'start': f'{start:#010x}', 'endExclusive': f'{end:#010x}',
            'sha256': digest(code), 'instructionCount': len(instructions),
            'calls': [f'{address:#010x}' for address in calls],
            'globalAddresses': [f'{address:#010x}' for address in globals_],
            'hostFieldOffsets': [f'{offset:#x}' for offset in fields],
            'stageIdComparisons': [f'{value:#x}' for value in compared],
            'instructions': instructions,
        }
    constants = {}
    for name, address in CONSTANTS.items():
        payload = read(address, 4)
        value = struct.unpack('<f', payload)[0]
        constants[name] = {'address': f'{address:#010x}', 'bytes': payload.hex(),
                           'sha256': digest(payload), 'float': value}
    if constants['msg22_fast']['float'] != 8.0 or constants['msg22_slow']['float'] != 2.0:
        raise ValueError('MSG22 constants changed')

    checks = {
        'exeSha256': EXE_SHA256,
        'dispatch': expected,
        'dispatchBytesSha256': digest(dispatch_code),
        'commandAdvance': [item['text'] for item in advance],
        'commandAdvanceBytesSha256': digest(advance_code),
        'jumpTable31Sha256': digest(table_bytes),
        'handlers': {str(opcode): f'{target:#010x}' for opcode, target in HANDLERS.items()},
        'referenceHandlers': {str(opcode): f'{target:#010x}' for opcode, target in REFERENCE.items()},
        'maxOpcode': 0x1e,
        'staticOnly': True,
        'unicornAvailable': unicorn_available,
        'unicornImportedButNotExecuted': unicorn_available,
        'callsEntered': False,
        'apiReturnsAssigned': False,
    }
    report = {
        'schema': 'th128-postboss-transition-probe/1',
        'sourceExe': str(path), 'sourceExeSha256': EXE_SHA256,
        'generatorSha256': digest(Path(__file__).read_bytes()),
        'checks': checks,
        'constants': constants,
        'handlers': handlers,
        'knownGaps': [
            'raw st_a1.msg entry 1 command sequence not readable from the current generation ACL',
            'callee bodies, return values and side effects not executed',
            'MSG21/22/30 host field names remain inferred from offsets only',
        ],
    }
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print('verified dispatch 0x4232a3->0x42497c; MSG21/22/30 at '
          + ', '.join(f'{opcode}:{target:#x}' for opcode, target in HANDLERS.items()))
    print(f'wrote {OUTPUT.relative_to(ROOT)} static-only fixture')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    main(parser.parse_args().exe)
