"""Capture every stable authoritative tick from one ordinary TH08 Replay."""

import argparse
import hashlib
import json
from pathlib import Path

from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[2]


def inspect_replay(raw, start_stage):
    if len(raw) < 104:
        raise ValueError('Invalid TH08 replay size')
    plain = bytearray(raw)
    file_size = int.from_bytes(plain[12:16], 'little')
    if not 104 <= file_size <= len(plain):
        raise ValueError('Invalid TH08 replay file size')
    key = plain[21]
    for index in range(24, file_size):
        plain[index] = (plain[index] - key) & 0xff
        key = (key + 7) & 0xff
    output = bytearray(104 + int.from_bytes(plain[28:32], 'little'))
    output[:104] = plain[:104]
    source = memoryview(plain)[104:file_size]
    dictionary = bytearray(8192)
    cursor = 0
    mask = 0x80
    current = 0
    head = 1
    written = 104

    def bit():
        nonlocal cursor, mask, current
        if mask == 0x80:
            current = source[cursor] if cursor < len(source) else 0
            cursor += 1
        value = 1 if current & mask else 0
        mask >>= 1
        if not mask:
            mask = 0x80
        return value

    def bits(count):
        value = 0
        for _ in range(count):
            value = (value << 1) | bit()
        return value

    def put(value):
        nonlocal head, written
        if written >= len(output):
            raise ValueError('TH08 replay payload exceeds declared size')
        output[written] = value
        written += 1
        dictionary[head] = value
        head = (head + 1) & 8191

    while True:
        if bit():
            put(bits(8))
        else:
            offset = bits(13)
            if not offset:
                break
            for index in range(bits(4) + 3):
                put(dictionary[(offset + index) & 8191])
    stages = []
    for stage in range(start_stage, 9):
        offset = int.from_bytes(output[32 + stage * 4:36 + stage * 4], 'little')
        if offset:
            stages.append({'stage': stage, 'score': int.from_bytes(output[offset:offset + 4], 'little')})
    if not stages:
        raise ValueError('Replay has no selected stage')
    return {'stages': stages, 'finalScore': stages[-1]['score']}

BOOTSTRAP = r'''async ({bytes,startStage}) => {
  const {bootReplayCapture}=await import('/replay-capture.mjs');
  return bootReplayCapture({presentationLab,bytes,startStage});
}'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--replay', type=Path, required=True)
    parser.add_argument('--start-stage', type=int, default=0)
    parser.add_argument('--url', default='http://127.0.0.1:8132/')
    parser.add_argument('--max-ticks', type=int, default=200_000)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if not 0 <= args.start_stage <= 8 or not 300 <= args.max_ticks <= 500_000:
        raise ValueError('Invalid stage or tick bound')
    replay = args.replay.resolve().read_bytes()
    expected = inspect_replay(replay, args.start_stage)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    rows_path = args.output.with_suffix('.rows.jsonl')
    ledger_path = args.output.with_suffix('.cursors.jsonl')
    errors = []
    result = None
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        context = browser.new_context(viewport={'width': 1280, 'height': 960}, service_workers='block')
        page = context.new_page(); page.set_default_timeout(180_000)
        page.on('pageerror', lambda error: errors.append(str(error)))
        try:
            page.goto(args.url, wait_until='load')
            page.wait_for_function('window.presentationLab !== undefined')
            initial = page.evaluate(BOOTSTRAP, {'bytes': list(replay), 'startStage': args.start_stage})
            ticks = 0; application_ticks = initial['applicationTicks']; done = initial['done']; tail = []; stages = []; final_score = None
            stop_reason = initial['stopReason']; terminal_scene = initial['terminalScene']; journal = initial['journal']
            with rows_path.open('w', encoding='utf-8', buffering=1) as rows, ledger_path.open('w', encoding='utf-8', buffering=1) as cursors:
                for sample in initial['ledger']:
                    cursors.write(json.dumps(sample, separators=(',', ':')) + '\n')
                for row in initial['rows']:
                    rows.write(json.dumps(row, separators=(',', ':')) + '\n'); ticks += 1
                while not done and application_ticks < args.max_ticks:
                    batch = page.evaluate('count => replayVerifierCapture.run(count)', min(600, args.max_ticks - application_ticks))
                    for row in batch['rows']:
                        rows.write(json.dumps(row, separators=(',', ':')) + '\n'); ticks += 1
                    for sample in batch['ledger']:
                        cursors.write(json.dumps(sample, separators=(',', ':')) + '\n')
                    journal = batch['journal']
                    application_ticks, done, tail, stages = batch['applicationTicks'], batch['done'], batch['tail'], batch['stages']
                    stop_reason, terminal_scene = batch['stopReason'], batch['terminalScene']
                    final_score = batch['finalScore']
                    if ticks and ticks % 6000 < len(batch['rows']):
                        print(json.dumps({'ticks': ticks, 'stages': stages, 'last': batch['rows'][-1]}), flush=True)
            if not done:
                stop_reason = 'maximum-ticks'
                terminal_scene = page.evaluate('() => presentationLab.controller.core.status(presentationLab.controller.runtime.app,0)')
            complete = done and stop_reason == 'returned-to-title' and terminal_scene == 1 and not errors
            terminal_match = stages == [item['stage'] for item in expected['stages']] and final_score == expected['finalScore']
            result = {'schema': 'th08/current-replay-tick-capture/v3', 'complete': complete,
                      'stopReason': stop_reason, 'terminalScene': terminal_scene,
                      'terminalMatch': terminal_match,
                      'provider': 'th08-eagler/diagnostic-browser', 'replay': {'path': str(args.replay.resolve()), 'bytes': len(replay), 'sha256': hashlib.sha256(replay).hexdigest()},
                      'build': initial['build'], 'rows': {'path': str(rows_path.resolve()), 'ticks': ticks},
                      'applicationTicks': application_ticks, 'journal': journal,
                      'cursorLedger': {'path': str(ledger_path.resolve()), 'rows': application_ticks},
                      'navigation': initial['navigation'], 'expected': expected,
                      'stages': stages, 'finalScore': final_score, 'tail': tail, 'errors': errors}
        except Exception as error:
            result = {'schema': 'th08/current-replay-tick-capture/v3', 'complete': False,
                      'stopReason': 'capture-error', 'terminalScene': None,
                      'replay': {'path': str(args.replay.resolve()), 'sha256': hashlib.sha256(replay).hexdigest()},
                      'rows': {'path': str(rows_path.resolve())}, 'errors': [*errors, repr(error)]}
            raise
        finally:
            args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
            context.close(); browser.close()
    if not result['complete']:
        raise RuntimeError(f'Incomplete TH08 candidate capture; evidence: {args.output}')
    print(json.dumps({'complete': True, 'ticks': result['rows']['ticks'], 'stages': result['stages'], 'output': str(args.output)}), flush=True)


if __name__ == '__main__':
    main()
