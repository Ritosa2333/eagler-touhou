"""Offline callback work-summary tests. Only two function ASTs are executed.

The fixture rows describe independent callback events, including a correction
retreat and a lower anchor with deliberately large work. No Playwright import,
argument parser, network connection, browser or checker top-level code executes.
"""
import ast
import copy
import hashlib
import json
import math
from pathlib import Path
import re
import sys

PHASES = [
    'forwardCapture', 'historyCapture', 'forwardUpdate', 'historyUpdate',
    'forwardSemanticDraw', 'historySemanticDraw', 'restore', 'confirmedCommit',
]
WORK = [
    'schema', 'enabled', 'valid', 'sequence', 'epoch', 'owner', 'generation',
    'frameBefore', 'frameAfter', 'exitReason', 'nativeInputAndStepMs', 'residualMs',
    'forwardElidedCalls', 'historyElidedCalls', 'diagnosticFlags', 'reserved',
    'forwardCaptureCalls', 'forwardCaptureMs', 'historyCaptureCalls', 'historyCaptureMs',
    'forwardUpdateCalls', 'forwardUpdateMs', 'historyUpdateCalls', 'historyUpdateMs',
    'forwardSemanticDrawCalls', 'forwardSemanticDrawMs',
    'historySemanticDrawCalls', 'historySemanticDrawMs',
    'restoreCalls', 'restoreMs', 'confirmedCommitCalls', 'confirmedCommitMs',
]
BASE = [
    'callbackEndMs', 'nativeCallbackMs', 'nativeLastSimulatedFrame', 'confirmedThrough',
    'heapBytes', 'observedFrameHighWater', 'nativeNextFrame', 'rollbackRequestFrame',
    'nativeRecoveryYields', 'nativeCorrecting', 'nativeCorrectionEnd',
    'nativePredictionFrontier', 'nativeMissingInputs', 'nativeIntervalScaleMilli',
    'nativeHighRefresh', 'nativeInterpolate', 'nativeAlpha', 'nativePresented',
    'nativePresentationPrimed',
]
OLD = [
    'schema', 'sequence', 'offered', 'completed', 'networkPrepareMs',
    'nativeInputAndStepMs', 'flushPresentationMs', 'audioPumpMs', 'nativeTotalMs',
    'maxSingleApplicationStepMs', 'stepCalls', 'spectatorStepCalls', 'loadingMs',
    'replaySeekBatchMs', 'exitReason', 'finishResult', 'finishCalled',
    'cumulativeRollbacks', 'cumulativeResimulatedFrames',
]
OLD_TAIL = [
    'presentationGateMs', 'scheduledInputGapScale', 'presentationScheduleAlpha',
    'authoredBoundaryFlags', 'tailPresentationDecision', 'tailPresentationCommitted',
]
INVALID_FRAME = 0xffffffff
MAX_EXACT = 2**53-1
APP = 0x123400
GROUPS = []
ASSERTIONS = 0
DISTRIBUTION = None
SUMMARIZE = None
SOURCE_IDENTITY = {}
EXTRACTED_NAMES = []


def equal(actual, expected, message=''):
    global ASSERTIONS
    ASSERTIONS += 1
    if actual != expected:
        raise AssertionError(message or f'Expected {expected!r}, got {actual!r}')


def check(condition, message='Condition failed'):
    global ASSERTIONS
    ASSERTIONS += 1
    if not condition:
        raise AssertionError(message)


def close(actual, expected, tolerance=1.e-10):
    global ASSERTIONS
    ASSERTIONS += 1
    if not math.isclose(actual, expected, rel_tol=tolerance, abs_tol=tolerance):
        raise AssertionError(f'Expected {expected!r}, got {actual!r}')


def rejects(call, pattern=None, types=(AssertionError,)):
    global ASSERTIONS
    ASSERTIONS += 1
    try:
        call()
    except types as error:
        if pattern and re.search(pattern, str(error)) is None:
            raise AssertionError(f'Wrong rejection: {str(error)[:400]}') from error
        return
    raise AssertionError('Expected invalid evidence to be rejected')


def test(name):
    def register(function):
        GROUPS.append((name, function))
        return function
    return register


