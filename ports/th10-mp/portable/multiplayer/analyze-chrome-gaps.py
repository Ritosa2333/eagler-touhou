"""Offline, bounded correlation of native callback gaps with a complete trace.

No browser or profiler is started. The source report, each co-located gzip and
the existing chrome-trace.py reader are SHA-bound before analysis. A successful
analysis means the stated evidence checks completed, never smoothness or a
physical CPU/GPU/OS diagnosis. The old untraced 700 ms sample remains unexplained.
"""
import argparse
import hashlib
import importlib.util
import json
import math
from pathlib import Path, PureWindowsPath
import re
import sys

SCHEMA = 'th10-chrome-gaps/1'
TRACE_SCHEMA = 'th10-chrome-trace/1'
LIMITS = {
    'reportBytes': 64 * 1024 * 1024, 'traceBytes': 128 * 1024 * 1024,
    'traceEventsPerPass': 2000000, 'seats': 3, 'rowsPerSeat': 72000,
    'rowColumns': 96, 'transportRows': 2048, 'contextRows': 128,
    'eventsPerTarget': 1024, 'argumentBytes': 1024, 'markerBytes': 4096,
    'outputBytes': 16 * 1024 * 1024, 'errors': 32,
}
FORBIDDEN = ('cpu_profiler', 'runtime_stats_sampling', 'screenshot', 'memory-infra')
STAGES = ('before-native-start', 'native-loops-complete')
GC_NAME = re.compile(r'(^|[.:_/])gc|major.?gc|minor.?gc|garbage.?collect|scaveng|mark.?compact', re.I)


class EvidenceError(ValueError):
    pass


def need(condition, message):
    if not condition:
        raise EvidenceError(message)


def finite(value):
    return isinstance(value, (int, float)) and not isinstance(value, bool) and math.isfinite(value)


def integer(value):
    return isinstance(value, int) and not isinstance(value, bool)


def basename(value):
    need(isinstance(value, str) and value, 'Missing artifact path')
    return PureWindowsPath(value).name if '\\' in value else Path(value).name


def encoded(value):
    return json.dumps(value, ensure_ascii=False, allow_nan=False, separators=(',', ':')).encode('utf-8')


def add_error(result, stage, error):
    if len(result['errors']) < LIMITS['errors']:
        result['errors'].append({'stage': stage, 'message': str(error)[:700]})
    else:
        result['droppedErrors'] += 1


def digest_file(path, limit):
    size = path.stat().st_size
    need(0 < size <= limit, 'File size outside explicit bound: ' + str(path))
    digest, count = hashlib.sha256(), 0
    with path.open('rb') as source:
        while True:
            chunk = source.read(256 * 1024)
            if not chunk:
                break
            count += len(chunk)
            need(count <= limit, 'File grew beyond explicit byte bound')
            digest.update(chunk)
    need(count == size, 'File size changed while hashing')
    return {'path': str(path), 'bytes': count, 'sha256': digest.hexdigest()}


def load_report(path):
    with path.open('rb') as source:
        raw = source.read(LIMITS['reportBytes'] + 1)
    need(0 < len(raw) <= LIMITS['reportBytes'], 'Report exceeds explicit byte bound')
    def reject_constant(value):
        raise EvidenceError('Non-finite JSON constant: ' + value)
    value = json.loads(raw, parse_constant=reject_constant)
    need(isinstance(value, dict), 'Report root must be an object')
    return value, {'path': str(path), 'bytes': len(raw), 'sha256': hashlib.sha256(raw).hexdigest()}


def load_reader(report):
    path = Path(__file__).resolve().with_name('chrome-trace.py')
    actual = digest_file(path, 1024 * 1024)
    sources = report.get('sources')
    need(isinstance(sources, dict), 'Missing source attestation')
    expected = [sha for name, sha in sources.items() if basename(name) == 'chrome-trace.py']
    need(len(expected) == 1 and expected[0] == actual['sha256'],
         'Current chrome-trace.py differs from the source SHA recorded by this run')
    spec = importlib.util.spec_from_file_location('th10_offline_chrome_trace_reader', path)
    need(spec is not None and spec.loader is not None, 'Cannot load existing trace reader')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    need(callable(getattr(module, '_trace_events', None)), 'Existing streaming reader unavailable')
    need(module.SCHEMA == TRACE_SCHEMA, 'Unsupported trace reader schema')
    return module._trace_events, actual


def columns(schema, required):
    need(isinstance(schema, list) and 0 < len(schema) <= LIMITS['rowColumns'] and
         all(isinstance(name, str) for name in schema) and len(set(schema)) == len(schema),
         'Invalid or duplicate rowSchema columns')
    need(all(name in schema for name in required), 'Required named rowSchema columns missing: ' +
         ', '.join(name for name in required if name not in schema))
    return {name: schema.index(name) for name in required}


