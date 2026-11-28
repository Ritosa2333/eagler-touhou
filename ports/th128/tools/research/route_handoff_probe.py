import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
OUTPUT = ROOT / 'local/research/route-handoff-fixtures.json'
FUNCTIONS = {'routeHandoff': 0x4273a0, 'timeScale': 0x434a20}
ANCHORS = {
    0x4273a3: 'mov eax, dword ptr [0x4b8954]',
    0x4273ac: 'mov eax, dword ptr [0x4b4d14]',
    0x4273c2: 'mov ecx, dword ptr [0x4b4d44]',
    0x4273f4: 'inc dword ptr [0x4b4d44]',
    0x4273da: 'add eax, 3',
    0x4273ff: 'inc eax',
    0x427403: 'mov dword ptr [0x4b4d14], eax',
    0x42740f: 'mov dword ptr [0x4b8a98], eax',
    0x434a23: 'fld dword ptr [0x4b6f58]',
    0x434a48: 'fdivr dword ptr [ebp + 8]',
    0x434a52: 'call 0x48fac0',
    0x434a64: 'call 0x458ff0',
}


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
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

    def read(address, size):
        return bytes(pe.get_data(address - base, size))

    functions = {}
    for name, start in FUNCTIONS.items():
        code = read(start, 0x100)
        instructions, end = [], None
        for item in decoder.disasm(code, start):
            instructions.append({'address': f'{item.address:#010x}', 'bytes': item.bytes.hex(),
                                 'text': f'{item.mnemonic} {item.op_str}'.strip()})
            if item.mnemonic.startswith('ret'):
                end = item.address + item.size
                break
        if end is None:
            raise ValueError(f'{name} has no ret in bounded slice')
        payload = read(start, end - start)
        if payload[-3:-1] != b'\xc2\x04':
            raise ValueError(f'{name} does not end in ret 4')
        functions[name] = {
            'start': f'{start:#010x}', 'endExclusive': f'{end:#010x}',
            'bytes': end - start, 'sha256': digest(payload),
            'instructions': instructions,
        }
    by_text = {item['address']: item['text'] for key in functions
               for item in functions[key]['instructions']}
    for address, text in ANCHORS.items():
        if by_text.get(f'{address:#010x}') != text:
            raise ValueError(f'anchor {address:#x} changed: {by_text.get(f"{address:#010x}")!r}')
    calls = {}
    for name, start in FUNCTIONS.items():
        calls[name] = [item['text'] for item in functions[name]['instructions']
                       if item['text'].startswith('call')]
    if calls['timeScale'] != ['call 0x48fac0', 'call 0x458ff0']:
        raise ValueError('time helper call order changed')
    constants = {'clock': 0x4b6f58, 'msg22_fast': 0x4a674c, 'msg22_slow': 0x4a682c}
    values = {name: {'address': f'{address:#010x}', 'bytes': read(address, 4).hex(),
                     'float': struct.unpack('<f', read(address, 4))[0]}
              for name, address in constants.items()}
    if values['clock']['float'] != 1.0 or values['msg22_fast']['float'] != 8.0 or values['msg22_slow']['float'] != 2.0:
        raise ValueError('scalar constants changed')
    cases = []
    for gate in (False, True):
        for progress in (0, 1, 2):
            for choice in (0, 1):
                next_id = 2
                advanced = False
                if gate:
                    next_id = 4 if progress % 2 == 1 else 2
                elif choice:
                    next_id, advanced = 4, True
                cases.append({'gate74': gate, 'progress': progress, 'message78': choice,
                              'nextId': next_id, 'progressIncremented': advanced,
                              'record': f'{0x4b1a30 + next_id * 40:#010x}'})
    report = {
        'schema': 'th128-route-handoff-probe/1',
        'sourceExe': str(path), 'sourceExeSha256': EXE_SHA256,
        'generatorSha256': digest(Path(__file__).read_bytes()),
        'functions': functions, 'calls': calls, 'constants': values,
        'firstStageCases': cases,
        'firstStageFacts': {
            'input': ['0x4b8954 pointer', '[0x4b8954]+0x74 gate', '[0x4b4d14] current id',
                      '[0x4b4d44] progress', '[ebp+8] message+78'],
            'output': ['[0x4b4d44] may increment by 1', '[0x4b4d14] next id',
                       '[0x4b8a98] = 0x4b1a30 + nextId*40'],
            'currentId1NextId': [2, 4],
            'timeScaleFormula': {
                'clockEqualZeroOrAtLeastOne': 'arg',
                'clockBetweenZeroAndOne': 'arg / clock',
                'then': 'truncate to int; call 0x458ff0(0x4d5a68, 0x4a28bd, 5, int); eax = 0',
                'derivedFromStaticBranchDecode': True,
            },
            'dynamicExecution': False,
            'unicornAvailable': unicorn_available,
            'calleesNotExecuted': ['0x48fac0', '0x458ff0'],
            'unknowns': ['numeric meaning of ids 0..23', 'actual 0x4b8954+0x74 and 0x4b4d44 runtime values',
                         'whether [0x4b8a98] consumers treat the record as stage resources'],
        },
    }
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print(f'route 0x4273a0..{functions["routeHandoff"]["endExclusive"]} '
          f'time 0x434a20..{functions["timeScale"]["endExclusive"]}')
    print(f'wrote {OUTPUT.relative_to(ROOT)} static-only fixture')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    main(parser.parse_args().exe)
