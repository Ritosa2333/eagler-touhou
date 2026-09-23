"""Real keyboard-driven MP stage completion and native menu Replay verification.

The diagnostic robot can observe native positions and emit keyboard input. It
cannot change a world owner, skip ECL, call stage completion, grant lives/power,
set invulnerability, or patch the saved tape. Its output is an ordinary all-seat
input recording, replayed by a fresh Runtime with different saved history.
"""
import argparse
import hashlib
import json
from pathlib import Path

from rollback_testkit import barrier, call, fixture

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8140/')
parser.add_argument('--output', default='artifacts/multiplayer-tests/replay-completion.json')
parser.add_argument('--case', choices=('stage', 'normal', 'extra'), default='stage')
parser.add_argument('--players', type=int, choices=(2, 3), default=2)
parser.add_argument('--target-stage', type=int, choices=range(2, 7), default=2)
phase = parser.add_mutually_exclusive_group()
phase.add_argument('--record-only', action='store_true')
phase.add_argument('--recorded-run', type=Path)
parser.add_argument('--playback-part', choices=('all', 'full', 'seek', 'browser-seek', 'escape-seek'), default='all')
args = parser.parse_args()
if args.case != 'stage' and args.playback_part not in ('all', 'full'):
    parser.error('seek playback parts require --case stage')
output = Path(args.output)
report = {'passed': False, 'scope': 'native keyboard-driven MP Replay completion',
          'case': args.case, 'players': args.players, 'targetStage': args.target_stage,
          'phase': 'record' if args.record_only else 'playback' if args.recorded_run else 'roundtrip',
          'playbackPart': args.playback_part,
          'coverage': ['all exposed pilot/economy values', 'both RNG seed/call counts',
                       'read-only playback', 'native stage/menu lifecycle'],
          'notClaimed': ['human gameplay acceptance', 'complete portable world hash', 'device performance'],
          'robotSourceSha256': hashlib.sha256(Path(__file__).with_name('replay-robot.mjs').read_bytes()).hexdigest()}


def checkpoint():
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2), encoding='utf-8')


def compare(actual, original):
    assert actual['replay'][4] == original['replay'][4], ('archive cursor', actual, original)
    actual_global_frame = actual['frame'] + actual['replay'][10]
    original_global_frame = original['frame'] + original['replay'][10]
    assert actual_global_frame == original_global_frame, ('global native frame', actual, original)
    for key in ('state', 'rng'):
        assert actual[key] == original[key], {
            'firstMismatch': actual['replay'][4]-1, 'category': key,
            'expected': original[key], 'actual': actual[key]}


def open_viewer(test, data, selected_stage=1):
    context = test.browser.new_context(service_workers='block')
    test.contexts.append(context)
    page = context.new_page()
    page.on('pageerror', lambda error: test.errors.append(str(error)))
    page.goto(args.url)
    page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
    test.identities.append(call(page, 'multiplayerSmoke.identity()'))
    assert call(page, 'multiplayerSmoke.historyProfile(2)')
    call(page, '(b)=>multiplayerSmoke.viewerStart(b)', data)
    call(page, 'multiplayerSmoke.viewerTicks(400)')

    def press(code, wait=55):
        call(page, '(c)=>multiplayerSmoke.key(c,true)', code)
        call(page, 'multiplayerSmoke.viewerTicks(2)')
        call(page, '(c)=>multiplayerSmoke.key(c,false)', code)
        call(page, '(n)=>multiplayerSmoke.viewerTicks(n)', wait)

    press('KeyZ')
    press('KeyZ')
    before = call(page, 'multiplayerSmoke.savedFiles()')
    press('KeyZ')
    for _ in range(selected_stage-1 if selected_stage < 7 else 0):
        press('ArrowDown', 20)
    press('KeyZ')
    return page, before, press