def select_targets(peer, context_ms):
    need(peer.get('schemaVersion') == 2 and peer.get('done') is True and peer.get('passed') is True,
         'Peer is not a completed schema-2 native run')
    need(not peer.get('sessionIdentityChanges'), 'Measured rows crossed a session identity')
    schema, rows = peer.get('rowSchema'), peer.get('rows')
    col = columns(schema, ['callbackEndMs', 'nativeCallbackMs', 'nativeLastSimulatedFrame',
                           'observedFrameHighWater'])
    need(isinstance(rows, list) and 2 <= len(rows) <= LIMITS['rowsPerSeat'], 'Invalid row count')
    high = peer.get('initialFrame')
    need(integer(high) and integer(peer.get('targetFrame')), 'Invalid frame bounds')
    frontiers, previous_end = [], None
    for index, row in enumerate(rows):
        need(isinstance(row, list) and len(row) == len(schema), 'Row width differs from rowSchema')
        end, native, frame = (row[col[name]] for name in
                              ['callbackEndMs', 'nativeCallbackMs', 'nativeLastSimulatedFrame'])
        need(finite(end) and end >= 0 and finite(native) and native >= 0 and integer(frame),
             'Invalid native row values at index ' + str(index))
        need(previous_end is None or end > previous_end, 'Callback end times are not strictly increasing')
        previous_end, high = end, max(high, frame)
        need(row[col['observedFrameHighWater']] == high, 'Observed frontier differs from raw history')
        frontiers.append(high)
    window = (peer.get('summary') or {}).get('window')
    need(isinstance(window, dict), 'Missing recorded measured window')
    first, last = window.get('startRowExclusive'), window.get('endRowInclusive')
    need(integer(first) and integer(last) and 0 <= first < last < len(rows), 'Invalid measured row indices')
    lower, upper = peer['initialFrame'] + 120, peer['targetFrame'] - 12
    need(window.get('requestedLowerFrame') == lower and window.get('requestedUpperFrame') == upper,
         'Recorded measurement policy changed')
    expected_first = next((i for i, frame in enumerate(frontiers) if frame >= lower), None)
    expected_last = next((i for i, frame in enumerate(frontiers)
                          if expected_first is not None and i > expected_first and frame >= upper), None)
    need((first, last) == (expected_first, expected_last), 'Window is not the first frontier crossings')
    end_col, native_col = col['callbackEndMs'], col['nativeCallbackMs']
    need(window.get('startMs') == rows[first][end_col] and window.get('endMs') == rows[last][end_col] and
         window.get('callbacks') == last - first, 'Window timestamps/count differ from raw rows')
    selected = [
        ('maximum-end-gap', max(range(first + 1, last + 1),
                               key=lambda i: rows[i][end_col] - rows[i - 1][end_col])),
        ('maximum-native', max(range(first + 1, last + 1), key=lambda i: rows[i][native_col])),
    ]
    targets = []
    for kind, index in selected:
        previous, current = rows[index - 1], rows[index]
        begin, end, native = previous[end_col], current[end_col], current[native_col]
        native_begin = end - native
        target = {
            'kind': kind, 'rowIndex': index, 'previousRowIndex': index - 1,
            'previousEndMs': begin, 'currentEndMs': end, 'nativeCallbackMs': native,
            'endGapMs': end - begin, 'estimatedNativeStartMs': native_begin,
            'outsideGapSignedMs': native_begin - begin,
            'boundaryScope': 'outside = previous callbackEndMs to current callbackEndMs minus '
                             'current nativeCallbackMs. The end gap includes the current native call. '
                             'This is a row-derived entry proxy, not a separately recorded native-entry '
                             'timestamp; JS finish-hook sampling can contribute residual timing.',
            'contextElapsedMs': [min(begin, native_begin) - context_ms, end + context_ms],
            'previousRawRow': previous, 'currentRawRow': current,
            'events': [], 'selectedEventCount': 0, 'eventOverflow': 0,
            'errors': [], 'droppedErrors': 0,
        }
        if native_begin < begin:
            add_error(target, 'outside-boundary', 'Negative outside gap retained; no clamped idle interval')
        targets.append(target)
    return col, window, targets


def nearby(rows, time_value, start, end):
    found = [i for i, row in enumerate(rows) if start <= time_value(row) <= end]
    before = next((i for i in range(len(rows) - 1, -1, -1) if time_value(rows[i]) < start), None)
    after = next((i for i, row in enumerate(rows) if time_value(row) > end), None)
    indices = sorted(set(found + ([] if before is None else [before]) + ([] if after is None else [after])))
    return {
        'selectionScope': 'Every sample inside context plus the nearest preceding/following sample',
        'matchingRows': len(indices), 'overflow': max(0, len(indices) - LIMITS['contextRows']),
        'rows': [{'index': i, 'values': rows[i]} for i in indices[:LIMITS['contextRows']]],
    }