def load_pure_functions():
    global EXTRACTED_NAMES
    path = Path(__file__).with_name('check-performance.py')
    raw = path.read_bytes()
    tree = ast.parse(raw.decode('utf-8'), filename=str(path))
    names = {'distribution', 'summarize_callback_work'}
    definitions = [node for node in tree.body
                   if isinstance(node, ast.FunctionDef) and node.name in names]
    EXTRACTED_NAMES = sorted(node.name for node in definitions)
    if EXTRACTED_NAMES != sorted(names):
        raise AssertionError('Expected exactly the two standalone pure functions')
    if any(node.decorator_list or node.args.defaults or node.args.kw_defaults
           for node in definitions):
        raise AssertionError('Unexpected definition-time code in extracted functions')
    # No module imports, argparse, Playwright setup or other top-level nodes.
    namespace = {'math': math}
    module = ast.Module(body=definitions, type_ignores=[])
    exec(compile(module, str(path), 'exec'), namespace)
    SOURCE_IDENTITY[path.name] = {
        'bytes': len(raw), 'sha256': hashlib.sha256(raw).hexdigest(),
        'extractedFunctions': EXTRACTED_NAMES,
    }
    return namespace['distribution'], namespace['summarize_callback_work']


def make_case(specifications, version=2):
    old_schema = OLD + (OLD_TAIL if version == 2 else [])
    fields = BASE + ['callbackTiming.'+name for name in old_schema]
    fields += ['callbackWorkTiming.'+name for name in WORK]
    data = {
        'callbackTimingEnabled': True, 'callbackWorkTimingEnabled': True,
        'callbackWorkTimingSchema': list(WORK), 'callbackTimingSchema': old_schema,
        'callbackWorkTimingOwner': APP, 'callbackWorkTimingDisableResult': 0,
        'rowSchema': fields,
    }
    anchors = []
    previous_frame = specifications[0].get('frameBefore', specifications[0].get('frame', 100))
    high = previous_frame
    for index, specification in enumerate(specifications):
        frame = specification.get('frame', previous_frame)
        high = max(high, frame)
        completed = specification.get('completed', 0)
        native = specification.get('native', 2.0)
        exit_reason = specification.get('exitReason', 0)
        sequence = specification.get('sequence', 100+index)
        values = {name: 0 for name in fields}
        values.update(dict(zip(BASE, [
            index*10.0, native+2.0, frame, frame, 65536, high, frame+1, -1,
            0, specification.get('correcting', 0), -1, frame, 0, 1000, 1, 1, .5, 1, 1,
        ])))
        parent = {
            'schema': version, 'sequence': sequence, 'offered': max(1, completed),
            'completed': completed, 'nativeInputAndStepMs': native,
            'nativeTotalMs': native+2.0, 'exitReason': exit_reason,
            'cumulativeRollbacks': specification.get('rollbacks', 10),
            'cumulativeResimulatedFrames': specification.get('resimulated', 100),
        }
        for name, value in parent.items():
            values['callbackTiming.'+name] = value
        work = {
            'schema': 1, 'enabled': 1, 'valid': 1, 'sequence': sequence, 'epoch': 7,
            'owner': APP, 'generation': 0,
            'frameBefore': specification.get('frameBefore', previous_frame) & INVALID_FRAME,
            'frameAfter': frame & INVALID_FRAME, 'exitReason': exit_reason,
            'nativeInputAndStepMs': native,
            'forwardElidedCalls': specification.get('forwardElided', 0),
            'historyElidedCalls': specification.get('historyElided', 0),
        }
        elapsed = 0.0
        for phase, (calls, milliseconds) in specification.get('phases', {}).items():
            work[phase+'Calls'] = calls
            work[phase+'Ms'] = milliseconds
            elapsed += milliseconds
        work['residualMs'] = specification.get('residual', native-elapsed)
        work.update(specification.get('work', {}))
        for name, value in work.items():
            values['callbackWorkTiming.'+name] = value
        anchors.append([values[name] for name in fields])
        previous_frame = frame
    return data, anchors, anchors[1:]


