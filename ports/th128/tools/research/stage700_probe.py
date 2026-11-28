import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
sys.path.insert(0, str(ROOT / 'tools'))

from bullet_probe import NativeBullet, datum, emulate, CONTEXT, TASK, STATE, COMMAND
from th128_assets.formats import parse_ecl

EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
ECL = ROOT / 'local/generations/8c71c316a4b24e68827f9fc560242050/zh/raw/st_a2_2.ecl'
ECL_SHA256 = 'af2e963cb2cf85eb40160fec2a2f05cc38837ac286681594f1c6b66b52f26e83'
DISPATCHER = (0x418030, 0x41806d)
RETURN = (0x41c52c, 0x41c52e)
INDICES = (0x41c7c0, 360)
TARGETS = (0x41c554, 620)
HANDLED = 502


def require(condition, message):
    if not condition:
        raise ValueError(message)


def main_command(ecl_path):
    source = ecl_path.read_bytes()
    require(hashlib.sha256(source).hexdigest() == ECL_SHA256, 'stage4 ECL hash mismatch')
    program = parse_ecl(source)
    main = next(value for value in program['subroutines'] if value['name'] == 'main')
    command = next(value for value in main['commands'] if value['offset'] == 40)
    require(main['offset'] == 38000 and command['opcode'] == 700 and command['time'] == 0
            and command['mask'] == 0 and command['parameterCount'] == 1
            and command['raw'] == 'f4010000', 'stage4 main+40 command mismatch')
    return source, command


def encode(command, payload, *, opcode=None, mask=None, parameter_count=None):
    opcode = command['opcode'] if opcode is None else opcode
    mask = command['mask'] if mask is None else mask
    parameter_count = command['parameterCount'] if parameter_count is None else parameter_count
    return struct.pack('<iHHHBBI', command['time'], opcode, 16 + len(payload), mask,
                       command['rankMask'], parameter_count, command['stackOffset']) + payload