def add_context(peer, col, targets, clock):
    transport = peer.get('transportTrace')
    transport_col, transport_rows = None, []
    if transport and transport.get('enabled'):
        need(transport.get('schema') == 'th10-transport-trace/1' and transport.get('schemaVersion') == 1,
             'Unsupported transport metadata schema')
        transport_col = columns(transport.get('rowSchema'), ['atMs', 'samplePerformanceMs'])
        transport_rows = transport.get('rows')
        need(isinstance(transport_rows, list) and len(transport_rows) <= LIMITS['transportRows'],
             'Transport row bound exceeded')
        previous = None
        for row in transport_rows:
            need(isinstance(row, list) and len(row) == len(transport['rowSchema']), 'Transport row width')
            value = row[transport_col['samplePerformanceMs']]
            need(finite(value) and (previous is None or value >= previous), 'Invalid transport sample clock')
            previous = value
        source_clock = transport.get('clock') or {}
        need(source_clock.get('performanceTimeOriginEpochMs') == clock['performanceTimeOriginMs'] and
             source_clock.get('startedAtPerformanceMs') == clock['startedAtPerformanceMs'],
             'Transport and trace markers do not share one page realm')
    for target in targets:
        lo, hi = target['contextElapsedMs']
        target['rowContext'] = nearby(peer['rows'], lambda row: row[col['callbackEndMs']], lo, hi)
        if target['rowContext']['overflow']:
            add_error(target, 'rows', 'Context row capacity exceeded; no downsampled PASS')
        if transport_col is None:
            target['transportContext'] = {'available': False, 'scope': 'Transport trace was not enabled'}
            continue
        context = nearby(transport_rows,
                         lambda row: row[transport_col['samplePerformanceMs']] - clock['startedAtPerformanceMs'],
                         lo, hi)
        context.update(available=True, selectionClock='samplePerformanceMs minus this page driver start',
                       sourceValid=transport.get('valid'), sourceComplete=transport.get('complete'),
                       sourceDropped=transport.get('dropped'), sourceReadErrors=transport.get('readErrors'),
                       receiveClockScope='lastReceivedAtDateEpochMs is a wall-clock observation, not a '
                                         'monotonic receive timestamp; no receive time is invented.')
        target['transportContext'] = context
        if context['overflow']:
            add_error(target, 'transport', 'Context transport row capacity exceeded')
        if (transport.get('valid') is not True or transport.get('complete') is not True or
                transport.get('dropped') != 0 or transport.get('readErrors') != 0):
            add_error(target, 'transport', 'Transport evidence is incomplete or has dropped/error samples')


def categories(event):
    value = event.get('cat', '')
    return {part.strip() for part in value.split(',')} if isinstance(value, str) else set()


def checked_events(reader, path):
    for index, event in enumerate(reader(path)):
        need(index < LIMITS['traceEventsPerPass'], 'Full trace event bound exceeded')
        need(isinstance(event, dict), 'Invalid raw trace event')
        yield index, event


def raw_markers(reader, path, trace):
    supplied = trace.get('markers')
    need(isinstance(supplied, list) and len(supplied) == 2, 'Two original markers required')
    by_stage = {marker.get('stage'): marker for marker in supplied}
    need(set(by_stage) == set(STAGES), 'Original marker stages incomplete')
    labels = [by_stage[stage].get('label') for stage in STAGES]
    need(all(isinstance(label, str) and 0 < len(label) <= 240 for label in labels) and
         len(set(labels)) == 2, 'Invalid original marker labels')
    found = {label: [] for label in labels}
    count = forbidden = 0
    for _, event in checked_events(reader, path):
        count += 1
        cats = categories(event)
        if any(part in str(event.get('cat', '')) for part in FORBIDDEN):
            forbidden += 1
        name = event.get('name')
        if name in found and 'blink.user_timing' in cats:
            need(len(found[name]) < 4, 'Raw marker duplicate bound exceeded')
            need(len(encoded(event)) <= LIMITS['markerBytes'], 'Raw marker byte bound exceeded')
            found[name].append(event)
    expected = (trace.get('summary') or {}).get('counts') or {}
    need(count == expected.get('events'), 'Raw trace event count differs from recorded complete summary')
    need(forbidden == 0, 'Raw trace contains excluded profiler/screenshot/memory categories')
    need(all(len(found[label]) == 1 for label in labels), 'Raw begin/end marker missing or duplicated')
    ordered = []
    for stage in STAGES:
        original = by_stage[stage]
        event = found[original['label']][0]
        page = original.get('page') or {}
        need(original.get('cdpClockSyncAcknowledged') is True and
             finite(page.get('markStartTimeMs')) and finite(page.get('performanceTimeOriginMs')),
             'Incomplete page marker clock')
        need(integer(event.get('pid')) and integer(event.get('tid')) and finite(event.get('ts')),
             'Invalid raw marker thread/timestamp')
        ordered.append({'stage': stage, 'original': original, 'rawTraceEvent': event})
    return ordered, count