def fixture(version=2):
    return make_case([
        # Lower anchor is validated, but its 1000 ms never enters measured totals.
        {'frame': 100, 'native': 1000, 'phases': {'forwardUpdate': (1, 1000)}},
        {'frame': 101, 'completed': 1, 'native': 6, 'forwardElided': 3,
         'phases': {'forwardCapture': (1, 1), 'forwardUpdate': (1, 2),
                    'forwardSemanticDraw': (1, 1), 'confirmedCommit': (1, 1)}},
        {'frame': 98, 'correcting': 1, 'rollbacks': 11, 'resimulated': 102,
         'native': 7, 'historyElided': 2,
         'phases': {'historyCapture': (2, .5), 'historyUpdate': (2, 3),
                    'historySemanticDraw': (2, 1), 'restore': (1, 1),
                    'confirmedCommit': (1, .5)}},
        {'frame': 102, 'completed': 1, 'rollbacks': 11, 'resimulated': 105,
         'native': 10, 'forwardElided': 1, 'historyElided': 4,
         'phases': {'forwardCapture': (1, 1), 'historyCapture': (3, 1),
                    'forwardUpdate': (1, 2), 'historyUpdate': (3, 2),
                    'forwardSemanticDraw': (1, 1), 'historySemanticDraw': (3, 1),
                    'restore': (1, 1), 'confirmedCommit': (1, .5)}},
        {'frame': 102, 'rollbacks': 11, 'resimulated': 105, 'native': 2},
        {'frame': 102, 'rollbacks': 12, 'resimulated': 107, 'native': 3,
         'historyElided': 1,
         'phases': {'historyCapture': (2, .25), 'historyUpdate': (2, .75),
                    'historySemanticDraw': (2, .5), 'restore': (1, .5),
                    'confirmedCommit': (1, .5)}},
    ], version)


def one_callback(specification=None, before=None):
    current = {'completed': 1, **(specification or {})}
    current.setdefault('frame', 101 if current['completed'] else 100)
    return make_case([{'frame': 100, **(before or {})}, current])


def column(data, name, prefix='callbackWorkTiming.'):
    return data['rowSchema'].index(prefix+name)


def mutate(case, name, value, row=1, prefix='callbackWorkTiming.'):
    data, anchors, _ = case
    anchors[row][column(data, name, prefix)] = value
    return case


def analyze(case):
    return SUMMARIZE(*case)


def reject_case(case, pattern=None, types=(AssertionError,)):
    # Even rejected raw evidence must remain unmodified (including NaN/-0 flags).
    before = json.dumps(case, allow_nan=True, separators=(',', ':'))
    rejects(lambda: analyze(case), pattern, types)
    equal(json.dumps(case, allow_nan=True, separators=(',', ':')), before)


@test('AST loader exposes only the two pure functions with bound source identity')
def loader_contract():
    equal(EXTRACTED_NAMES, ['distribution', 'summarize_callback_work'])
    check(callable(DISTRIBUTION) and callable(SUMMARIZE))
    equal(len(WORK), 32)
    equal(len(set(WORK)), 32)
    check(SOURCE_IDENTITY['check-performance.py']['bytes'] > 0)
    equal(len(SOURCE_IDENTITY['check-performance.py']['sha256']), 64)


@test('distribution keeps strict thresholds and raw negative residual values')
def distribution_boundaries():
    equal(DISTRIBUTION([]), None)
    values = [100.1, 0, 25, 25.1, 50, 50.1, 100]
    original = values[:]
    result = DISTRIBUTION(values)
    equal(result['count'], 7)
    equal(result['p50'], 50)
    equal(result['p95'], 100.1)
    equal(result['p99'], 100.1)
    equal((result['over25ms'], result['over50ms'], result['over100ms']), (5, 3, 1))
    equal(values, original)
    equal(DISTRIBUTION([-0.0000005])['max'], -0.0000005)


