"""Bounded original-TH128 evidence for the post-Boss MSG21/22/30 scalar events.

Only the pinned read-only executable is opened; no original code is launched and
no state outside the emulator images is touched. Every read, code chunk and
emulation is bounded, and every emulation stops on an unimplemented original
call instead of inventing a return value.

    PYTHONPATH=<reverse-libs>;<tools/research> python tools/research/postboss_event_probe.py --exe ...

Output: local/research/postboss-event-fixtures.json
"""

import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

from bullet_probe import NativeBullet, ROOT, address, datum, emulate, STACK_POINTER

SCHEMA = 'th128-postboss-event-fixtures/1'
MANAGER, SESSION, FLOW, FLOW_GUARD, PLAYS, MESSAGE = 0x390000, 0x3a0000, 0x3b0000, 0x3e0000, 0x3c0000, 0x3d0000
PLAY_SIZE = 0x660 + 17 * 0x19b * 4 + 0x40
HANDLE = 0x1234abcd
FRAME = STACK_POINTER + 0x200

# Bounded original slices (start, end); each is an executable region actually
# decoded by Capstone before it is used in an emulation.
SLICES = {
    'dispatch': (0x4232a3, 0x4232bd),
    'handler21': (0x42454e, 0x4247f6),
    'handler22': (0x4247f6, 0x424841),
    'handler30': (0x42486a, 0x424928),
    'stageClearPrologue': (0x424fb0, 0x425040),
    'slotClearPrologue': (0x41fe20, 0x41fee0),
    'scoreAccumulate': (0x40e4e0, 0x40e514),
    'timeScaleHelper': (0x434a20, 0x434a6e),
    'routeAdvanceHelper': (0x4273a0, 0x427410),
}

# Original call boundaries the handlers reach. Production rules must not fake
# any of these; the probe stops on the call instruction itself.
CALLS = {
    'stageClearSpawn': 0x424fb0,  # spawns ANM 0x52 and resets manager score fields
    'stageClearSlots': 0x41fe20,  # clears the eight owned slot lists
    'saveReplay': 0x424561,       # call 0x438170
    'resetPath': 0x424586,        # call 0x437050
    'scoreAccumulate': 0x40e4e0,
    'setObjectState': 0x424682,   # call 0x411c70(eax=0x4d2e68, ecx=12)
    'routeAdvance': 0x42468b,     # call 0x4273a0([esi+0x78])
    'scoreFinalStage': 0x424601,  # labelB score call
    'scoreRouteFinal': 0x424776,  # labelA score call
    'timeScale': 0x424823,        # call 0x434a20(pushed float), plain stage
    'timeScaleFinal': 0x424837,   # call 0x434a20(pushed float), route-final stage
    'cutInCall': 0x424891,        # call 0x4670b0 for the post-Boss cut-in
    'cursorAdvance': 0x4248be,    # common MSG cursor advance tail
}

STAGE_CLEAR_STOPS = tuple(CALLS[name] for name in
                          ('saveReplay', 'resetPath', 'scoreFinalStage', 'scoreRouteFinal',
                           'setObjectState', 'cursorAdvance'))
FINAL_STAGES = (3, 5, 8, 10, 13, 15)
DIFFICULTY_MULTIPLIER = (5000, 10000, 20000, 40000, 40000)
# Raw bytes of the two stage-conditional floats, filled from the pinned image.
FLOAT_CONSTANTS = (struct.pack('<f', 8.0), struct.pack('<f', 2.0))


def read(native, start, size):
    return native.read(start, size)