def map_clock(markers, trace, peer):
    first, last = markers
    p0, p1 = (marker['original']['page'] for marker in markers)
    e0, e1 = (marker['rawTraceEvent'] for marker in markers)
    need((e0['pid'], e0['tid']) == (e1['pid'], e1['tid']), 'Raw markers belong to different threads')
    need(p0['performanceTimeOriginMs'] == p1['performanceTimeOriginMs'], 'Page time origin changed')
    page_span = p1['markStartTimeMs'] - p0['markStartTimeMs']
    trace_span = e1['ts'] - e0['ts']
    need(page_span > 0 and trace_span > 0, 'Clock anchors are not ordered')
    renderer = (trace.get('summary') or {}).get('rendererThread') or {}
    need((renderer.get('pid'), renderer.get('tid')) == (e0['pid'], e0['tid']),
         'Raw marker thread differs from recorded renderer')
    start, elapsed = peer.get('startedAtPerformanceMs'), peer.get('elapsedMs')
    native_clock = trace.get('nativeRunClock') or {}
    need(finite(start) and finite(elapsed) and elapsed > 0 and
         native_clock.get('startedAtPerformanceMs') == start and native_clock.get('elapsedMs') == elapsed,
         'Native run clock does not match this peer')
    need(p0['markStartTimeMs'] <= start and start + elapsed <= p1['markStartTimeMs'] + .001,
         'Markers do not bracket the complete recorded native loop')
    offset0, offset1 = e0['ts'] - p0['markStartTimeMs'] * 1000, e1['ts'] - p1['markStartTimeMs'] * 1000
    clock = {
        'scope': 'Two-point affine map within this page only. Raw endpoint offsets and their drift '
                 'are retained; interpolation does not prove perfect clocks or remove timestamp quantization.',
        'performanceTimeOriginMs': p0['performanceTimeOriginMs'], 'startedAtPerformanceMs': start,
        'nativeElapsedMs': elapsed, 'renderer': {'pid': e0['pid'], 'tid': e0['tid'], 'name': renderer.get('name')},
        'pageAnchorMs': [p0['markStartTimeMs'], p1['markStartTimeMs']],
        'traceAnchorUs': [e0['ts'], e1['ts']], 'traceUsPerPageMs': trace_span / page_span,
        'offsetAtBeginUs': offset0, 'offsetAtEndUs': offset1, 'offsetDriftUs': offset1 - offset0,
        'apparentRateDriftPpm': (trace_span / page_span / 1000 - 1) * 1000000,
        'markers': markers,
    }
    return clock


def trace_time(clock, elapsed_ms):
    page = clock['startedAtPerformanceMs'] + elapsed_ms
    return clock['traceAnchorUs'][0] + (page - clock['pageAnchorMs'][0]) * clock['traceUsPerPageMs']


def event_kinds(event):
    name, cats = event.get('name', ''), categories(event)
    if not isinstance(name, str):
        return []
    kinds = []
    if 'toplevel' in cats and (name == 'RunTask' or name.endswith('::RunTask')):
        kinds.append('RunTask')
    if name in ('FireAnimationFrame', 'FunctionCall', 'RequestAnimationFrame', 'CancelAnimationFrame'):
        kinds.append(name)
    if any(cat == 'v8' or cat.startswith('v8.') or cat.startswith('disabled-by-default-v8.') for cat in cats):
        kinds.append('V8')
    if GC_NAME.search(name) and any('v8' in cat or 'timeline' in cat or 'gc' in cat.lower() for cat in cats):
        kinds.append('GC')
    return kinds