def record_run(test):
    loadouts = [[0, 0], [1, 0], [0, 2]][:args.players]
    pages = [test.open(local=seat, loadouts=loadouts,
                       difficulty=4 if args.case == 'extra' else 0, history=1)
             for seat in range(args.players)]
    barrier(pages)
    record = pages[-1]
    trace = []
    max_frames = 100000 if args.case == 'extra' else 200000 if args.case == 'normal' else args.target_stage*20000
    for batch in range((max_frames+2000)//200):
        result = call(record, '(stage)=>multiplayerSmoke.recordKeyboardBatch(200,stage)',
                      args.target_stage if args.case == 'stage' else 0)
        trace.extend(result['rows'])
        p = result['probe']
        report['recording'] = {'frames': len(trace), 'probeHeader': p[:16],
                               'pilots': p[16:52], 'controls': result['controls'], 'net': result['net'],
                               'replay': result['replay'], 'life': result['life'],
                               'stageReady': result['stageReady'], 'saved': result['saved']}
        checkpoint()
        if batch % 6 == 0:
            print(json.dumps({'phase': 'record', **report['recording']}), flush=True)
        if args.case == 'stage' and result['stageReady']:
            assert call(record, 'multiplayerSmoke.saveReplay()')
            break
        if result['saved']:
            assert args.case in ('normal', 'extra') and p[11] == 1 and p[4] == 8, (
                'native clear/ranking marker not reached', p[:16])
            break
        assert len(trace) < max_frames, 'Native stage/result target was not reached within the bounded input run'
    else:
        raise AssertionError('Native recording never completed its target')
    data = call(record, 'multiplayerSmoke.replayBytes()')
    saved_frames = int.from_bytes(bytes(data[32:36]), 'little')
    description_bytes = int.from_bytes(bytes(data[24:28]), 'little')
    chapter_count = int.from_bytes(bytes(data[28:32]), 'little')
    chapters = [int.from_bytes(bytes(data[40+description_bytes+i*8:44+description_bytes+i*8]), 'little')
                for i in range(chapter_count)]
    if args.case == 'stage':
        assert any(label & 255 == args.target_stage for label in chapters), ('No requested native stage input chapter', chapters)
    # A result save records exactly through its native save-confirmation input.
    # No unsimulated tail is appended, and no missing frame is filled in.
    assert saved_frames == len(trace), (saved_frames, len(trace))
    assert [row['replay'][4] for row in trace] == list(range(1,len(trace)+1))
    tape = output.with_suffix('.rpyx')
    tape.write_bytes(bytes(data))
    trace_path = output.with_name(output.stem+'-trace.json')
    trace_path.write_text(json.dumps(trace,separators=(',',':')), encoding='utf-8')
    report.update(replayFile=str(tape.resolve()), fileSha256=hashlib.sha256(bytes(data)).hexdigest(),
                  traceFile=str(trace_path.resolve()), traceSha256=hashlib.sha256(trace_path.read_bytes()).hexdigest(),
                  recordedFrames=len(trace), chapters=chapters, recordingComplete=True,
                  recordingIdentity=test.identities[-1])
    checkpoint()
    test.close_contexts()
    return data, trace


def playback_run(test, data, trace):
    if args.playback_part in ('all', 'full'):
        playback, before_files, press = open_viewer(test, data, 7 if args.case == 'extra' else 1)
        for key in ('wasmSha256', 'sourceDigest', 'assets'):
            assert test.identities[-1][key] == report['recordingIdentity'][key], ('Recording/playback identity differs', key)
        compared = 0
        for _ in range(len(trace)//200+20):
            rows = call(playback, 'multiplayerSmoke.takeReplayTrace()')
            for row in rows:
                assert compared < len(trace), 'Playback exceeded the recorded input boundary'
                compare(row,trace[compared]);compared += 1
            current = call(playback, 'multiplayerSmoke.replayStatus()')
            report['playback'] = {'comparedFrames': compared, 'status': current}
            checkpoint()
            if current[6]:
                break
            call(playback, 'multiplayerSmoke.viewerTicks(200)')
            if compared % 1000 < 200:
                print(json.dumps({'phase':'playback','compared':compared,'total':len(trace)}),flush=True)
        assert compared == len(trace) and current[6] == 1, (compared,len(trace),current)
        assert call(playback, 'multiplayerSmoke.savedFiles()') == before_files
        press('Escape',100)
        assert call(playback, 'multiplayerSmoke.replayStatus()')[8] == 0
        assert call(playback, 'multiplayerSmoke.savedFiles()') == before_files
        report['fullPlayback'] = {'passed':True,'comparedFrames':compared,'readOnly':True}
        checkpoint()
        test.close_contexts()
        if args.playback_part == 'full':
            return

    if args.case == 'stage':
        if args.playback_part in ('all', 'seek'):
            seek, before_files, press = open_viewer(test,data,args.target_stage)
            status = call(seek,'multiplayerSmoke.replayStatus()')
            target = status[10]
            assert target > 0 and status[7] == 0 and status[11] == 0 and status[12] == args.target_stage, status
            rows = call(seek,'multiplayerSmoke.takeReplayTrace()')
            assert rows and rows[0]['replay'][4] == target+1, (target, rows[:1])
            compared = 0
            for row in rows:
                compare(row,trace[row['replay'][4]-1]);compared += 1
            call(seek,'multiplayerSmoke.viewerTicks(40)')
            for row in call(seek,'multiplayerSmoke.takeReplayTrace()'):
                compare(row,trace[row['replay'][4]-1]);compared += 1
            assert compared >= 40 and call(seek,'multiplayerSmoke.replayStatus()')[11] == 0
            assert call(seek,'multiplayerSmoke.savedFiles()') == before_files
            press('Escape',100)
            assert call(seek,'multiplayerSmoke.savedFiles()') == before_files
            report['stageSelection'] = {'passed':True,'targetFrame':target,
                                        'directCheckpoint':True,'comparedFrames':compared,
                                        'readOnly':True}
            checkpoint()
            test.close_contexts()
            if args.playback_part == 'seek':
                return

        if args.playback_part in ('all', 'browser-seek'):
            raf, before_files, press = open_viewer(test, data, args.target_stage)
            status = call(raf,'multiplayerSmoke.replayStatus()')
            assert status[7] > 0 and status[11] == 1 and status[12] == args.target_stage, status
            target = status[7]
            # Physical controls must not contaminate archived per-seat inputs.
            call(raf, "multiplayerSmoke.key('ArrowRight',true);multiplayerSmoke.key('KeyX',true)")
            browser_loop = call(raf, '(n)=>multiplayerSmoke.replayRaf(n)', target+41)
            previous = browser_loop['initial']
            first_normal = None
            for sample in browser_loop['samples']:
                prior, row = previous['observation']['replay'], sample['observation']
                current = row['replay']
                compare(row, trace[current[4]-1])
                if prior[11] and current[11]:
                    assert sample['graphics'][5] == previous['graphics'][5], 'Seek presented an intermediate framebuffer'
                    assert sample['audio'][5] == 0 and sample['audio'][9] == 1, 'Seek leaked physical audio output'
                    assert prior[4] <= current[4] <= prior[4]+4
                elif prior[11]:
                    assert current[4] == target+1 and sample['graphics'][5] == previous['graphics'][5]+1
                    assert sample['audio'][9] == 0, 'Seek did not release output suppression'
                    first_normal = current[4]
                else:
                    assert prior[4] <= current[4] <= prior[4]+1, 'Normal cadence retained seek catch-up debt'
                assert sample['audio'][8] == 0
                previous = sample
            assert first_normal == target+1 and browser_loop['final'][4] == target+41
            call(raf, "multiplayerSmoke.key('ArrowRight',false);multiplayerSmoke.key('KeyX',false)")
            assert call(raf, 'multiplayerSmoke.savedFiles()') == before_files
            press('Escape',100)
            assert call(raf, 'multiplayerSmoke.savedFiles()') == before_files
            report['browserSeek'] = {'passed':True, 'callbacks':len(browser_loop['samples']),
                                     'targetFrame':target, 'presentationFence':True,
                                     'outputSuppression':True, 'normalCadenceReset':True,
                                     'physicalInputIsolation':True, 'readOnly':True}
            checkpoint()
            test.close_contexts()
            if args.playback_part == 'browser-seek':
                return

        if args.playback_part in ('all', 'escape-seek'):
            escape, before_files, _ = open_viewer(test, data, args.target_stage)
            status = call(escape,'multiplayerSmoke.replayStatus()')
            assert status[7] > 0 and status[11] == 1 and status[12] == args.target_stage, status
            target = status[7]
            cancelled = call(escape, '(n)=>multiplayerSmoke.replayRaf(n,2)', target+1)
            assert cancelled['escaped'] and cancelled['final'][8] == 0
            assert cancelled['life'][1] == 4 and cancelled['life'][2] == 4
            assert call(escape, 'multiplayerSmoke.savedFiles()') == before_files
            report['escapeDuringSeek'] = {'passed':True, 'nativeTitleReturn':True, 'readOnly':True}
            checkpoint()
            test.close_contexts()


checkpoint()
with fixture(args.url, report, args.output) as test:
    if args.recorded_run:
        source = json.loads(args.recorded_run.read_text(encoding='utf-8'))
        assert source.get('passed') and source.get('recordingComplete'), 'Input is not a completed recording report'
        assert (source['case'], source['players'], source['targetStage']) == (args.case, args.players, args.target_stage)
        data_bytes = Path(source['replayFile']).read_bytes()
        trace_bytes = Path(source['traceFile']).read_bytes()
        assert hashlib.sha256(data_bytes).hexdigest() == source['fileSha256'], 'Recording file changed'
        assert hashlib.sha256(trace_bytes).hexdigest() == source['traceSha256'], 'Recording trace changed'
        data, trace = list(data_bytes), json.loads(trace_bytes)
        assert len(trace) == source['recordedFrames'] == int.from_bytes(data_bytes[32:36], 'little')
        for key in ('replayFile', 'fileSha256', 'traceFile', 'traceSha256', 'recordedFrames',
                    'chapters', 'recordingComplete', 'recordingIdentity'):
            report[key] = source[key]
        report['recordedRun'] = str(args.recorded_run.resolve())
    else:
        data, trace = record_run(test)
    if not args.record_only:
        playback_run(test, data, trace)
print(json.dumps({'passed':report['passed'],'report':args.output}),flush=True)
