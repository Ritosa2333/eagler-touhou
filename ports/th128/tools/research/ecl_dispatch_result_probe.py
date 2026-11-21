import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
from bullet_probe import NativeBullet, datum, emulate

EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
STAGE700 = ROOT / 'local/research/stage700-fixtures.json'
STATE, COMMAND = 0x300000, 0x360000


def require(condition, message):
    if not condition:
        raise ValueError(message)


def read(native, address, size):
    return native.read(address, size)


def cursor_case(native, result):
    state = bytearray(0x2000)
    struct.pack_into('<I', state, 4, COMMAND)
    command = struct.pack('<iHHHBBI', 0, 700, 20, 0, 255, 1, 0) + struct.pack('<i', 500)
    chunks = [native.chunk(0x46fbe0, 0x46fbf1), native.chunk(0x46fc0c, 0x46fc12)]
    data = [datum('state', STATE, bytes(state)), datum('command', COMMAND, command, writable=False)]
    stop = 0x46fc0c if result == -1 else 0x46fbf1
    run = emulate(chunks, 0x46fbe0, data, registers={'ebx': STATE, 'eax': result}, stops=[stop], budget=32)
    after = bytes.fromhex(run['memory']['state']['after'])
    return {
        'hostResult': result,
        'stop': run['stop'],
        'cursorBefore': COMMAND,
        'cursorAfter': struct.unpack_from('<I', after, 4)[0],
        'advanced': struct.unpack_from('<I', after, 4)[0] == COMMAND + 20,
    }