def small_args(value):
    """Retain small args (including rAF ids) without copying stacks or payloads."""
    budget, omitted = [64], [0]
    missing = object()
    priority = {'id': 0, 'requestId': 0, 'callbackId': 0, 'frame': 1, 'data': 2,
                'functionName': 3, 'lineNumber': 3, 'columnNumber': 3, 'url': 4}
    def copy(item, depth, allowance):
        budget[0] -= 1
        if budget[0] < 0 or depth > 3 or allowance < 4:
            omitted[0] += 1
            return missing
        if item is None or isinstance(item, bool) or finite(item):
            if len(encoded(item)) <= allowance:
                return item
            omitted[0] += 1
            return missing
        if isinstance(item, str):
            result = item[:256]
            while result and len(encoded(result)) > allowance:
                result = result[:len(result) // 2]
            if result != item:
                omitted[0] += 1
            return result
        if isinstance(item, list):
            result = []
            omitted[0] += max(0, len(item) - 12)
            for child in item[:12]:
                value = copy(child, depth + 1, allowance - len(encoded(result)) - 1)
                if value is not missing:
                    result.append(value)
            return result
        if isinstance(item, dict):
            entries = list(item.items())
            entries.sort(key=lambda entry: priority.get(entry[0], 5))
            omitted[0] += max(0, len(entries) - 16)
            result = {}
            for key, child in entries[:16]:
                key = str(key)
                if len(key) > 100:
                    omitted[0] += 1
                    continue
                remaining = allowance - len(encoded(result)) - len(encoded(key)) - 2
                value = copy(child, depth + 1, remaining)
                if value is not missing:
                    result[key] = value
            return result
        omitted[0] += 1
        return missing
    result = copy(value, 0, LIMITS['argumentBytes'])
    if result is missing:
        result = {'omitted': 'argument bound or unsupported type'}
    need(len(encoded(result)) <= LIMITS['argumentBytes'], 'Internal argument byte-bound error')
    return result, bool(omitted[0])


def compact_event(index, event, kinds):
    result = {'traceEventIndex': index, 'kinds': kinds}
    for key in ('pid', 'tid', 'ts', 'dur', 'tts', 'tdur'):
        if finite(event.get(key)):
            result[key] = event[key]
    for key in ('name', 'cat', 'ph', 'scope', 's', 'bp'):
        if isinstance(event.get(key), str):
            result[key] = event[key][:480]
    for key in ('id', 'id2', 'bind_id'):
        if key in event:
            result[key], truncated = small_args(event[key])
            if truncated:
                result['identifierTruncated'] = True
    if 'args' in event:
        result['args'], result['argsTruncated'] = small_args(event['args'])
    if event.get('ph') == 'X' and finite(event.get('dur')) and event['dur'] >= 0:
        result['wallDurationMs'] = event['dur'] / 1000
        if finite(event.get('tdur')) and event['tdur'] >= 0:
            result['threadDurationMs'] = event['tdur'] / 1000
            result['threadTimeScope'] = 'Actual full-span tdur; never apportioned to a clipped overlap'
        else:
            result['threadTimeScope'] = 'No usable tdur for this complete span'
    else:
        result['threadTimeScope'] = 'Not a complete X span; no CPU duration inferred'
    return result


def intersects(start, end, low, high):
    return low <= start <= high if end == start else end > low and start < high


def union_intervals(intervals, low, high):
    clipped = sorted((max(low, start), min(high, end)) for start, end in intervals
                     if end > low and start < high and end > start)
    merged = []
    for start, end in clipped:
        if merged and start <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], end)
        else:
            merged.append([start, end])
    uncovered, cursor = [], low
    for start, end in merged:
        if start > cursor:
            uncovered.append([cursor, start])
        cursor = max(cursor, end)
    if cursor < high:
        uncovered.append([cursor, high])
    return merged, uncovered