def state(case, difficultyTable, floatConstants):
    manager = bytearray(0x5a40)
    struct.pack_into('<i', manager, 0x5a04, case.get('managerFlags', 0))
    session = bytearray(0x100)
    struct.pack_into('<i', session, 0x74, 1 if case.get('sessionPaused') else 0)
    flow = bytearray(0x100)
    struct.pack_into('<i', flow, 0x10, 1 if case.get('flowModeOne') else 0)
    struct.pack_into('<I', flow, 0x1c, FLOW_GUARD)
    guard = bytearray(0x40)
    guard[0xa] = 1 if case.get('flowGate') else 0
    plays = bytearray(PLAY_SIZE)
    stats = case.get('stats', case.get('stage', 1))
    difficulty = case.get('difficulty', 1)
    struct.pack_into('<i', plays, 0x660 + (stats * 0x19b + difficulty) * 4, case.get('playCount', 0))
    message = bytearray(0x1b0)
    struct.pack_into('<i', message, 0x80, case.get('messageTimer', 0))
    struct.pack_into('<i', message, 0x78, case.get('messageField78', 0))
    # Scalar globals share pages with the fields the handlers write, so every
    # page 0x4b4xxx/0x4b8xxx image is declared writable data. The observed
    # after-image still proves which fields the original code actually changed.
    return [
        datum('managerGlobal', 0x4b8950, MANAGER),
        datum('manager', MANAGER, bytes(manager)),
        datum('sessionGlobal', 0x4b8954, SESSION),
        datum('session', SESSION, bytes(session)),
        datum('flowGlobal', 0x4b8a84, FLOW),
        datum('flow', FLOW, bytes(flow)),
        datum('flowGuard', FLOW_GUARD, bytes(guard)),
        datum('playCountGlobal', 0x4b8a88, PLAYS),
        datum('playCounts', PLAYS, bytes(plays)),
        datum('message', MESSAGE, bytes(message)),
        datum('stage', 0x4b4d14, case.get('stage', 1)),
        datum('stageMirror', 0x4b4d18, 0),
        datum('statsStage', 0x4b4d44, stats),
        datum('difficulty', 0x4b4d0c, difficulty),
        datum('rateA', 0x4b4d64, case.get('rateA', 0)),
        datum('rateB', 0x4b4cdc, case.get('rateB', 0)),
        datum('nextStageRecord', 0x4b8a98, 0),
        datum('suspendFlags', 0x4b4d50, bytes([0x10 if case.get('suspend') else 0])),
        datum('difficultySwitch', 0x424a18, difficultyTable, writable=False),
        datum('timeConstantPlain', 0x4a674c, floatConstants[0], writable=False),
        datum('timeConstantFinal', 0x4a682c, floatConstants[1], writable=False),
    ]


def field(result, region, offset, kind='i'):
    after = bytes.fromhex(result['memory'][region]['after'])
    return struct.unpack_from('<' + kind, after, offset)[0]


def playCountField(result, case):
    stats = case.get('stats', case.get('stage', 1))
    return field(result, 'playCounts', 0x660 + (stats * 0x19b + case.get('difficulty', 1)) * 4)


def stackInt(result, index):
    head = bytes.fromhex(result['stackHead'])
    return struct.unpack_from('<i', head, index * 4)[0]


def run(chunks, entry, case, difficultyTable, *, registers=None, stops=(), stack=None):
    result = emulate(chunks, entry, state(case, difficultyTable, FLOAT_CONSTANTS),
                     registers={'esi': MESSAGE, **(registers or {})},
                     stops=stops, budget=512, stack=stack)
    result.pop('instructionTrace', None)
    result.pop('inputRegisters', None)
    return result


def probeMsg(path):
    binary = path.read_bytes()
    count = struct.unpack_from('<I', binary, 0)[0]
    offsets = [struct.unpack_from('<I', binary, 4 + index * 8)[0] for index in range(count)]
    entries = []
    for index, start in enumerate(offsets):
        end = offsets[index + 1] if index + 1 < count else len(binary)
        offset, commands = start, []
        while offset < end:
            time, opcode, length = struct.unpack_from('<HBB', binary, offset)
            commands.append({'time': time, 'opcode': opcode, 'length': length})
            offset += 4 + length
            if opcode == 0:
                break
        entries.append(commands)
    return {'path': str(path.relative_to(ROOT)).replace('\\', '/'),
            'sha256': hashlib.sha256(binary).hexdigest(), 'bytes': len(binary),
            'entries': count, 'entryCommandCounts': [len(item) for item in entries],
            'entry1Tail': [{'time': item['time'], 'opcode': item['opcode']} for item in entries[1][-9:]],
            'entry1TerminalOrder': [item['opcode'] for item in entries[1] if item['opcode'] in (21, 22, 30)],
            'entry0HasTerminalEvent': any(item['opcode'] in (21, 22, 30) for item in entries[0])}