@test('contiguous callbacks exclude lower anchor and retain all five work classes')
def independent_totals():
    case = fixture()
    before = copy.deepcopy(case)
    result = analyze(case)
    equal(case, before)
    equal(result['schema'], WORK)
    total = result['all']
    equal(total['callbacks'], 5)
    close(total['nativeInputAndStepTotalMs'], 28)
    close(total['residualTotalMs'], 5)
    equal(total['forwardElidedCalls'], 4)
    equal(total['historyElidedCalls'], 7)
    equal(total['nativeInputAndStepCallbackMs']['max'], 10)
    equal(total['nativeInputAndStepCallbackMs']['p50'], 6)
    expected = {
        'forwardCapture': (2, 2), 'historyCapture': (7, 1.75),
        'forwardUpdate': (2, 4), 'historyUpdate': (7, 5.75),
        'forwardSemanticDraw': (2, 2), 'historySemanticDraw': (7, 2.5),
        'restore': (3, 2.5), 'confirmedCommit': (4, 2.5),
    }
    for phase, (calls, elapsed) in expected.items():
        detail = total['phases'][phase]
        equal(detail['calls'], calls)
        close(detail['totalMs'], elapsed)
        close(detail['meanMsPerCall'], elapsed/calls)
        equal(detail['callbackMs']['count'], 5)
    close(sum(detail['totalMs'] for detail in total['phases'].values()), 23)
    expected_groups = {
        'ordinary-forward': 6, 'partial-correction': 7,
        'complete-recovery-plus-forward': 10, 'no-forward-no-correction': 2,
        'complete-recovery-no-forward': 3,
    }
    equal(set(result['groups']), set(expected_groups))
    for kind, elapsed in expected_groups.items():
        equal(result['groups'][kind]['callbacks'], 1)
        close(result['groups'][kind]['nativeInputAndStepTotalMs'], elapsed)
    close(sum(group['nativeInputAndStepTotalMs'] for group in result['groups'].values()), 28)
    check('wall-time' in result['scope'])
    check('No journal-only CPU attribution' in result['scope'])


@test('zero-work phases retain zero callback samples and undefined per-call means')
def zero_work():
    result = analyze(one_callback({'completed': 0, 'native': 0}))
    total = result['all']
    equal(total['callbacks'], 1)
    equal(total['nativeInputAndStepTotalMs'], 0)
    equal(total['residualTotalMs'], 0)
    for detail in total['phases'].values():
        equal(detail['calls'], 0)
        equal(detail['totalMs'], 0)
        equal(detail['meanMsPerCall'], None)
        equal(detail['callbackMs']['count'], 1)
        equal(detail['callbackMs']['max'], 0)
    equal(set(result['groups']), {'no-forward-no-correction'})


@test('old timing schema one and two produce the same new-work summary')
def old_layouts():
    equal(analyze(fixture(1)), analyze(fixture(2)))
    case = fixture()
    data, anchors, rows = case
    # Move only the new block: lookup must use names, never assumed old length.
    order = list(range(len(data['rowSchema'])-32)) + list(range(len(data['rowSchema'])-1, len(data['rowSchema'])-33, -1))
    data['rowSchema'] = [data['rowSchema'][index] for index in order]
    for index, row in enumerate(anchors):
        anchors[index] = [row[position] for position in order]
    equal(analyze((data, anchors, anchors[1:])), analyze(fixture()))


@test('callback sequence owner exit and parent timing mismatch reject')
def callback_identity():
    for field, value, pattern in [
        ('sequence', 500, 'another callback'),
        ('owner', 0, 'app identity'),
        ('owner', APP+8, 'app identity'),
        ('exitReason', 3, 'exit mismatch'),
        ('nativeInputAndStepMs', 6.25, 'input interval mismatch'),
        ('frameAfter', 102, 'frame mismatch'),
    ]:
        reject_case(mutate(fixture(), field, value), pattern)
    case = fixture()
    case[0]['callbackWorkTimingOwner'] = APP+8
    reject_case(case, 'app identity')


@test('epoch or generation crossing anywhere in the anchor interval rejects')
def interval_identity():
    for field in ['epoch', 'generation']:
        for row in [0, 1, 5]:
            case = fixture()
            value = 8 if field == 'epoch' else 1
            reject_case(mutate(case, field, value, row), 'crossed loop or generation')
    case = fixture()
    for row in case[1]:
        row[column(case[0], 'generation')] = INVALID_FRAME
    equal(analyze(case)['all']['callbacks'], 5)


@test('unsigned INVALID_FRAME maps signed native minus one without inventing progress')
def frame_domain():
    case = make_case([
        {'frame': -1, 'frameBefore': -1},
        {'frame': -1, 'completed': 0},
        {'frame': 0, 'completed': 1},
    ])
    result = analyze(case)
    equal(result['all']['callbacks'], 2)
    equal(set(result['groups']), {'no-forward-no-correction', 'ordinary-forward'})
    equal(case[1][1][column(case[0], 'frameBefore')], INVALID_FRAME)
    equal(case[1][1][column(case[0], 'frameAfter')], INVALID_FRAME)
    equal(case[1][2][column(case[0], 'frameBefore')], INVALID_FRAME)
    for field in ['generation', 'frameBefore', 'frameAfter']:
        reject_case(mutate(fixture(), field, 2**32))
        reject_case(mutate(fixture(), field, -1))