def scan_targets(reader, path, clock, targets, expected_count):
    renderer = clock['renderer']
    for target in targets:
        requested = [trace_time(clock, value) for value in target['contextElapsedMs']]
        target['requestedContextTraceUs'] = requested
        target['contextTraceUs'] = [max(requested[0], clock['traceAnchorUs'][0]),
                                    min(requested[1], clock['traceAnchorUs'][1])]
        target['contextClippedToMarkers'] = target['contextTraceUs'] != requested
        target['outsideTraceUs'] = [trace_time(clock, target['previousEndMs']),
                                    trace_time(clock, target['estimatedNativeStartMs'])]
        target['nativeTraceUs'] = [target['outsideTraceUs'][1], trace_time(clock, target['currentEndMs'])]
        need(clock['traceAnchorUs'][0] <= target['outsideTraceUs'][0] <= target['nativeTraceUs'][1] <=
             clock['traceAnchorUs'][1], 'Selected target lies outside recorded clock markers')
    count = 0
    for index, event in checked_events(reader, path):
        count += 1
        if event.get('pid') != renderer['pid'] or event.get('tid') != renderer['tid']:
            continue
        kinds, start = event_kinds(event), event.get('ts')
        if not kinds or not finite(start):
            continue
        complete = event.get('ph') == 'X' and finite(event.get('dur')) and event['dur'] >= 0
        end = start + event['dur'] if complete else start
        for target in targets:
            if not intersects(start, end, *target['contextTraceUs']):
                continue
            target['selectedEventCount'] += 1
            if len(target['events']) >= LIMITS['eventsPerTarget']:
                target['eventOverflow'] += 1
                continue
            out = compact_event(index, event, kinds)
            out['overlapsOutsideGap'] = intersects(start, end, *target['outsideTraceUs'])
            out['overlapsEstimatedNative'] = intersects(start, end, *target['nativeTraceUs'])
            target['events'].append(out)
    need(count == expected_count, 'Trace changed between bounded streaming passes')
    for target in targets:
        target['events'].sort(key=lambda event: (event.get('ts', 0), event['traceEventIndex']))
        if target['eventOverflow']:
            add_error(target, 'events', 'Selected event capacity exceeded; truncated evidence is FAIL')
        run_tasks = [event for event in target['events'] if 'RunTask' in event['kinds']]
        complete = [event for event in run_tasks if event.get('ph') == 'X' and
                    finite(event.get('dur')) and event['dur'] >= 0]
        other = [event for event in run_tasks if event not in complete and event['overlapsOutsideGap']]
        if other:
            add_error(target, 'RunTask-union', 'Non-complete RunTask records intersect gap; no B/E pairing is invented')
        low, high = target['outsideTraceUs']
        if high >= low:
            merged, uncovered = union_intervals([(event['ts'], event['ts'] + event['dur'])
                                                  for event in complete], low, high)
            target['outsideRunTaskWall'] = {
                'scope': 'Union of this renderer thread complete toplevel RunTask wall intervals, '
                         'clipped to the row-derived outside gap. Nested/overlapping tasks count once. '
                         'Uncovered intervals mean no captured RunTask coverage, not idle/GPU/OS proof.',
                'available': not other and not target['eventOverflow'],
                'outsideTraceWallMs': (high - low) / 1000,
                'runTaskWallUnionMs': sum(end - begin for begin, end in merged) / 1000,
                'uncoveredByRunTaskWallMs': sum(end - begin for begin, end in uncovered) / 1000,
                'coveredTraceUs': merged, 'uncoveredTraceUs': uncovered,
                'completeRunTasksIntersectingGap': sum(event['overlapsOutsideGap'] for event in complete),
                'fullSpanTdurPresent': sum(event['overlapsOutsideGap'] and 'threadDurationMs' in event
                                          for event in complete),
                'cpuTotalMs': None,
                'cpuScope': 'No clipped/summed CPU estimate. Individual full-span tdur is preserved in events.',
            }
        else:
            target['outsideRunTaskWall'] = {'available': False, 'reason': 'negative-outside-gap'}
        target['eventCountsByKind'] = {}
        for event in target['events']:
            for kind in event['kinds']:
                target['eventCountsByKind'][kind] = target['eventCountsByKind'].get(kind, 0) + 1
        target['gcScope'] = 'GC is a bounded name/category classification of actual recorded events; '\
                            'zero matches cannot exclude GC. V8/GC asynchronous or incomplete records '\
                            'are retained but are not converted to CPU spans.'
        target['passed'] = not target['errors'] and not target['droppedErrors']


def trace_artifact(report_path, trace):
    need(trace.get('schema') == TRACE_SCHEMA and trace.get('passed') is True and
         trace.get('dataLossOccurred') is False, 'Trace is not a complete no-loss PASS')
    for key in ('startAcknowledged', 'endAcknowledged', 'streamComplete', 'summaryComplete',
                'streamClosed', 'sessionDetached'):
        need(trace.get(key) is True, 'Incomplete trace control/cleanup field: ' + key)
    need(not trace.get('errors') and not trace.get('droppedErrors'), 'Trace recorded errors')
    need((trace.get('summary') or {}).get('markersComplete') is True, 'Recorded markers incomplete')
    need(trace.get('drainStage') == 'after-final-state-audio-fence', 'Trace drain did not follow final fence')
    parameters = trace.get('parameters') or {}
    config = parameters.get('traceConfig') or {}
    need(parameters.get('transferMode') == 'ReturnAsStream' and parameters.get('streamFormat') == 'json' and
         parameters.get('streamCompression') == 'gzip' and config.get('recordMode') == 'recordUntilFull' and
         config.get('traceBufferSizeInKb') == 32768 and config.get('enableSampling') is False and
         config.get('enableSystrace') is False, 'Trace collection policy differs from the bounded no-sampling contract')
    artifact = trace.get('artifact') or {}
    need(artifact.get('validated') is True and artifact.get('format') == 'json' and
         artifact.get('compression') == 'gzip', 'Invalid trace artifact schema')
    name = basename(artifact.get('path'))
    need(name.endswith('.json.gz'), 'Trace artifact is not a JSON gzip')
    path = report_path.parent / name
    actual = digest_file(path, LIMITS['traceBytes'])
    need(actual['bytes'] == artifact.get('bytes') and actual['sha256'] == artifact.get('sha256'),
         'Trace gzip bytes/SHA differ from this report')
    actual['recordedPath'] = artifact['path']
    actual['resolutionScope'] = 'Co-located basename only; content must match recorded size and SHA-256'
    return path, actual


