"""Optional, bounded Chrome event tracing; never starts a CPU profiler.

CDP shapes were checked against the installed Playwright 1.58.0 protocol.d.ts
and sync_api/_generated.py, archived in artifacts/rollback-smoothness/
api-reference-playwright/. Only small control messages run beside native loops.
The caller must end every trace before the final fence, then call finish only
after that fence or after stopping/closing all owned pages on a failure.
"""
import base64
import gzip
import hashlib
import json
import math
import time
from pathlib import Path

SCHEMA = 'th10-chrome-trace/1'
READ_BYTES = 64 * 1024
MAX_STREAM_BYTES = 128 * 1024 * 1024
MAX_JSON_CHARS = 256 * 1024 * 1024
MAX_VALUE_CHARS = 8 * 1024 * 1024
MAX_EVENTS = 2000000
MAX_THREADS = 256
TRACE_PARAMETERS = {
    'transferMode': 'ReturnAsStream',
    'streamFormat': 'json',
    'streamCompression': 'gzip',
    'tracingBackend': 'chrome',
    'traceConfig': {
        'recordMode': 'recordUntilFull',
        'traceBufferSizeInKb': 32768,
        'enableSampling': False,
        'enableSystrace': False,
        'enableArgumentFilter': False,
        'includedCategories': [
            'toplevel', 'devtools.timeline', 'blink.user_timing', 'v8',
        ],
        'excludedCategories': [
            # Diagnostic04 exhausted 32 MiB with per-draw cc/gpu events before
            # the final marker. Keep the same bound and the complete-run gate.
            'cc', 'gpu',
            'disabled-by-default-devtools.screenshot',
            'disabled-by-default-devtools.timeline.stack',
            'disabled-by-default-v8.cpu_profiler',
            'disabled-by-default-v8.cpu_profiler.hires',
            'disabled-by-default-cpu_profiler',
            'disabled-by-default-v8.runtime_stats_sampling',
            'disabled-by-default-memory-infra', 'memory-infra',
        ],
    },
}


def _number(value):
    return isinstance(value, (int, float)) and not isinstance(value, bool) and math.isfinite(value)


class _JsonCursor:
    """Incremental JSON decoding with separate file and single-value bounds."""
    def __init__(self, source):
        self.source, self.buffer, self.pos = source, '', 0
        self.eof, self.total = False, 0
        self.decoder = json.JSONDecoder()

    def more(self):
        self.buffer = self.buffer[self.pos:]
        self.pos = 0
        chunk = self.source.read(READ_BYTES)
        self.total += len(chunk)
        if self.total > MAX_JSON_CHARS:
            raise ValueError('Expanded trace exceeds the explicit JSON character bound')
        if not chunk:
            self.eof = True
        self.buffer += chunk
        if len(self.buffer) > MAX_VALUE_CHARS:
            raise ValueError('A trace JSON value exceeds the explicit single-value bound')

    def peek(self):
        while True:
            while self.pos < len(self.buffer) and self.buffer[self.pos].isspace():
                self.pos += 1
            if self.pos < len(self.buffer):
                return self.buffer[self.pos]
            if self.eof:
                return None
            self.more()

    def expect(self, character):
        if self.peek() != character:
            raise ValueError('Malformed trace JSON structure')
        self.pos += 1

    def value(self):
        self.peek()
        while True:
            try:
                value, end = self.decoder.raw_decode(self.buffer, self.pos)
            except json.JSONDecodeError:
                if self.eof:
                    raise ValueError('Incomplete or malformed trace JSON value') from None
                self.more()
                continue
            # A scalar at a chunk boundary may continue in the next chunk.
            if end == len(self.buffer) and not self.eof:
                self.more()
                continue
            self.pos = end
            return value