def probe(executable, msg):
    native = NativeBullet(executable)
    try:
        chunks = {name: native.chunk(*span) for name, span in SLICES.items()}
        slices = {name: {'start': address(span[0]), 'endExclusive': address(span[1]),
                         'sha256': hashlib.sha256(read(native, span[0], span[1] - span[0])).hexdigest()}
                  for name, span in SLICES.items()}
        dispatchTable = native.constant(0x42497c, 128)
        difficultyTable = read(native, 0x424a18, 20)
        global FLOAT_CONSTANTS
        FLOAT_CONSTANTS = (read(native, 0x4a674c, 4), read(native, 0x4a682c, 4))
        table = struct.unpack('<32I', dispatchTable['bytes'])
        constants = {'floatAt4a674c': struct.unpack('<f', read(native, 0x4a674c, 4))[0],
                     'floatAt4a682c': struct.unpack('<f', read(native, 0x4a682c, 4))[0],
                     'floatAt4b6f58': struct.unpack('<f', read(native, 0x4b6f58, 4))[0],
                     'stageBaseScoreTableAddress': address(0x4b7220),
                     'stageBaseScoreTable': list(struct.unpack('<24i', read(native, 0x4b7220, 96))),
                     'cutInId': 0xa9, 'cutInLayer': 0x17, 'stageClearEffectId': 0x52,
                     'stageTableBase': address(0x4b1a30), 'stageRecordBytes': 40,
                     'stageClearSlotCount': 8, 'scoreCap': 0x3b9aca00,
                     'difficultyMultipliers': list(DIFFICULTY_MULTIPLIER),
                     'routeFinalStages': list(FINAL_STAGES), 'extraStage': 16,
                     'extraStageScoreBase': 250, 'extraStageRecord': address(0x4b1dc8)}

        # The dispatch reads the opcode byte at command+2 and jumps through 0x42497c.
        dispatchRuns = []
        dispatchMessage = bytearray(0x1b0)
        struct.pack_into('<I', dispatchMessage, 0x154, 0x360000)
        for opcode in (21, 22, 30):
            dispatchRun = emulate([chunks['dispatch']], 0x4232a3,
                                  [datum('command', 0x360000, struct.pack('<HBB', 0, opcode, 0), writable=False),
                                   datum('message', MESSAGE, bytes(dispatchMessage)), dispatchTable],
                                  registers={'esi': MESSAGE, 'eax': 0x360000}, budget=16)
            dispatchRun.pop('instructionTrace', None)
            dispatchRun.pop('inputRegisters', None)
            dispatchRuns.append({'opcode': opcode, 'stop': dispatchRun['stop']})

        # 22: stage-conditional float delivered to the original helper 0x434a20.
        timeScale = []
        for stage in range(18):
            case = {'stage': stage}
            result = run([chunks['handler22']], 0x4247f6, case, difficultyTable,
                         stops=(CALLS['timeScale'], CALLS['timeScaleFinal'], CALLS['cursorAdvance']))
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            stop = result['stop']['address']
            value = None
            if stop in (address(CALLS['timeScale']), address(CALLS['timeScaleFinal'])):
                value = struct.unpack('<f', struct.pack('<I', stackInt(result, 0) & 0xffffffff))[0]
            timeScale.append({'stage': stage, 'stop': stop, 'pushedFloat': value,
                              'stageClamped': stage in FINAL_STAGES,
                              'managerFlags': field(result, 'manager', 0x5a04, 'I'),
                              'stageAfter': field(result, 'stage', 0)})

        # 30: gate, opaque ANM spawn and the message timer fields after the call.
        cutIn = []
        for paused in (0, 1):
            case = {'sessionPaused': paused, 'messageTimer': 5}
            result = run([chunks['handler30']], 0x42486a, case, difficultyTable,
                         registers={'ebp': FRAME}, stops=(CALLS['cutInCall'], CALLS['cursorAdvance']))
            calls = result['stop']['address'] == address(CALLS['cutInCall'])
            cutIn.append({'input': case, 'stop': result['stop']['reason'],
                          'stopAddress': result['stop']['address'], 'spawnsAnm': calls,
                          'stackArguments': [stackInt(result, index) for index in range(4)] if calls else None,
                          'messageAfter': {'handle6c': field(result, 'message', 0x6c, 'I'),
                                           'field70': field(result, 'message', 0x70),
                                           'field74': field(result, 'message', 0x74),
                                           'field78': field(result, 'message', 0x78)}})
        for timer in (0, 5, -5):
            case = {'sessionPaused': 0, 'messageTimer': timer}
            result = run([chunks['handler30']], 0x424896, case, difficultyTable,
                         registers={'ebp': FRAME}, stops=(CALLS['cursorAdvance'],),
                         stack={0x200 - 0x64: struct.pack('<I', HANDLE)})
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            cutIn.append({'input': case, 'stop': result['stop']['reason'],
                          'stopAddress': result['stop']['address'],
                          'postCallPrecondition': {'frameSlot': address(0x200 - 0x64), 'handle': address(HANDLE)},
                          'messageAfter': {'handle6c': field(result, 'message', 0x6c, 'I'),
                                           'field70': field(result, 'message', 0x70),
                                           'field74': field(result, 'message', 0x74),
                                           'field78': field(result, 'message', 0x78)}})

        # 21: prologue calls, branch selection and the branch scalar effects.
        stageClear = []
        variants = ((0, 0, 0), (1, 0, 0), (0, 1, 1), (0, 1, 0))
        for stage in range(18):
            for suspend, paused, gate in variants:
                if (suspend or paused) and stage not in (1, 2, 3, 5, 16):
                    continue
                case = {'stage': stage, 'stats': 3 if stage == 3 else stage, 'difficulty': 1,
                        'rateA': 12345, 'rateB': 98765, 'playCount': 41,
                        'suspend': suspend, 'sessionPaused': paused, 'flowGate': gate,
                        'messageField78': 7}
                result = run([chunks['handler21']], 0x424558, case, difficultyTable,
                             stops=STAGE_CLEAR_STOPS)
                if result['stop']['reason'] != 'boundary':
                    raise ValueError(result['stop'])
                stageClear.append({'input': case, 'stop': result['stop']['address'],
                                   'managerFlags': field(result, 'manager', 0x5a04, 'I'),
                                   'managerBonus': field(result, 'manager', 0x5a38)})

        # Branch score argument: stop on the untouched score call, then continue
        # after it with the register carry the original code itself leaves.
        scoreCases = []

        def scoreCase(stage, difficulty, stats, rateA=12345, rateB=98765, playCount=41,
                      flowModeOne=0):
            case = {'stage': stage, 'stats': stats, 'difficulty': difficulty,
                    'rateA': rateA, 'rateB': rateB, 'playCount': playCount,
                    'messageField78': 7, 'flowModeOne': flowModeOne}
            extra = stage == 16
            entry = 0x4245d4 if extra else 0x424695
            stops = (CALLS['scoreFinalStage'],) if extra else (CALLS['scoreRouteFinal'],)
            started = run([chunks['handler21']], entry, case, difficultyTable, stops=stops,
                          registers={'eax': 0x10} if extra else None)
            if started['stop']['reason'] != 'boundary':
                raise ValueError(started['stop'])
            bonus = stackInt(started, 0)
            registers = ({'ebx': bonus, 'edi': MANAGER, 'esi': MESSAGE} if extra
                         else {'ebx': MANAGER, 'edi': bonus, 'esi': MESSAGE})
            followEntry = 0x424606 if extra else 0x42477b
            follow = run([chunks['handler21']], followEntry, {**case, 'managerFlags': 0x10},
                         difficultyTable, registers=registers,
                         stops=(CALLS['resetPath'], CALLS['cursorAdvance']))
            if follow['stop']['reason'] != 'boundary':
                raise ValueError(follow['stop'])
            record = {'input': case, 'scoreStop': started['stop']['address'],
                      'bonusRaw': bonus,
                      'registerCarry': {'ebx': hex(registers['ebx']),
                                        'edi': hex(registers['edi'])},
                      'continueEntry': address(followEntry),
                      'continueStop': follow['stop']['address'],
                      'managerFlagsAfter': field(follow, 'manager', 0x5a04, 'I'),
                      'managerBonusAfter': field(follow, 'manager', 0x5a38),
                      'managerField5a3c': field(follow, 'manager', 0x5a3c),
                      'stageAfter': field(follow, 'stage', 0),
                      'stageMirrorAfter': field(follow, 'stageMirror', 0),
                      'nextStageRecordAfter': field(follow, 'nextStageRecord', 0, 'I'),
                      'playCountAfter': playCountField(follow, case)}
            scoreCases.append(record)

        for stage, difficulty, stats in ((3, 0, 3), (3, 1, 3), (3, 2, 3), (3, 3, 3), (3, 4, 3),
                                         (5, 3, 5), (8, 3, 8), (10, 3, 10), (13, 3, 13), (15, 3, 15),
                                         (16, 3, 16)):
            scoreCase(stage, difficulty, stats)
        # Truncating division and 32-bit wrap on the same proven arithmetic.
        scoreCase(3, 2, 3, rateA=-12345, rateB=-98765)
        scoreCase(5, 2, 5, rateA=-12345, rateB=500)
        # The plausible-play counter at its original 99999 cap must not increment.
        scoreCase(5, 1, 5, playCount=99999)
        # The 0x4b8a84+0x10 gate short-circuits both branches to the reset call.
        scoreCase(3, 1, 3, flowModeOne=1)
        scoreCase(16, 1, 16, flowModeOne=1)

        # The labelC tail pushes the message field 0x78 into the route helper.
        routeCase = {'stage': 2, 'stats': 2, 'messageField78': 7}
        route = run([chunks['handler21']], 0x424687, routeCase, difficultyTable,
                    stops=(CALLS['routeAdvance'],))
        if route['stop']['reason'] != 'boundary':
            raise ValueError(route['stop'])
        routeArm = {'pushValue': stackInt(route, 0), 'stop': route['stop']['address']}

        # The prologue helpers are composite original routines: bounded static
        # slices only, so production must list them as required operations.
        prologue = {
            'stageClearPrologue': ['0x4670b0(id=0x52,layer=0x17,flags=0) -> manager+0x58cc',
                                   'manager+0x5a38 = 0x4b7220[stage]',
                                   'score[0x4b4cc4] += 0x4b7220[stage]/10 (cap 0x3b9ac9ff)',
                                   'manager+0x5a04 |= 0x100'],
            'slotClearPrologue': ['[0x4b8a80]+0x438ec |= 0x10',
                                  'eight owned lists at [0x4b8a80]+0x8868',
                                  'entry+0x3f8 = 3 and [entry+0x4c8] callback'],
            'slices': ['stageClearPrologue', 'slotClearPrologue'],
        }
        return {'schema': SCHEMA,
                'sourceExeSha256': hashlib.sha256(native.executable.read_bytes()).hexdigest(),
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                'msg': probeMsg(msg),
                'dispatch': {'code': {'start': address(0x4232a3), 'endExclusive': address(0x4232bd),
                                      'sha256': slices['dispatch']['sha256']},
                             'tableAddress': '0x42497c', 'tableBytes': dispatchTable['bytes'].hex(),
                             'tableSha256': hashlib.sha256(dispatchTable['bytes']).hexdigest(),
                             'handlers': {str(opcode): address(value)
                                          for opcode, value in enumerate(table[:31])},
                             'emulatedDispatch': dispatchRuns},
                'slices': slices, 'constants': constants,
                'callBoundaries': {name: address(value) for name, value in CALLS.items()},
                'prologue': prologue,
                'timeScaleCases': timeScale, 'cutInCases': cutIn,
                'stageClearCases': stageClear, 'scoreCases': scoreCases,
                'routeArm': routeArm,
                'apiReturnsAssigned': False, 'launchesOriginalExecutable': False}
    finally:
        native.image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--msg')
    parser.add_argument('--out', default=str(ROOT / 'local/research/postboss-event-fixtures.json'))
    arguments = parser.parse_args(argv)
    if Path(arguments.out).resolve().parent != (ROOT / 'local/research').resolve():
        raise ValueError('report must be delivered to local/research')
    msg = Path(arguments.msg) if arguments.msg else (
        ROOT / 'local' / json.loads((ROOT / 'local/catalog.json').read_text(encoding='utf-8'))
        ['variants']['ja']['manifest']).parent / 'raw/st_a1.msg'
    report = probe(Path(arguments.exe), msg)
    Path(arguments.out).write_text(json.dumps(report, indent=1, ensure_ascii=False) + '\n',
                                   encoding='utf-8')
    print('postboss MSG entries %d terminal order %s; %d time-scale, %d cut-in, %d stage-clear, %d score cases'
          % (report['msg']['entries'], report['msg']['entry1TerminalOrder'],
             len(report['timeScaleCases']), len(report['cutInCases']),
             len(report['stageClearCases']), len(report['scoreCases'])))


if __name__ == '__main__':
    main(sys.argv[1:])