def analyze(report_path, context_ms):
    result = {
        'schema': SCHEMA, 'passed': False, 'scope': 'Offline diagnostic correlation, not a performance '
        'acceptance or a physical CPU/GPU/OS root-cause attribution. No relation to the old untraced '
        '700 ms sample is inferred. A new run that lacks that stall cannot explain the old run.',
        'limits': LIMITS, 'contextMs': context_ms, 'errors': [], 'droppedErrors': 0, 'peers': [],
    }
    try:
        report, result['sourceReport'] = load_report(report_path)
        result['analyzerSource'] = digest_file(Path(__file__).resolve(), 1024 * 1024)
        need(report.get('schemaVersion') == 2 and report.get('kind') == 'native-raf-real-rtc',
             'Unsupported source report schema/kind')
        need(report.get('passed') is True and not report.get('errors'), 'Source correctness run did not pass')
        group = report.get('chromeTrace') or {}
        need(group.get('schema') == TRACE_SCHEMA and group.get('enabled') is True and
             group.get('complete') is True and group.get('nativeLoopsComplete') is True and
             group.get('finalStateAudioFencePassed') is True and
             group.get('independentBrowserProcesses') is True, 'Complete symmetric trace/final fence required')
        peers, traces, identities = report.get('peers'), group.get('peers'), report.get('identities')
        need(isinstance(peers, list) and 2 <= len(peers) <= LIMITS['seats'] and
             isinstance(traces, list) and len(traces) == len(peers) and
             group.get('requestedSeats') == len(peers), 'Seat count mismatch')
        need(isinstance(identities, list) and len(identities) == len(peers), 'Missing seat runtime identities')
        reader, result['traceReaderSource'] = load_reader(report)
        need(all(isinstance(trace, dict) and integer(trace.get('seat')) for trace in traces),
             'Trace records need explicit integer seats')
        trace_by_seat = {trace.get('seat'): trace for trace in traces}
        need(len(trace_by_seat) == len(peers) and set(trace_by_seat) == set(range(len(peers))),
             'Duplicate/missing trace seat')
        need(all(trace.get('parameters') == traces[0].get('parameters') for trace in traces),
             'Trace configurations are not symmetric')
        result['traceParameters'] = traces[0].get('parameters')
        result['sourceOptions'] = {key: report.get('options', {}).get(key) for key in
                                  ('frames', 'cpu_rates', 'one_way_ms', 'jitter_ms', 'spike_frame',
                                   'spike_ms', 'spike_delay_ms', 'spike_seat', 'touch_motion_hz')}
        for seat, peer in enumerate(peers):
            out = {'seat': seat, 'passed': False, 'errors': [], 'droppedErrors': 0}
            result['peers'].append(out)
            try:
                trace = trace_by_seat[seat]
                need(trace.get('runtimeIdentity') == identities[seat], 'Trace/runtime seat identity mismatch')
                out['runtimeIdentity'] = identities[seat]
                out['runId'] = trace.get('runId')
                path, out['traceArtifact'] = trace_artifact(report_path, trace)
                col, out['measuredWindow'], targets = select_targets(peer, context_ms)
                out['rowSchema'], out['columnIndices'] = peer['rowSchema'], col
                out['targets'] = targets
                markers, count = raw_markers(reader, path, trace)
                out['rawTraceEventCount'] = count
                out['clock'] = map_clock(markers, trace, peer)
                transport = peer.get('transportTrace') or {}
                out['transportSchema'] = {key: transport.get(key) for key in
                                          ('schema', 'rowSchema', 'peerSchema', 'sampling', 'clock')}
                add_context(peer, col, targets, out['clock'])
                scan_targets(reader, path, out['clock'], targets, count)
                after = digest_file(path, LIMITS['traceBytes'])
                need(after['sha256'] == out['traceArtifact']['sha256'] and
                     after['bytes'] == out['traceArtifact']['bytes'], 'Trace changed during analysis')
                out['traceShaRecheckedAfterAnalysis'] = True
                out['passed'] = all(target['passed'] for target in targets)
            except Exception as error:
                add_error(out, 'seat-analysis', error)
        after = digest_file(report_path, LIMITS['reportBytes'])
        need(after['sha256'] == result['sourceReport']['sha256'], 'Source report changed during analysis')
        reader_after = digest_file(Path(result['traceReaderSource']['path']), 1024 * 1024)
        need(reader_after['sha256'] == result['traceReaderSource']['sha256'], 'Trace reader changed during analysis')
        result['sourceReportShaRecheckedAfterAnalysis'] = True
        result['traceReaderShaRecheckedAfterAnalysis'] = True
        result['passed'] = bool(result['peers']) and all(peer['passed'] for peer in result['peers'])
    except Exception as error:
        add_error(result, 'analysis', error)
    if result['errors'] or result['droppedErrors']:
        result['passed'] = False
    return result


