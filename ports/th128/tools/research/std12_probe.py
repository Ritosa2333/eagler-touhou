import hashlib
import json
import math
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE = Path(r'E:\gee\妖精大战争\th128.exe')
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
STAGE_SHA256 = 'fa2bc2844f1af0d8973ede623b1be78f4bc536465f671652c1051f2643d2c323'
OUTPUT = ROOT / 'local/research/std12-fixtures.json'


def digest(payload):
    return hashlib.sha256(payload).hexdigest()


def f32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]


def disassemble(pe, start, end):
    image = pe.get_data(start - pe.OPTIONAL_HEADER.ImageBase, end - start)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [
        {'address': f'{item.address:#010x}', 'bytes': item.bytes.hex(),
         'text': f'{item.mnemonic} {item.op_str}'.strip()}
        for item in decoder.disasm(image, start)
    ]


def read_stage():
    catalog = json.loads((ROOT / 'local/catalog.json').read_text(encoding='utf8'))
    manifest = ROOT / 'local' / catalog['variants']['ja']['manifest']
    path = manifest.parent / 'raw/stage_b2.std'
    payload = path.read_bytes()
    if digest(payload) != STAGE_SHA256:
        raise ValueError('stage_b2.std hash mismatch')
    count = struct.unpack_from('<H', payload, 0)[0]
    face = struct.unpack_from('<I', payload, 4)[0]
    script = struct.unpack_from('<I', payload, 8)[0]
    commands = []
    cursor = script
    while cursor + 8 <= len(payload):
        size = struct.unpack_from('<H', payload, cursor + 6)[0]
        if size == 65535:
            break
        opcode = struct.unpack_from('<H', payload, cursor + 4)[0]
        time = struct.unpack_from('<i', payload, cursor)[0]
        values = list(struct.unpack_from('<' + 'i' * ((size - 8) // 4), payload, cursor + 8))
        commands.append({'offset': cursor - script, 'time': time, 'size': size,
                         'opcode': opcode, 'integers': values})
        cursor += size
    opcode12 = commands[[command['opcode'] for command in commands].index(12)]
    return {'path': str(path.relative_to(ROOT)), 'sha256': digest(payload), 'count': count,
            'faceOffset': face, 'scriptOffset': script, 'commands': len(commands),
            'opcode12': opcode12}


def main():
    binary = EXE.read_bytes()
    if hashlib.sha256(binary).hexdigest() != EXE_SHA256:
        raise ValueError('pinned TH128 executable hash mismatch')
    pe = pefile.PE(data=binary, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase

    def read(address, size):
        payload = bytes(pe.get_data(address - base, size))
        if len(payload) != size:
            raise ValueError('truncated original slice')
        return payload

    dispatch = disassemble(pe, 0x405633, 0x40564c)
    handlers = list(struct.unpack('<19I', read(0x406178, 19 * 4)))
    mode_handlers = list(struct.unpack('<7I', read(0x4061c4, 7 * 4)))
    if handlers[12] != 0x405caf or mode_handlers[6] != 0x4060fb:
        raise ValueError('STD12 dispatch changed')
    if 'jmp dword ptr [eax*4 + 0x406178]' not in [item['text'] for item in dispatch]:
        raise ValueError('STD dispatch decode changed')
    constants = {}
    for name, address, kind in (
            ('pi', 0x4a6580, 'd'), ('one', 0x4a6588, 'd'), ('frameStep', 0x4a6b48, 'd'),
            ('amplitude', 0x4a6b28, 'd'), ('upAmplitude', 0x4a6b20, 'd')):
        payload = read(address, 8 if kind == 'd' else 4)
        constants[name] = {'address': f'{address:#010x}', 'bytes': payload.hex(),
                           'value': struct.unpack('<' + kind, payload)[0]}
    samples = []
    for frame in (0, 1, 20, 60, 511):
        angle = math.pi - 2 * frame * math.pi / 512
        sine = math.sin(angle)
        samples.append({'frame': frame, 'angle': angle,
                        'offset': f32(-15 * sine), 'upX': f32(-0.01 * sine)})
    consumer = {
        'start': '0x00435211', 'endExclusive': '0x0043525b',
        'sha256': digest(read(0x435211, 0x43525b - 0x435211)),
        'instructions': disassemble(pe, 0x435211, 0x43525b),
        'application': '[global+0x3c..0x44] is added to [global+0..8] before the look-at call; +0x18 is the up vector',
    }
    fixture = {
        'schema': 'th128-std12-probe/1',
        'sourceExeSha256': EXE_SHA256,
        'stage': read_stage(),
        'dispatch': {'start': '0x00405633', 'endExclusive': '0x0040564c',
                     'sha256': digest(read(0x405633, 0x40564c - 0x405633)),
                     'instructions': dispatch, 'opcodeLimit': 0x12,
                     'opcode12Handler': f'{handlers[12]:#010x}',
                     'mode6Handler': f'{mode_handlers[6]:#010x}'},
        'opcode12Handler': {'start': f'{handlers[12]:#010x}', 'endExclusive': '0x00405d00',
                            'sha256': digest(read(handlers[12], 0x405d00 - handlers[12])),
                            'instructions': disassemble(pe, handlers[12], 0x405d00)},
        'mode6Handler': {'start': f'{mode_handlers[6]:#010x}', 'endExclusive': '0x0040615d',
                         'sha256': digest(read(mode_handlers[6], 0x40615d - mode_handlers[6])),
                         'instructions': disassemble(pe, mode_handlers[6], 0x40615d)},
        'constants': constants,
        'directState': {
            'mode': 'byte at command payload offset 8 (low byte of integer)',
            'zeroOffsetWhenModeZero': '[stage+0x3884..0x388c] written zero only when mode==0',
            'timerReset': '[stage+0x24]=-1, [stage+0x28]=0, [stage+0x2c]=0.0',
            'effectTable': [f'{value:#010x}' for value in mode_handlers],
            'implementedModes': [0, 6],
            'pendingModes': [1, 4, 5, 7],
            'noopModes': [2, 3] + list(range(8, 256)),
        },
        'cameraCopy': {
            'start': '0x00403b8a', 'endExclusive': '0x00403ba6',
            'sha256': digest(read(0x403b8a, 0x403ba6 - 0x403b8a)),
            'instructions': disassemble(pe, 0x403b8a, 0x403ba6),
            'stageOffset': '0x3884..0x388c -> global 0x4d32d8..0x4d32e0',
            'consumer': 'global camera struct is consumed by the look-at setup below',
        },
        'cameraConsumer': consumer,
        'mode6': {
            'sampleVectors': samples,
            'phase': 'pi - 2*time*pi/512',
            'offset': '-15*sin(phase) stored at stage+0x3884',
            'upX': '-0.01*sin(phase) stored at stage+0x3860',
            'timerStep': 'unit clock: frame++, time+=1.0; frames>=512 reset to -1/0/0',
            'sin': 'x87 fsin; sampled values matched host libm for these angles',
        },
        'knownGaps': [
            'mode1/4/5/7 remain explicit consumer-pending errors',
            'mode6 CPU vectors are not claimed bit-equivalent to the browser GPU path',
            'mode0 reset and modes2/3/8..255 no-op are retained',
        ],
    }
    OUTPUT.write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf8')
    print(f'stage_b2 opcode12 offset={fixture["stage"]["opcode12"]["offset"]} '
          f'mode={fixture["stage"]["opcode12"]["integers"]}; handler={handlers[12]:#x}; '
          f'mode6={mode_handlers[6]:#x}')
    print(f'wrote {OUTPUT.relative_to(ROOT)}')


if __name__ == '__main__':
    main()