@test('each diagnostic field rejects nonfinite nonnumeric and JSON booleans')
def numeric_integrity():
    for field in WORK:
        for value in [float('nan'), float('inf'), -float('inf'), None, '1', True, False]:
            reject_case(mutate(fixture(), field, value), 'Non-finite or non-numeric work timing')


@test('identity and count fields reject fractional negative and inexact integers')
def integer_integrity():
    for field in WORK:
        if field.endswith('Ms'):
            continue
        for value in [.5, -1, 2**53]:
            reject_case(mutate(fixture(), field, value), 'Inexact work timing identity/count')
    case = one_callback({'sequence': MAX_EXACT,
                         'phases': {'forwardUpdate': (MAX_EXACT, .25)}},
                        {'sequence': MAX_EXACT-1})
    equal(analyze(case)['all']['phases']['forwardUpdate']['calls'], MAX_EXACT)


@test('invalid publication diagnostic flags reserved bytes and lower anchor reject')
def publication_integrity():
    for field, value, pattern in [
        ('schema', 2, 'Invalid work timing publication'),
        ('enabled', 0, 'Invalid work timing publication'),
        ('valid', 0, 'Invalid work timing publication'),
        ('diagnosticFlags', 512, 'flags or reserved'),
        ('reserved', 1, 'flags or reserved'),
    ]:
        reject_case(mutate(fixture(), field, value), pattern)
    reject_case(mutate(fixture(), 'diagnosticFlags', 32, row=0), 'flags or reserved')
    # Corrupt both native flags and residual without replacing them with zeroes.
    case = mutate(fixture(), 'valid', 0)
    mutate(case, 'diagnosticFlags', 544)
    mutate(case, 'residualMs', -3.125)
    reject_case(case, 'Invalid work timing publication')


@test('tiny legal negative residual remains negative in totals distributions and input')
def tolerance_is_not_clamping():
    case = one_callback({
        'native': 1, 'phases': {'forwardUpdate': (1, 1.0000005)},
        'residual': -.0000005,
    })
    before = copy.deepcopy(case)
    result = analyze(case)
    equal(case, before)
    equal(result['all']['residualTotalMs'], -.0000005)
    equal(result['all']['residualCallbackMs']['max'], -.0000005)
    equal(result['groups']['ordinary-forward']['residualTotalMs'], -.0000005)
    check(result['all']['residualTotalMs'] < 0)
    # Exact tolerance boundary is accepted; the native rule is strictly below it.
    edge = one_callback({'native': 1, 'phases': {'forwardUpdate': (1, 1.000001)},
                         'residual': -1.e-6})
    equal(analyze(edge)['all']['residualTotalMs'], -1.e-6)


@test('real negative input phase time or excessive residual overlap rejects')
def negative_intervals():
    reject_case(one_callback({
        'native': 1, 'phases': {'forwardUpdate': (1, 1.001)}, 'residual': -.001,
    }), 'Overlapping work subphases')
    reject_case(one_callback({'native': -.0000005, 'residual': -.0000005}))
    for phase in PHASES:
        reject_case(one_callback({'native': 1, 'phases': {phase: (1, -.25)}}),
                    'Negative work subphase')


@test('subphase sums must match the original parent interval without compensating edits')
def sum_integrity():
    reject_case(mutate(fixture(), 'residualMs', 1.1), 'accounting mismatch')
    reject_case(mutate(fixture(), 'forwardUpdateMs', 2.1), 'accounting mismatch')
    reject_case(mutate(fixture(), 'nativeInputAndStepMs', 6.1), 'input interval mismatch')
    case = fixture()
    mutate(case, 'nativeInputAndStepMs', 6.1, prefix='callbackTiming.')
    reject_case(case, 'input interval mismatch')


@test('zero calls cannot own positive duration but measured zero-length calls are valid')
def call_time_integrity():
    for phase in PHASES:
        reject_case(one_callback({'native': 1, 'phases': {phase: (0, .25)}}))
        result = analyze(one_callback({'native': 1, 'phases': {phase: (1, 0)}}))
        detail = result['all']['phases'][phase]
        equal(detail['calls'], 1)
        equal(detail['totalMs'], 0)
        equal(detail['meanMsPerCall'], 0)


