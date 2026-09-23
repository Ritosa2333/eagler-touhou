"""Native Replay seek PCM/refill and physical-output suppression regression."""
import argparse
import hashlib
import json
from pathlib import Path

from rollback_testkit import call, fixture

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8140/')
parser.add_argument('--output', default='artifacts/multiplayer-tests/replay-audio.json')
args = parser.parse_args()
probe = Path(__file__).with_name('replay-audio-seek.mjs')
digest = hashlib.sha256(probe.read_bytes()).hexdigest()
report = {'passed': False, 'scope': 'native Replay seek audio lifecycle', 'probeSha256': digest}
with fixture(args.url, report, args.output) as test:
    context = test.browser.new_context(service_workers='block')
    test.contexts.append(context)
    page = context.new_page()
    page.on('pageerror', lambda error: test.errors.append(str(error)))
    page.goto(args.url)
    page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
    test.identities.append(call(page, 'multiplayerSmoke.identity()'))
    # A real user activation opens the browser audio device; the native mixer
    # alone owns the PCM, seek progression, ring notifications and lifecycle.
    page.locator('canvas').click()
    result = call(page, 'multiplayerSmoke.audioSeek()')
    report['result'] = result
    assert result['passed'], result
    assert hashlib.sha256(probe.read_bytes()).hexdigest() == digest, 'Audio probe changed during its run'
print(json.dumps({'passed': report['passed'], 'report': args.output}), flush=True)