def probe(exe, stage700):
    native = NativeBullet(exe)
    require(hashlib.sha256(native.binary).hexdigest() == EXE_SHA256, 'original executable hash mismatch')
    cases = [cursor_case(native, value) for value in (0, 1, -1)]
    for case in cases[:2]:
        require(case['advanced'] and case['stop']['address'].lower() == '0x0046fbf1',
                'non-negative host result did not advance the cursor')
    require(not cases[2]['advanced'] and cases[2]['stop']['address'].lower() == '0x0046fc0c',
            'negative host result advanced the cursor')
    existing = json.loads(stage700.read_text(encoding='utf8'))
    actual = next(value for value in existing['cases'] if value['label'] == 'main+40 actual')
    require(actual['eax'] == 0 and actual['stop']['address'].lower() == '0x0041c52e'
            and not any(actual['changed'].values()), 'existing opcode700 result is not the expected zero return')
    table = read(native, 0x46fdc0, 0x5d)
    targets = read(native, 0x46fcc0, 0x174)
    opcode_handlers = {}
    for opcode in (40, 83, 10):
        index = table[opcode]
        opcode_handlers[str(opcode)] = hex(struct.unpack_from('<I', targets, index * 4)[0])
    return {
        'schema': 'th128-ecl-dispatch-result/1',
        'executable': {'path': str(exe), 'sha256': hashlib.sha256(native.binary).hexdigest()},
        'callerChain': {
            'enemyUpdate': '0x416160',
            'eclThreadLoop': '0x46fe20',
            'genericVm': '0x46e100',
            'enemyUpdateCall': {'address': '0x4164e1', 'bytes': read(native, 0x4164e1, 5).hex()},
            'threadLoopCall': {'address': '0x46fe52', 'bytes': read(native, 0x46fe52, 5).hex()},
            'hostCall': {'address': '0x46fbde', 'bytes': read(native, 0x46fbd4, 0x0c).hex(),
                         'slot': '[state+0x1014]', 'entry': 'vtable[0]'},
            'hostResultCompare': {'address': '0x46fbe0', 'bytes': read(native, 0x46fbe0, 5).hex(),
                                  'note': 'cmp eax,-1; je 0x46fc0c'},
        },
        'genericVmEvidence': {
            'opcodeRange': '0..0x5c', 'hostBranch': '0x46fbd4', 'opcodeHandlers': opcode_handlers,
            'note': 'opcode83 subtracts time, opcode40 allocates locals, opcode10 returns/restores stack',
        },
        'cursorCases': cases,
        'timeBranch': {
            'address': '0x46fbf1..0x46fc0c',
            'bytes': read(native, 0x46fbf1, 0x1b).hex(),
            'note': 'after cursor advance, next command time is compared with thread time; not due adds the caller frame delta at [ebp+8]',
        },
        'dispatcherReturn': {
            'success': {'address': '0x41c52c', 'bytes': read(native, 0x41c52c, 2).hex()},
            'negative': {'address': '0x41c52e', 'bytes': read(native, 0x41c52e, 7).hex(),
                         'note': 'skips xor eax,eax and preserves -1'},
        },
        'realHostHold419': {
            'handler': '0x41b3b5',
            'negativeBranch': {'address': '0x41b3b5', 'orAddress': '0x41b3d5', 'jmpAddress': '0x41b3d8',
                               'bytes': read(native, 0x41b3b5, 0x28).hex()},
            'note': 'message gate closed -> or eax,-1; jmp 0x41c52e',
        },
        'existing700': {
            'fixture': str(stage700.resolve()),
            'case': 'main+40 actual',
            'eax': actual['eax'], 'stop': actual['stop'],
            'changed': actual['changed'],
        },
        'conclusion': {
            'hostResultZero': 'success/continue: original VM advances [state+4] by word[command+6] at 0x46fbe5',
            'hostResultNonZeroNonNegative': 'also advances; only -1 is special',
            'hostResultNegative': 'hold/terminal: original VM jumps to 0x46fc0c and does not advance the cursor',
            'opcode700': 'the identified ECL host dispatcher returns 0 for 700 (existing fixture); the generic VM advances on result 0. The host-slot setter remains a prefix, so this is an evidence-backed host-callback skip, not a separate StageScriptController',
        },
        'safeRules': [
            'The generic VM advances only by the original command size at [command+6]; keep that field intact and bounds-checked.',
            'Only callback result -1 suppresses cursor advance; do not treat all nonzero results as hold.',
            'Original host table range is 256..615. Out-of-table opcodes (including 700) return 0 and are skipped; unregistered in-range host opcodes still need their real handler and must not be silently skipped.',
            'Opcode700 operands are not read by the out-of-table path; do not add a StageWorld 700 handler or arbitrary argument acceptance.',
        ],
        'patchContract': {
            'file': 'src/ecl/EclVm.cpp',
            'point': 'Vm::executeHost / host callback return path (current hostOpcodes_ lookup around lines 609-627)',
            'change': 'use HostResultRules.hpp: result != -1 -> advance the command cursor by the instruction size; result == -1 -> retain the current command and take the hold/terminal path, not a generic fault',
            'outOfTable': 'for opcode outside 256..615 with no host handler, return 0 (original dispatcher out-of-table) so it advances; for unregistered 256..615 keep explicit unsupported',
            'notNow': 'do not edit EclVm/StageWorld in this task; StageWorld owner receives this single contract',
        },
        'limitations': [
            'The host callback slot [state+0x1014] is identified, but its exact setter is outside this bounded slice.',
            'The cursor fragment starts after the host call, so the host callback itself is a prefix; opcode700 return 0 comes from the existing independent fixture.',
            'Opcode419 supplies a real -1 branch statically; no full message-gate runtime was executed.',
        ],
        'probeSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded TH128 ECL host-result/cursor contract probe.')
    parser.add_argument('--exe', type=Path, default=Path(r'E:\gee\妖精大战争\th128.exe'))
    parser.add_argument('--report', type=Path, default=ROOT / 'local/research/ecl-dispatch-result-fixtures.json')
    arguments = parser.parse_args(argv)
    destination = arguments.report.resolve()
    allowed = (ROOT / 'local/research').resolve()
    require(destination.parent == allowed and destination.name == 'ecl-dispatch-result-fixtures.json',
            'report must be local/research/ecl-dispatch-result-fixtures.json')
    require(arguments.exe.is_file(), 'original executable is unavailable')
    report = probe(arguments.exe, STAGE700)
    destination.write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print(json.dumps({'cursorCases': report['cursorCases'], 'conclusion': report['conclusion']},
                     separators=(',', ':')))
    print(f"report={destination}")


if __name__ == '__main__':
    main()
