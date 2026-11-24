import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
from bullet_probe import NativeBullet, EXE_SHA256, datum, emulate  # noqa: E402

DESTINATION = ROOT / 'local/research/enemy-stage-fields-fixtures.json'
CONTEXT, TASK, STATE, COMMAND = 0x300000, 0x340000, 0x350000, 0x360000
HANDLER_STOP = 0x41c52c


def handler(native, opcode):
    index = native.read(0x41c7c0 + opcode - 256, 1)[0]
    return index, struct.unpack('<I', native.read(0x41c554 + 4 * index, 4))[0]


def disassemble(native, start, size):
    import capstone
    code = native.read(start, size)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [['0x%08x' % item.address, item.mnemonic, item.op_str] for item in decoder.disasm(code, start)]


def read_text(native):
    for section in native.image.sections:
        if section.Name.rstrip(b'\0') == b'.text':
            start = 0x400000 + section.VirtualAddress
            size = min(section.Misc_VirtualSize, section.SizeOfRawData)
            data = bytearray()
            while size > 0:
                step = min(65536, size)
                data.extend(native.read(start, step))
                start += step
                size -= step
            return 0x400000 + section.VirtualAddress, bytes(data)
    raise ValueError('missing .text')


def field_scan(native):
    import capstone
    start, data = read_text(native)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    result = {}
    for field in (0x238, 0x3fa0, 0x3fa4, 0x3e9c):
        needle = struct.pack('<I', field)
        hits, offset = [], 0
        while True:
            found = data.find(needle, offset)
            if found < 0:
                break
            offset = found + 1
            address = start + found
            begin = max(start, address - 20)
            window = data[begin - start:begin - start + 44]
            for item in decoder.disasm(window, begin):
                if item.address <= address < item.address + item.size and '[' in item.op_str:
                    hits.append(['0x%08x' % item.address, item.mnemonic, item.op_str])
                    break
        result['0x%03x' % field] = sorted({tuple(hit) for hit in hits})
        result['0x%03x' % field] = [list(hit) for hit in sorted({tuple(hit) for hit in hits})]
    return result


def store_case(native, opcode, argument, entry):
    size = 0x14 if opcode == 452 else 0x25
    chunks = [native.chunk(entry, entry + size), native.chunk(0x41e0c0, 0x41e0d0),
              native.chunk(0x46fed0, 0x46ff66), native.chunk(HANDLER_STOP, HANDLER_STOP + 2)]
    context = bytearray([0xa5]) * 0x4000
    struct.pack_into('<I', context, 0x3f7c, TASK)
    state = bytearray(0x1020)
    struct.pack_into('<I', state, 4, COMMAND)
    enemy = bytearray([0xa5]) * 0x5178
    struct.pack_into('<I', enemy, 4, STATE)
    command = struct.pack('<iHHHBBIi', 0, opcode, 20, 0, 255, 1, 0, argument)
    result = emulate(chunks, entry, [
        datum('context', CONTEXT, bytes(context)), datum('state', STATE, bytes(state)),
        datum('enemy', TASK, bytes(enemy)), datum('command', COMMAND, command, writable=False),
        datum('table', 0x4b1e54, native.read(0x4b1e54, 0x500), writable=False)],
        registers={'ebx': CONTEXT}, stack={0x18: struct.pack('<I', COMMAND)}, stops=(HANDLER_STOP,), budget=128)
    if result['stop'] != {'reason': 'boundary', 'address': '0x0041c52c'}:
        raise ValueError(result['stop'])
    after = bytes.fromhex(result['memory']['context']['after'])
    return {'opcode': opcode, 'argument': argument, 'stop': result['stop'],
            'field238': struct.unpack_from('<i', after, 0x238)[0],
            'field3fa0': '0x%08x' % struct.unpack_from('<I', after, 0x3fa0)[0],
            'field3fa4': struct.unpack_from('<i', after, 0x3fa4)[0]}


def probe(executable):
    native = NativeBullet(executable)
    try:
        if hashlib.sha256(native.executable.read_bytes()).hexdigest() != EXE_SHA256:
            raise ValueError('probe requires the pinned Japanese executable')
        dispatch = {}
        for opcode in (259, 262, 269, 452, 529):
            index, entry = handler(native, opcode)
            dispatch[str(opcode)] = {'index': index, 'handler': '0x%06x' % entry}
        expected = {'259': 0x418997, '262': 0x418da3, '269': 0x418ecb, '452': 0x41894e, '529': 0x41a286}
        if any(int(dispatch[str(opcode)]['handler'], 16) != expected[str(opcode)] for opcode in expected):
            raise ValueError('unexpected handler dispatch')
        table_raw = native.read(0x4b1e54, 32 * 4)
        table = [struct.unpack_from('<I', table_raw, index * 4)[0] for index in range(32)]
        radius = struct.unpack('<f', native.read(0x4a6820, 4))[0]
        cases = [store_case(native, 452, 2, 0x41894e),
                 store_case(native, 452, -3, 0x41894e),
                 store_case(native, 529, 0, 0x41a286),
                 store_case(native, 529, 2, 0x41a286),
                 store_case(native, 529, 3, 0x41a286),
                 store_case(native, 529, 40, 0x41a286),
                 store_case(native, 529, 100, 0x41a286)]
        return {
            'schema': 'th128-enemy-stage-fields/1',
            'executable': {'sha256': EXE_SHA256,
                           'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'dispatch': dispatch,
            'handlers': {'452': disassemble(native, 0x41894e, 0x14),
                         '529': disassemble(native, 0x41a286, 0x25),
                         '259': disassemble(native, 0x418997, 0x75),
                         '262': disassemble(native, 0x418da3, 0x55),
                         '269': disassemble(native, 0x418ecb, 0x4a),
                         'enemyUpdateCallbackUse': disassemble(native, 0x4164e0, 0x60)},
            'table4b1e54': table,
            'callbackEntries': {'2': disassemble(native, 0x41e860, 0x60),
                                '3': disassemble(native, 0x41e9b0, 0x60)},
            'collisionRadius4a6820': radius,
            'fieldScan': field_scan(native),
            'cases': cases,
            'conclusions': {
                '452': 'handler reads one integer and stores it at +0x238; 259/262 add 7 and pass the result as the ANM birth layer; 269 uses the fixed layer 8; no other 452 logic exists',
                '529': 'handler selects table4b1e54[argument] into +0x3fa0 and clears +0x3fa4; the enemy update calls a nonzero +0x3fa0 as a native function pointer each tick; real arguments 2/3 select native player-collision callbacks, 0 clears; no bounds check in the original',
                'tableIndex0': table[0] == 0,
                'selectedCallbackPointers': {'2': '0x%06x' % table[2], '3': '0x%06x' % table[3]}},
            'apiReturnsAssigned': False, 'windowsGameProcessLaunched': False,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'scope': '452 integer store and 259/262/269 layer consumers; 529 table selection and native callback consumer; bounded static slices and handler stores only'
        }
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    report = probe(arguments.exe)
    target = DESTINATION.resolve()
    if target != (ROOT / 'local/research/enemy-stage-fields-fixtures.json').resolve():
        raise ValueError('report must be local/research/enemy-stage-fields-fixtures.json')
    target.write_text(json.dumps(report, indent=1) + '\n', encoding='utf-8', newline='\n')
    print('recorded 452 layer consumer, 529 callback selection and %d original store cases' % len(report['cases']))
