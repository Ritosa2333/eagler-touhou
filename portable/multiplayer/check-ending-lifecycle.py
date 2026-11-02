"""Native all-seat terminal transition, SDL draw/clock and independent Ending input."""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

parser = argparse.ArgumentParser()
parser.add_argument('--url', required=True)
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('--first-only', action='store_true')
args = parser.parse_args()
report = {'passed': False, 'cases': [], 'errors': []}

def call(page, script, value=None):
    return page.evaluate(script, value)

def state(page):
    return call(page, '({ending:multiplayerSmoke.endingState(),native:multiplayerSmoke.nativeStatus()})')

with sync_playwright() as p:
    browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        for players, ending in ([(2, 1)] if args.first_only else [(2, 0), (2, 1), (2, 2), (3, 2)]):
            case = {'players': players, 'endingKind': ending, 'passed': False}
            report['cases'].append(case)
            pages = []
            try:
                for seat in range(players):
                    page = browser.new_page(viewport={'width': 640, 'height': 480})
                    pages.append(page)
                    page.on('pageerror', lambda error: report['errors'].append(str(error)))
                    page.goto(args.url)
                    page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
                    call(page, 'v=>multiplayerSmoke.start(...v)', [1234, list(range(players)), seat, [0x55667788, 0x10203040]])
                case['identity'] = call(pages[0], 'multiplayerSmoke.identity()')
                hello = [call(page, 'multiplayerSmoke.sessionPacket(1)') for page in pages]
                for seat, page in enumerate(pages):
                    for remote, wire in enumerate(hello):
                        if remote != seat:
                            assert call(page, 'p=>multiplayerSmoke.applyWire(p)', wire) == 0
                    assert call(page, 'multiplayerSmoke.markReady()')
                ready = [call(page, 'multiplayerSmoke.sessionPacket(2)') for page in pages]
                for seat, page in enumerate(pages):
                    for remote, wire in enumerate(ready):
                        if remote != seat:
                            assert call(page, 'p=>multiplayerSmoke.applyWire(p)', wire) == 0
                for frame in range(180):
                    for seat, page in enumerate(pages):
                        if frame == 0 and seat == 0:
                            assert call(page, 'multiplayerSmoke.captureInput(0,0,2,8188,-8192,0)')
                        else:
                            assert call(page, 'f=>multiplayerSmoke.capture(f,0)', frame)
                    for source, page in enumerate(pages):
                        for target, receiver in enumerate(pages):
                            if source != target:
                                wire = call(page, 'v=>multiplayerSmoke.inputPacket(...v)', [target, frame, frame+1])
                                assert call(receiver, 'p=>multiplayerSmoke.applyWire(p)', wire) == 0
                    for page in pages:
                        call(page, 'multiplayerSmoke.ticks(1)')
                for page in pages:
                    assert call(page, 'kind=>multiplayerSmoke.endingSetup(kind)', ending)
                    call(page, 'multiplayerSmoke.ticks(1)')
                case['entered'] = [state(page) for page in pages]
                for page in pages:
                    call(page, 'multiplayerSmoke.ticks(90)')
                case['afterTicks'] = [state(page) for page in pages]
                for entered, after in zip(case['entered'], case['afterTicks']):
                    assert entered['ending'][0] == 9 and entered['ending'][2] == 1, entered
                    assert after['native'][9] >= entered['native'][9]+90, ('Ending native draw stopped', entered, after)
                    assert after['ending'][6] >= entered['ending'][6]+1500, ('Local clock stopped', entered, after)
                    assert after['ending'][3:5] == entered['ending'][3:5], 'Ending captured gameplay inputs'
                # The production requestAnimationFrame owner must also render
                # after retirement. Only one local player now presses Confirm.
                for page in pages:
                    call(page, 'multiplayerSmoke.startLoop()')
                pages[0].wait_for_timeout(1100)
                call(pages[0], "multiplayerSmoke.key('KeyZ',true)")
                pages[0].wait_for_timeout(500)
                case['localInput'] = [state(page) for page in pages]
                assert case['localInput'][0]['ending'][10] & 1, 'Local Ending confirm was not sampled'
                assert all(not row['ending'][10] & 1 for row in case['localInput'][1:]), 'Ending input crossed seats'
                call(pages[0], "multiplayerSmoke.key('KeyZ',false)")
                case['afterRaf'] = [state(page) for page in pages]
                for n, page in enumerate(pages):
                    call(page, 'multiplayerSmoke.stopLoop()')
                    before, after = case['afterTicks'][n], case['afterRaf'][n]
                    assert after['native'][9] > before['native'][9]+10, ('rAF ending draw stopped', before, after)
                    assert after['ending'][6] > before['ending'][6], 'Local ending time did not advance'
                    assert after['ending'][3:5] == before['ending'][3:5], 'rAF sent gameplay input after retirement'
                    assert after['native'][2] == 0, after
                    target = args.output.with_name(f'{args.output.stem}-{players}p-{ending}-{n}.png')
                    page.screenshot(path=str(target))
                # Pointer tap confirmation uses the same physical SDL polling
                # after retirement, independently of the other local players.
                call(pages[1], '''()=>{
                    multiplayerSmoke.touchConfigure();
                    multiplayerSmoke.touch(0,17,.5,.5);multiplayerSmoke.touch(2,17,.5,.5);
                    multiplayerSmoke.ticks(1);
                }''')
                case['localTouch'] = state(pages[1])
                assert case['localTouch']['ending'][10] & 1, 'Local Ending tap was not sampled'
                if players == 2 and ending == 2:
                    call(pages[0], "multiplayerSmoke.key('ControlLeft',true)")
                    for _ in range(40):
                        if state(pages[0])['ending'][0] != 9:
                            break
                        call(pages[0], 'multiplayerSmoke.ticks(60)')
                    call(pages[0], "multiplayerSmoke.key('ControlLeft',false)")
                    case['results'] = state(pages[0])
                    assert case['results']['ending'][0] == 6, ('Ending did not reach native results', case['results'])
                    call(pages[0], 'multiplayerSmoke.ticks(20)')
                    assert state(pages[0])['native'][9] >= case['results']['native'][9]+20
                    # Use physical local Confirm through the real high-score,
                    # statistics, slot and name-entry screens after retirement.
                    for _ in range(32):
                        call(pages[0], "multiplayerSmoke.key('KeyZ',true)")
                        call(pages[0], 'multiplayerSmoke.ticks(1)')
                        call(pages[0], "multiplayerSmoke.key('KeyZ',false)")
                        call(pages[0], 'multiplayerSmoke.ticks(35)')
                        if call(pages[0], 'multiplayerSmoke.savedReplays()'):
                            break
                    case['savedReplays'] = call(pages[0], 'multiplayerSmoke.savedReplays()')
                    assert case['savedReplays'] and all(row['valid'] for row in case['savedReplays']), ('Native result Replay was not saved', state(pages[0]))
                assert not report['errors'], report['errors']
                case['passed'] = True
                print(f'th08 {players}P ending {ending}: PASS', flush=True)
            except BaseException:
                case['failedState'] = [state(page) for page in pages]
                for n, page in enumerate(pages):
                    page.screenshot(path=str(args.output.with_name(f'{args.output.stem}-failure-{n}.png')))
                raise
            finally:
                for page in pages:
                    page.close()
        report['passed'] = True
    except BaseException as error:
        report['failure'] = str(error)
        raise
    finally:
        browser.close()
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