def _trace_events(path):
    with gzip.open(path, 'rt', encoding='utf-8') as source:
        reader = _JsonCursor(source)
        reader.expect('{')
        seen_events = False
        while reader.peek() != '}':
            key = reader.value()
            if not isinstance(key, str):
                raise ValueError('Trace root key is not a string')
            reader.expect(':')
            if key == 'traceEvents':
                if seen_events:
                    raise ValueError('Duplicate traceEvents array')
                seen_events = True
                reader.expect('[')
                while reader.peek() != ']':
                    event = reader.value()
                    if not isinstance(event, dict):
                        raise ValueError('Trace event is not an object')
                    yield event
                    if reader.peek() == ']':
                        break
                    reader.expect(',')
                    if reader.peek() == ']':
                        raise ValueError('Trailing comma in traceEvents')
                reader.expect(']')
            else:
                reader.value()
            if reader.peek() == '}':
                break
            reader.expect(',')
            if reader.peek() == '}':
                raise ValueError('Trailing comma in trace root')
        reader.expect('}')
        if not seen_events or reader.peek() is not None:
            raise ValueError('Missing traceEvents array or trailing JSON data')


def summarize_trace(path, markers):
    """Availability and bounded long-event context, without summing nested spans."""
    counts = {'events': 0, 'completeWallEvents': 0, 'threadTimestampEvents': 0,
              'threadDurationEvents': 0, 'wallWithThreadDuration': 0,
              'wallWithoutThreadDuration': 0, 'forbiddenCategoryEvents': 0,
              'unassignedThreadEvents': 0, 'threadSummaryOverflowEvents': 0}
    threads, matches = {}, {m['label']: [] for m in markers}
    category_hits = dict.fromkeys(TRACE_PARAMETERS['traceConfig']['includedCategories'], 0)
    marker_counts = dict.fromkeys(matches, 0)
    marker_thread, marker_thread_mismatch = None, False
    forbidden_fragments = ('cpu_profiler', 'runtime_stats_sampling', 'screenshot', 'memory-infra')
    for event in _trace_events(path):
        counts['events'] += 1
        if counts['events'] > MAX_EVENTS:
            raise ValueError('Trace exceeds the explicit event-count bound')
        raw_category = str(event.get('cat', ''))
        event_categories = {name.strip() for name in raw_category.split(',')}
        for requested in category_hits:
            if requested in event_categories:
                category_hits[requested] += 1
        name, category = str(event.get('name', ''))[:240], raw_category[:480]
        if any(part in category for part in forbidden_fragments):
            counts['forbiddenCategoryEvents'] += 1
        pid, tid = event.get('pid'), event.get('tid')
        assigned = isinstance(pid, int) and isinstance(tid, int)
        key = (pid, tid) if assigned else None
        thread = threads.get(key)
        if assigned and thread is None and len(threads) < MAX_THREADS:
            thread = threads[key] = {'pid': pid, 'tid': tid, 'name': None, 'events': 0,
                                    'completeWallEvents': 0, 'threadTimestampEvents': 0,
                                    'threadDurationEvents': 0, 'wallWithThreadDuration': 0,
                                    'wallWithoutThreadDuration': 0, 'timelineCompleteSpans': 0,
                                    'longestCompleteEvents': []}
        if not assigned:
            counts['unassignedThreadEvents'] += 1
        elif thread is None:
            counts['threadSummaryOverflowEvents'] += 1
        if thread:
            thread['events'] += 1
            if event.get('ph') == 'M' and name == 'thread_name':
                arguments = event.get('args')
                if isinstance(arguments, dict):
                    thread['name'] = str(arguments.get('name', ''))[:160]
        tts, tdur = event.get('tts'), event.get('tdur')
        for field, present in [('threadTimestampEvents', _number(tts)),
                               ('threadDurationEvents', _number(tdur) and tdur >= 0)]:
            if present:
                counts[field] += 1
                if thread:
                    thread[field] += 1
        ts, dur = event.get('ts'), event.get('dur')
        if event.get('ph') == 'X' and _number(ts) and _number(dur) and dur >= 0:
            with_thread = _number(tdur) and tdur >= 0
            field = 'wallWithThreadDuration' if with_thread else 'wallWithoutThreadDuration'
            counts['completeWallEvents'] += 1
            counts[field] += 1
            if thread:
                thread['completeWallEvents'] += 1
                thread[field] += 1
                if 'devtools.timeline' in event_categories:
                    thread['timelineCompleteSpans'] += 1
                detail = {k: event[k] for k in ['pid', 'tid', 'ts', 'dur', 'tts', 'tdur']
                          if k in event and _number(event[k])}
                detail.update(name=name, cat=category, wallDurationMs=dur / 1000)
                if with_thread:
                    detail['threadDurationMs'] = tdur / 1000
                longest = thread['longestCompleteEvents']
                if len(longest) < 8 or dur > longest[-1]['dur']:
                    longest.append(detail)
                    longest.sort(key=lambda value: value['dur'], reverse=True)
                    del longest[8:]
        if name in matches and 'blink.user_timing' in category and assigned and _number(ts):
            marker_counts[name] += 1
            if marker_thread is None:
                marker_thread = key
            elif key != marker_thread:
                marker_thread_mismatch = True
            if len(matches[name]) < 4:
                matches[name].append({k: event[k] for k in ['pid', 'tid', 'ts', 'ph'] if k in event})
    locations = {(e['pid'], e['tid']) for values in matches.values() for e in values}
    markers_present = bool(matches) and all(marker_counts.values())
    one_thread = markers_present and len(locations) == 1 and not marker_thread_mismatch
    renderer = threads.get(next(iter(locations))) if one_thread else None
    mapped = []
    for marker in markers:
        values = matches[marker['label']]
        raw = min(values, key=lambda value: value['ts']) if values else None
        page = marker.get('page') or {}
        mark_ms = page.get('markStartTimeMs')
        mapped.append({'label': marker['label'], 'stage': marker['stage'],
                       'matchCount': marker_counts[marker['label']], 'firstMatches': values,
                       'traceMinusPageMarkUs': raw['ts'] - mark_ms * 1000
                       if raw and _number(mark_ms) else None})
    required_stages = {m['stage'] for m in markers}
    complete_markers = (one_thread and required_stages == {'before-native-start', 'native-loops-complete'}
                        and len(markers) == 2)
    if complete_markers:
        first_ts, last_ts = (min(matches[m['label']], key=lambda value: value['ts'])['ts'] for m in markers)
        complete_markers = last_ts > first_ts
    return {
        'scope': 'Event fields and finite long-span context only; nested spans are not summed. '
                 'ts/dur are Chrome trace microseconds; tts/tdur are recorded only when actually present. '
                 'A wall span without tdur cannot establish physical CPU consumption or why a thread waited. '
                 'Detailed cc/gpu categories are excluded to preserve a complete bounded renderer trace. '
                 'A low thread-time/wall-time ratio cannot distinguish GPU waits, OS scheduling or other blocking. '
                 'No naturally emitted GC event does not establish the absence of GC blocking.',
        'counts': counts, 'threadTimeAvailable': counts['threadDurationEvents'] > 0,
        'requestedCategoryEvents': category_hits,
        'rendererTimelineCompleteSpans': renderer['timelineCompleteSpans'] if renderer else None,
        'rendererThreadTimeAvailable': bool(renderer and renderer['threadDurationEvents']),
        'rendererThreadTimeStatus': ('thread-unidentified' if renderer is None else
                                    'no-complete-wall-events' if not renderer['completeWallEvents'] else
                                    'missing' if not renderer['wallWithThreadDuration'] else
                                    'partial' if renderer['wallWithoutThreadDuration'] else 'present'),
        'markersComplete': bool(complete_markers), 'clockMarkers': mapped,
        'rendererThread': renderer, 'threads': list(threads.values()),
    }