@test('classification uses counter deltas and previous correcting at closure boundaries')
def classification_boundaries():
    examples = [
        ({'completed': 2}, {}, 'ordinary-forward'),
        ({'completed': 0}, {}, 'no-forward-no-correction'),
        ({'completed': 0, 'correcting': 1}, {}, 'partial-correction'),
        ({'completed': 1, 'correcting': 1, 'rollbacks': 11}, {}, 'partial-correction'),
        ({'completed': 1, 'rollbacks': 11}, {}, 'complete-recovery-plus-forward'),
        ({'completed': 0, 'resimulated': 101}, {}, 'complete-recovery-no-forward'),
        ({'completed': 1}, {'correcting': 1}, 'complete-recovery-plus-forward'),
        ({'completed': 0}, {'correcting': 1}, 'complete-recovery-no-forward'),
    ]
    for current, before, expected in examples:
        result = analyze(one_callback(current, before))
        equal(set(result['groups']), {expected})
        equal(result['groups'][expected]['callbacks'], 1)
        equal(result['all']['callbacks'], 1)


@test('regressing cumulative rollback or resimulation counts reject even at lower anchor')
def classification_regression():
    for field in ['cumulativeRollbacks', 'cumulativeResimulatedFrames']:
        case = one_callback()
        first = case[1][0][column(case[0], field, 'callbackTiming.')]
        reject_case(mutate(case, field, first-1, prefix='callbackTiming.'),
                    'classification counters regressed')


@test('parent option schema row width and stop acknowledgement remain strict gates')
def outer_integrity():
    for value in [False, None]:
        case = fixture()
        case[0]['callbackTimingEnabled'] = value
        reject_case(case, 'require the original callback timing')
    for replacement in [WORK[:-1], WORK[::-1]]:
        case = fixture()
        case[0]['callbackWorkTimingSchema'] = replacement
        reject_case(case, 'Incomplete work timing schema')
    for extra in [False, True]:
        case = fixture()
        if extra:
            case[1][1].append(0)
        else:
            case[1][1].pop()
        reject_case(case, 'Incomplete work timing row')
    case = fixture()
    index = column(case[0], 'historySemanticDrawMs')
    case[0]['rowSchema'][index] = 'missing.required.column'
    reject_case(case, types=(ValueError,))
    for value in [None, 1, -1, '0']:
        case = fixture()
        case[0]['callbackWorkTimingDisableResult'] = value
        reject_case(case, 'was not disabled')


def main():
    global DISTRIBUTION, SUMMARIZE
    try:
        DISTRIBUTION, SUMMARIZE = load_pure_functions()
        raw = Path(__file__).read_bytes()
        SOURCE_IDENTITY[Path(__file__).name] = {
            'bytes': len(raw), 'sha256': hashlib.sha256(raw).hexdigest(),
        }
    except Exception as error:
        print(json.dumps({
            'schema': 'th10-callback-work-summary-tests/1', 'passed': False,
            'groups': 0, 'assertions': ASSERTIONS, 'sourceIdentity': SOURCE_IDENTITY,
            'error': {'name': type(error).__name__, 'message': str(error)[:1000]},
        }, sort_keys=True))
        return 1
    results = []
    for name, function in GROUPS:
        before = ASSERTIONS
        try:
            function()
            results.append({'name': name, 'passed': True, 'assertions': ASSERTIONS-before})
        except Exception as error:
            results.append({
                'name': name, 'passed': False, 'assertions': ASSERTIONS-before,
                'error': {'name': type(error).__name__, 'message': str(error)[:1000]},
            })
    passed = all(result['passed'] for result in results)
    print(json.dumps({
        'schema': 'th10-callback-work-summary-tests/1', 'passed': passed,
        'groups': len(results), 'passedGroups': sum(result['passed'] for result in results),
        'assertions': ASSERTIONS, 'results': results, 'sourceIdentity': SOURCE_IDENTITY,
        'scope': 'Offline AST-extracted real summary and independent numeric fixtures. '
                 'Wall-time accounting and evidence integrity only; no browser, native '
                 'phase-clock validation, CPU attribution or performance acceptance.',
    }, sort_keys=True))
    return 0 if passed else 1


if __name__ == '__main__':
    sys.exit(main())