def summary_for(result, details_path):
    peers = []
    for peer in result['peers']:
        targets = []
        for target in peer.get('targets', []):
            wall = target.get('outsideRunTaskWall') or {}
            targets.append({
                'kind': target['kind'], 'rowIndex': target['rowIndex'],
                'currentEndMs': target['currentEndMs'], 'endGapMs': target['endGapMs'],
                'nativeCallbackMs': target['nativeCallbackMs'], 'outsideGapSignedMs': target['outsideGapSignedMs'],
                'runTaskWallUnionMs': wall.get('runTaskWallUnionMs'),
                'uncoveredByRunTaskWallMs': wall.get('uncoveredByRunTaskWallMs'),
                'selectedEventCount': target['selectedEventCount'], 'eventOverflow': target['eventOverflow'],
                'eventCountsByKind': target.get('eventCountsByKind'),
                'passed': target.get('passed', False), 'errors': target['errors'],
            })
        peers.append({'seat': peer['seat'], 'passed': peer['passed'],
                      'clockOffsetDriftUs': (peer.get('clock') or {}).get('offsetDriftUs'),
                      'targets': targets, 'errors': peer['errors']})
    return {'schema': SCHEMA + '-summary', 'passed': result['passed'],
            'scope': result['scope'], 'detailsPath': str(details_path),
            'sourceReport': result.get('sourceReport'), 'peers': peers, 'errors': result['errors']}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', required=True, type=Path)
    parser.add_argument('--output', type=Path, help='New detail JSON path; never overwrites existing evidence')
    parser.add_argument('--context-ms', type=float, default=100)
    args = parser.parse_args()
    if not finite(args.context_ms) or not 0 <= args.context_ms <= 1000:
        parser.error('--context-ms must be finite and within 0..1000')
    report_path = args.report.resolve()
    output = (args.output or report_path.with_name('chrome-gap-analysis.json')).resolve()
    summary_path = output.with_name(output.stem + '.summary.json')
    need(output.suffix == '.json' and output != report_path and summary_path != report_path,
         'Output must be a distinct .json file')
    need(not output.exists() and not summary_path.exists(), 'Output already exists; choose a new evidence path')
    result = analyze(report_path, args.context_ms)
    raw = json.dumps(result, ensure_ascii=False, allow_nan=False, indent=2).encode('utf-8') + b'\n'
    if len(raw) > LIMITS['outputBytes']:
        # Still persist an explicit FAIL receipt; never silently omit evidence
        # from an otherwise passing analysis when the final byte bound is hit.
        result = {
            'schema': SCHEMA, 'passed': False, 'scope': result['scope'], 'limits': LIMITS,
            'sourceReport': result.get('sourceReport'), 'peers': [], 'droppedErrors': 0,
            'errors': [{'stage': 'output-bound', 'message': 'Detailed output exceeded its byte bound; evidence omitted, FAIL'}],
            'outputOverflow': {'requestedBytes': len(raw), 'maximumBytes': LIMITS['outputBytes']},
            'summaryBeforeOverflow': summary_for(result, output),
        }
        raw = json.dumps(result, ensure_ascii=False, allow_nan=False, indent=2).encode('utf-8') + b'\n'
        need(len(raw) <= LIMITS['outputBytes'], 'Even the overflow failure receipt exceeds its byte bound')
    summary = summary_for(result, output)
    summary['detailsArtifact'] = {'bytes': len(raw), 'sha256': hashlib.sha256(raw).hexdigest()}
    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open('xb') as target:
        target.write(raw)
    with summary_path.open('x', encoding='utf-8', newline='\n') as target:
        json.dump(summary, target, ensure_ascii=False, allow_nan=False, indent=2)
        target.write('\n')
    # Only the small summary goes to stdout. Raw rows/events occur once in details.
    print(json.dumps(summary, ensure_ascii=False, allow_nan=False))
    return 0 if result['passed'] else 1


if __name__ == '__main__':
    try:
        sys.exit(main())
    except Exception as failure:
        print(json.dumps({'schema': SCHEMA, 'passed': False, 'error': str(failure)[:700]}), file=sys.stderr)
        sys.exit(1)