class ChromeTrace:
    def __init__(self, seat, output_path, run_id, identity):
        self.seat, self.run_id = seat, run_id
        self.path = Path(output_path).with_name(
            Path(output_path).stem + '.chrome-trace-' + run_id + '-seat-' + str(seat) + '.json.gz')
        self.session, self.completion = None, None
        self.started = self.end_requested = self.finished = False
        self.report = {
            'schema': SCHEMA, 'seat': seat, 'runId': run_id, 'passed': False,
            'runtimeIdentity': identity, 'parameters': TRACE_PARAMETERS,
            'scope': 'Optional symmetric browser-process renderer/task event trace; diagnostic timing is not pure performance. '
                     'Detailed cc/gpu categories excluded; no CPU Profiler, JS stack sampling, system tracing, screenshots or memory dumps.',
            'clockScope': 'Each page performance clock has its own recorded timeOrigin. '
                          'Page marks map to trace events in that same realm; host monotonic bounds are '
                          'Python time.monotonic_ns. Date.now is metadata, never a conversion between clocks. '
                          'Do not subtract raw trace timestamps from different browser processes without mapping.',
            'limits': {'traceBufferKiB': 32768, 'streamBytes': MAX_STREAM_BYTES,
                       'jsonCharacters': MAX_JSON_CHARS, 'singleJsonValueCharacters': MAX_VALUE_CHARS,
                       'events': MAX_EVENTS, 'summaryThreads': MAX_THREADS, 'longEventsPerThread': 8},
            'markers': [], 'errors': [], 'droppedErrors': 0, 'dataLossOccurred': None,
            'streamComplete': False, 'summaryComplete': False, 'streamClosed': False, 'sessionDetached': False,
        }

    def _error(self, stage, error):
        value = {'stage': stage, 'message': str(error)[:600]}
        if len(self.report['errors']) < 16:
            self.report['errors'].append(value)
        else:
            self.report['droppedErrors'] += 1

    def _on_complete(self, payload):
        if self.completion is not None:
            self._error('tracingComplete', 'Duplicate completion event')
            return
        self.completion = {k: payload.get(k) for k in
                           ['stream', 'traceFormat', 'streamCompression', 'dataLossOccurred']}
        self.report['completionHostMonotonicNs'] = time.monotonic_ns()
        self.report['dataLossOccurred'] = payload.get('dataLossOccurred')
        self.report['reportedFormat'] = payload.get('traceFormat')
        self.report['reportedCompression'] = payload.get('streamCompression')

    def _on_nonstream(self, payload):
        self.report['unexpectedNonstreamEvents'] = (
            self.report.get('unexpectedNonstreamEvents', 0) + len(payload.get('value', [])))
        if not self.report.get('nonstreamErrorRecorded'):
            self.report['nonstreamErrorRecorded'] = True
            self._error('dataCollected', 'ReturnAsStream unexpectedly delivered event payloads')

    def start(self, browser):
        try:
            self.session = browser.new_browser_cdp_session()
            self.session.on('Tracing.tracingComplete', self._on_complete)
            self.session.on('Tracing.dataCollected', self._on_nonstream)
            self.report['browserVersion'] = browser.version
            self.report['startHostMonotonicNs'] = time.monotonic_ns()
            self.session.send('Tracing.start', TRACE_PARAMETERS)
            self.started = True
            self.report['startAcknowledged'] = True
            # chrome-trace-api-probe-01 observed [] before start and 288 names
            # after this acknowledgement. Keep the support gate after start;
            # started must remain true if enumeration fails so cleanup ends it.
            requested = TRACE_PARAMETERS['traceConfig']['includedCategories']
            self.report['categorySupport'] = {
                'enumerationPhase': 'after-Tracing.start-acknowledged',
                'requested': requested, 'count': None, 'missing': None,
            }
            available = self.session.send('Tracing.getCategories')['categories']
            if not isinstance(available, list) or not all(isinstance(name, str) for name in available):
                raise ValueError('Malformed trace category enumeration')
            missing = [name for name in requested if name not in available]
            self.report['categorySupport'].update(count=len(available), missing=missing)
            if missing:
                raise ValueError('Requested trace categories unavailable after start: ' + ', '.join(missing))
        except Exception as error:
            self._error('start', error)

    def mark(self, page, stage):
        if not self.started or self.end_requested:
            return
        label = 'th10-chrome-trace:' + self.run_id + ':seat-' + str(self.seat) + ':' + stage
        marker = {'label': label, 'stage': stage, 'hostBeforeMonotonicNs': time.monotonic_ns()}
        self.report['markers'].append(marker)
        try:
            self.session.send('Tracing.recordClockSyncMarker', {'syncId': label})
            marker['cdpClockSyncAcknowledged'] = True
            marker['page'] = page.evaluate("""label => {
                const mark = performance.mark(label);
                return {performanceTimeOriginMs:performance.timeOrigin,
                        markStartTimeMs:mark.startTime,performanceNowMs:performance.now(),
                        dateNowMs:Date.now(),visibilityState:document.visibilityState,
                        focused:document.hasFocus()};
            }""", label)
            origins = [m['page']['performanceTimeOriginMs'] for m in self.report['markers'] if m.get('page')]
            if len(set(origins)) != 1:
                raise ValueError('Clock markers crossed page performance time origins')
        except Exception as error:
            self._error('clock-marker-' + stage, error)
        finally:
            marker['hostAfterMonotonicNs'] = time.monotonic_ns()

    @property
    def ready(self):
        return self.started and not self.report['errors']

    def end(self):
        """Only request stop; never wait for tracingComplete or read its stream."""
        if not self.started or self.end_requested:
            return
        self.end_requested = True
        self.report['endHostMonotonicNs'] = time.monotonic_ns()
        try:
            self.session.send('Tracing.end')
            self.report['endAcknowledged'] = True
        except Exception as error:
            self._error('end', error)

    def finish(self, stage):
        """May do large IO/analysis; caller guarantees the declared drain boundary."""
        if self.finished:
            return self.report
        self.finished = True
        self.report['drainStage'] = stage
        self.report['drainHostMonotonicNs'] = time.monotonic_ns()
        handle = None
        try:
            self.end()
            if not self.started:
                raise ValueError('Trace was not started')
            deadline = time.monotonic() + 30
            while self.completion is None:
                if time.monotonic() >= deadline:
                    raise TimeoutError('No tracingComplete within 30 seconds after drain boundary')
                # A small browser-level CDP round trip pumps Playwright events,
                # including after failure cleanup closed every owned page.
                self.session.send('Tracing.getCategories')
                if self.completion is None:
                    time.sleep(.025)
            handle = self.completion.get('stream')
            if not isinstance(handle, str) or not handle:
                raise ValueError('tracingComplete did not provide a stream handle')
            self.report['streamPath'] = str(self.path)
            count, chunks, digest = 0, 0, hashlib.sha256()
            with self.path.open('xb') as target:
                while True:
                    chunk = self.session.send('IO.read', {'handle': handle, 'size': READ_BYTES})
                    data = chunk['data']
                    raw = base64.b64decode(data, validate=True) if chunk.get('base64Encoded') else data.encode('utf-8')
                    if count + len(raw) > MAX_STREAM_BYTES:
                        raise ValueError('Trace stream exceeds the explicit byte bound')
                    target.write(raw)
                    digest.update(raw)
                    count += len(raw)
                    chunks += 1
                    if chunk.get('eof') is True:
                        break
                    if not raw:
                        raise ValueError('Trace stream made no progress without EOF')
            self.report['streamComplete'] = True
            self.report['artifact'] = {'path': str(self.path), 'bytes': count,
                                       'sha256': digest.hexdigest(), 'format': self.completion.get('traceFormat'),
                                       'compression': self.completion.get('streamCompression'), 'validated': False,
                                       'readCalls': chunks}
            if self.completion.get('traceFormat') not in [None, 'json']:
                raise ValueError('Returned trace format differs from requested JSON')
            if self.completion.get('streamCompression') not in [None, 'gzip']:
                raise ValueError('Returned trace compression differs from requested gzip')
            self.report['summary'] = summarize_trace(self.path, self.report['markers'])
            self.report['summaryComplete'] = True
            self.report['artifact'].update(format='json', compression='gzip', validated=True)
            if self.report['dataLossOccurred'] is not False:
                raise ValueError('Trace data loss is true or was not reported')
            if not self.report['summary']['markersComplete']:
                raise ValueError('Both native-boundary markers were not found on one renderer thread')
            if self.report['summary']['counts']['forbiddenCategoryEvents']:
                raise ValueError('Trace contains excluded sampling/screenshot/memory-dump categories')
        except Exception as error:
            self._error('finish', error)
        finally:
            if handle is None and self.completion:
                handle = self.completion.get('stream')
            if handle and self.session:
                try:
                    self.session.send('IO.close', {'handle': handle})
                    self.report['streamClosed'] = True
                except Exception as error:
                    self._error('stream-close', error)
            if self.session:
                try:
                    self.session.detach()
                    self.report['sessionDetached'] = True
                except Exception as error:
                    self._error('session-detach', error)
            self.report['finishedHostMonotonicNs'] = time.monotonic_ns()
            self.report['passed'] = (not self.report['errors'] and self.report['streamComplete']
                                     and self.report['summaryComplete']
                                     and self.report['streamClosed'] and self.report['sessionDetached']
                                     and self.report['dataLossOccurred'] is False)
        return self.report
