"""Stopped-loop browser helpers; these do not implement the game or netcode."""
import json
import hashlib
import sys
import uuid
from contextlib import contextmanager
from pathlib import Path

from playwright.sync_api import sync_playwright


def call(page, expression, arg=None):
    return page.evaluate(expression, arg)


def net(page):
    return call(page, 'multiplayerSmoke.netStatus()')


def state(page):
    return call(page, 'multiplayerSmoke.status()')


def hashes(page):
    return call(page, 'multiplayerSmoke.canonical()')


def tick(page, count=1):
    return call(page, '(n)=>multiplayerSmoke.netTicks(n)', count)


def submit(page, seat, frame, buttons=0):
    result = call(page, '(x)=>multiplayerSmoke.submitRemote(...x)',
                  [seat, frame, buttons])
    # Export returns RemoteInputResult + 1. Do not accept error enum values as
    # success merely because they are nonzero.
    assert result in (1, 2, 3, 4), (seat, frame, buttons, result, net(page))
    return result


def barrier(pages):
    hellos = [call(page, 'multiplayerSmoke.sessionPacket(1)') for page in pages]
    for seat, page in enumerate(pages):
        for peer, packet in enumerate(hellos):
            if peer != seat:
                assert call(page, '(p)=>multiplayerSmoke.applySession(p)', packet)
    for page in pages:
        assert call(page, 'multiplayerSmoke.markReady()')
    ready = [call(page, 'multiplayerSmoke.sessionPacket(2)') for page in pages]
    for seat, page in enumerate(pages):
        for peer, packet in enumerate(ready):
            if peer != seat:
                assert call(page, '(p)=>multiplayerSmoke.applySession(p)', packet)
        assert call(page, 'multiplayerSmoke.canStart()')


class Fixture:
    def __init__(self, browser, url):
        self.browser, self.url = browser, url
        self.contexts, self.errors = [], []
        self.identities = []
        self.locals = {}

    def open(self, local=0, loadouts=None, difficulty=1, seed=1234, session_id=None, history=None):
        loadouts = loadouts or [[0, 0], [1, 1]]
        context = self.browser.new_context(service_workers='block')
        self.contexts.append(context)
        page = context.new_page()
        self.locals[page] = local
        page.on('pageerror', lambda error: self.errors.append(str(error)))
        page.goto(self.url)
        page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
        self.identities.append(call(page, 'multiplayerSmoke.identity()'))
        low, high = session_id or (0x51455201, 0x10203040)
        if history is not None:
            assert call(page, '(x)=>multiplayerSmoke.historyProfile(x)', history)
        call(page, '(x)=>multiplayerSmoke.startNet(...x)',
             [loadouts, local, difficulty, seed, low, high])
        return page

    def pair(self, difficulty=1, local=0):
        reference = self.open(local=local, difficulty=difficulty)
        helper = self.open(local=1-local, difficulty=difficulty)
        barrier([reference, helper])
        late = self.open(local=local, difficulty=difficulty)
        # HELLO/READY packets are duplicate-safe; helper has genuinely received
        # HELLO and marked itself ready (not fabricated READY control data).
        barrier([late, helper])
        return reference, late

    def advance(self, page, last, seats=2, buttons=0):
        for _ in range(1100 + last):
            if net(page)[3] >= last:
                break
            frame = net(page)[2]
            for seat in range(seats):
                if seat != self.locals[page]:
                    submit(page, seat, frame, buttons)
            tick(page)
        assert net(page)[3] == last, ('frontier', last, net(page), state(page))

    def close_contexts(self):
        for context in reversed(self.contexts):
            context.close()
        self.contexts.clear()
        self.locals.clear()


@contextmanager
def fixture(url, report, output):
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True,
                                              args=['--enable-unsafe-swiftshader'])
        test = Fixture(browser, url)
        report['passed'] = False
        report['browser'] = browser.version
        source_paths = [Path(__file__), Path(__file__).with_name('smoke.mjs'), Path(sys.argv[0]).resolve()]
        source_hashes = {str(path): hashlib.sha256(path.read_bytes()).hexdigest() for path in source_paths}
        report['sources'] = source_hashes
        try:
            yield test
            assert not test.errors, test.errors
            assert len({value['wasmSha256'] for value in test.identities}) == 1, test.identities
            assert source_hashes == {str(path): hashlib.sha256(path.read_bytes()).hexdigest() for path in source_paths}, 'Fixture source changed during run'
            report['passed'] = True
        except BaseException as error:
            report['error'] = str(error)
            raise
        finally:
            report['pageErrors'] = test.errors
            report['runtimeIdentities'] = test.identities
            browser.close()
            output = Path(output)
            output.parent.mkdir(parents=True, exist_ok=True)
            output.write_text(json.dumps(report, indent=2), encoding='utf-8')
            history = output.parent / 'history'
            history.mkdir(parents=True, exist_ok=True)
            (history / (output.stem + '-' + uuid.uuid4().hex + '.json')).write_text(
                json.dumps(report, indent=2), encoding='utf-8')


def equal(reference, late, label):
    a, b = state(reference), state(late)
    ah, bh = hashes(reference), hashes(late)
    assert a == b, (label, a, b)
    assert ah[1:28] == bh[1:28], (label, 'full owner mismatch', ah, bh)
    aa, ba = [call(page, 'multiplayerSmoke.audioStatus()') for page in (reference,late)]
    assert aa == ba, (label, 'committed audio mismatch', aa, ba)