def dispatch(native, command, *, stop, include_handler, budget=64):
    context = bytearray(0x4000)
    struct.pack_into('<I', context, 0x3f7c, TASK)
    task = struct.pack('<2I', 0, STATE)
    state = bytearray(0x1020)
    struct.pack_into('<I', state, 4, COMMAND)
    chunks = [native.chunk(*DISPATCHER)]
    chunks.append(native.chunk(0x41a8c3, 0x41a8c5) if include_handler else native.chunk(*RETURN))
    data = [
        datum('context', CONTEXT, bytes(context)),
        datum('task', TASK, task, writable=False),
        datum('state', STATE, bytes(state)),
        datum('command', COMMAND, command, writable=False),
        native.constant(*INDICES),
        native.constant(*TARGETS),
    ]
    result = emulate(chunks, DISPATCHER[0], data,
                     registers={'ecx': CONTEXT, 'ebx': CONTEXT}, stops=[stop], budget=budget)
    changed = {name: result['memory'][name]['before'] != result['memory'][name]['after']
               for name in ('context', 'task', 'state', 'command')}
    return {
        'stop': result['stop'],
        'eax': result.get('registers', {}).get('eax'),
        'changed': changed,
        'handlerReached': include_handler,
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded TH128 stage4 opcode 700 dispatch probe.')
    parser.add_argument('--exe', type=Path, default=Path(r'E:\gee\妖精大战争\th128.exe'))
    parser.add_argument('--ecl', type=Path, default=ECL)
    parser.add_argument('--report', type=Path, default=ROOT / 'local/research/stage700-fixtures.json')
    arguments = parser.parse_args(argv)
    destination = arguments.report.resolve()
    allowed = (ROOT / 'local/research').resolve()
    require(destination.parent == allowed and destination.name == 'stage700-fixtures.json',
            'report must be local/research/stage700-fixtures.json')
    require(arguments.exe.is_file(), 'original executable is unavailable')
    native = NativeBullet(arguments.exe)
    require(hashlib.sha256(native.binary).hexdigest() == EXE_SHA256, 'original executable hash mismatch')
    source, command = main_command(arguments.ecl)
    indices = native.read(*INDICES)
    targets = native.read(*TARGETS)
    index = 700 - 256
    require(index >= len(indices), 'opcode 700 unexpectedly has a table index')
    handler_offset = struct.unpack_from('<I', targets, indices[HANDLED - 256] * 4)[0]
    require(handler_offset == 0x41a8c3, 'handled opcode dispatch mismatch')
    require(native.read(RETURN[0], RETURN[1] - RETURN[0]).hex() == '33c0',
            'out-of-range return is not xor eax,eax')

    actual_payload = bytes.fromhex(command['raw'])
    cases = []
    for label, payload, opcode, mask, parameter_count, include_handler in (
            ('main+40 actual', actual_payload, 700, 0, 1, False),
            ('700/operand0', struct.pack('<i', 0), 700, 0, 1, False),
            ('700/operand-1', struct.pack('<i', -1), 700, 0, 1, False),
            ('700/operand2147483647', struct.pack('<i', 2147483647), 700, 0, 1, False),
            ('700/float-mask', actual_payload, 700, 1, 1, False),
            ('700/no-operand', b'', 700, 0, 0, False),
            ('616/out-of-range', b'', 616, 0, 0, False),
            ('502/handled-dispatch', struct.pack('<2i', 7, 9), HANDLED, 0, 2, True)):
        encoded = encode(command, payload, opcode=opcode, mask=mask, parameter_count=parameter_count)
        stop = 0x41a8c3 if include_handler else 0x41c52e
        result = dispatch(native, encoded, stop=stop, include_handler=include_handler)
        result.update(label=label, opcode=opcode, parameterCount=parameter_count, mask=mask,
                      payload=payload.hex(), encoded=encoded.hex())
        cases.append(result)

    actual = cases[0]
    require(actual['eax'] == 0 and actual['stop']['reason'] == 'boundary'
            and actual['stop']['address'].lower() == '0x0041c52e', 'actual 700 dispatch did not return 0')
    require(not any(actual['changed'].values()), 'actual 700 dispatch changed ECL state')
    for case in cases[1:6]:
        require(case['eax'] == 0 and not any(case['changed'].values()),
                '700 operand variant changed dispatcher behavior')
    require(cases[6]['eax'] == 0 and not any(cases[6]['changed'].values()), '616 did not short-circuit')
    require(cases[7]['handlerReached'] and cases[7]['stop']['address'].lower() == '0x0041a8c3',
            'handled opcode did not reach its original handler')

    report = {
        'schema': 'th128-stage700/1',
        'executable': {'path': str(arguments.exe), 'sha256': hashlib.sha256(native.binary).hexdigest()},
        'ecl': {'path': str(arguments.ecl.resolve()), 'sha256': hashlib.sha256(source).hexdigest(),
                'subroutine': 'main', 'offset': 40, 'time': command['time'], 'opcode': command['opcode'],
                'mask': command['mask'], 'parameterCount': command['parameterCount'],
                'rankMask': command['rankMask'], 'stackOffset': command['stackOffset'],
                'raw': command['raw'], 'operand': 500},
        'dispatch': {
            'dispatcher': '0x418030..0x41806d',
            'dispatcherSha256': hashlib.sha256(native.read(DISPATCHER[0], DISPATCHER[1] - DISPATCHER[0])).hexdigest(),
            'indexTable': '0x41c7c0', 'jumpTable': '0x41c554',
            'minOpcode': 256, 'maxOpcode': 256 + len(indices) - 1,
            'opcode700Index': index, 'opcode700IndexInRange': index < len(indices),
            'outOfRangeTarget': '0x41c52c', 'outOfRangeBytes': native.read(RETURN[0], RETURN[1] - RETURN[0]).hex(),
            'handledOpcode': HANDLED, 'handledHandler': '0x41a8c3',
        },
        'cases': cases,
        'conclusion': {
            'opcode': 700,
            'enemyDispatchTableMember': False,
            'handler': None,
            'dispatchResult': 0,
            'operandRead': False,
            'stateWrites': False,
            'handlerCall': False,
            'scalarHandlerFound': False,
            'moduleJustified': False,
            'reason': 'opcode-256=444 exceeds the 360-entry table; the dispatcher goes directly to xor eax,eax and returns 0 without reading the command operands or touching ECL state',
        },
        'limitations': [
            'This proves the TH128 enemy ECL dispatcher only; it does not identify or prove the missing stage-controller interpreter that may own opcode 700.',
            'The stage4 main is currently executed by the reconstruction as a root entity; the literal-main spawn is established, but a separate stage-script owner is not excluded.',
            'No sound, background or globalContext access is observed on this path; no typed operation is justified from this evidence.',
        ],
        'probeSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
    }
    destination.write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print(json.dumps({key: report[key] for key in ('dispatch', 'conclusion')}, separators=(',', ':')))
    print(f"cases={len(cases)} report={destination}")


if __name__ == '__main__':
    main()
