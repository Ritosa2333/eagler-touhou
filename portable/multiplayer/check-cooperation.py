"""TH08 native 2P/3P cooperation fixtures; requires --multiplayer-fixtures at :8140.

The fixture only sets explicit lives/power/positions and invokes native death.
All deathbomb, spirit, input, rescue, items, retry-menu and resource work runs in-game.
This is not transport, rollback or cross-device acceptance.
"""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8140/')
parser.add_argument('--output', default='artifacts/multiplayer-tests/cooperation-native.json')
parser.add_argument('--case',default='all')
args = parser.parse_args()
report = {'passed':False,'scope': 'TH08 native 2P/3P cooperation; no transport or rollback', 'cases': [],'errors':[]}


def call(page, expression, arg=None):
    return page.evaluate(expression, arg)


def boot(browser, loadouts=None):
    page = browser.new_page(viewport={'width': 640, 'height': 480})
    page.on('pageerror',lambda error:report['errors'].append(str(error)))
    page.goto(args.url)
    page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
    report.setdefault('runtimeIdentities',[]).append(call(page,'multiplayerSmoke.identity()'))
    call(page, '(seats)=>multiplayerSmoke.start(1234,seats)', loadouts or [0, 1])
    call(page, 'multiplayerSmoke.ticks(120)')
    for _ in range(3):
        call(page, "multiplayerSmoke.key('KeyZ',true)")
        call(page, 'multiplayerSmoke.ticks(2)')
        call(page, "multiplayerSmoke.key('KeyZ',false)")
        call(page, 'multiplayerSmoke.ticks(120)')
    assert call(page, 'multiplayerSmoke.fixtureStatus()')[0] == 1
    return page


def status(page):
    return call(page, 'multiplayerSmoke.fixtureStatus()')


def press(page, key):
    call(page, f"multiplayerSmoke.key('{key}',true)")
    call(page, 'multiplayerSmoke.ticks(2)')
    call(page, f"multiplayerSmoke.key('{key}',false)")


def rescue(browser):
    page = boot(browser)
    assert call(page, 'multiplayerSmoke.fixtureDie(1)') == 1
    call(page, 'multiplayerSmoke.ticks(40)')
    initial = status(page)
    assert initial[10] == 1 and initial[14] == 4 and initial[13] == 0, initial
    assert call(page, 'multiplayerSmoke.fixturePlace(0,180,384)') == 1
    assert call(page, 'multiplayerSmoke.fixturePlace(1,171,393,1,-1)') == 1
    assert call(page, 'multiplayerSmoke.commit([4,0])')
    call(page, 'multiplayerSmoke.ticks(89)')
    pending = status(page)
    assert pending[6] == 89 and pending[10] == 1, pending
    call(page, 'multiplayerSmoke.ticks(1)')
    revived = status(page)
    assert revived[10] == 0 and revived[14] == 3 and revived[13] == 0, revived
    assert revived[8] == initial[8] - 1, (initial, revived)
    call(page, 'multiplayerSmoke.ticks(30)')
    assert status(page)[8] == revived[8], 'holding focus repeated life transfer'
    page.close()
    return {'case': 'native-final-death-and-rescue', 'passed': True,
            'spirit': initial, 'at_89': pending, 'revived': revived}


def wipe_retry(browser):
    page = boot(browser)
    assert call(page, 'multiplayerSmoke.fixtureDie(0)') == 1
    assert call(page, 'multiplayerSmoke.fixtureDie(1)') == 1
    call(page, 'multiplayerSmoke.ticks(40)')
    spirit = status(page)
    assert spirit[5] == spirit[10] == 1 and spirit[2] == 10, spirit
    call(page, 'multiplayerSmoke.ticks(169)')
    before = status(page)
    assert before[2] == 179 and before[1] == before[3] == 0, before
    call(page, 'multiplayerSmoke.ticks(1)')
    retry = status(page)
    assert retry[2] == 180 and retry[1] == retry[3] == 1, retry
    call(page, 'multiplayerSmoke.ticks(70)')
    press(page, 'ArrowUp')
    call(page, 'multiplayerSmoke.ticks(6)')
    press(page, 'KeyZ')
    call(page, 'multiplayerSmoke.ticks(120)')
    continued = status(page)
    assert continued[1] == continued[2] == continued[3] == 0, continued
    assert continued[5] == continued[10] == 0, continued
    assert continued[8] == continued[13] == 2, continued
    positions = call(page, 'multiplayerSmoke.status()')
    assert positions[13] != positions[25], 'pilots respawned on top of each other'
    page.close()
    return {'case': 'full-wipe-and-native-retry', 'passed': True,
            'spirit': spirit, 'at_179': before, 'at_180': retry, 'continued': continued,
            'spawn_x': [positions[13], positions[25]]}


def three_player_rescue(browser):
    page = boot(browser, [0, 1, 2])
    assert call(page, 'multiplayerSmoke.fixtureDie(2)') == 1
    call(page, 'multiplayerSmoke.ticks(40)')
    spirit = status(page)
    assert spirit[15] == 1 and spirit[19] == 4, spirit
    assert call(page, 'multiplayerSmoke.fixturePlace(0,180,384)') == 1
    # Both alternatives really compete within the twenty-pixel radius. The
    # spirit must beat a closer living recipient, not just an out-of-range one.
    assert call(page, 'multiplayerSmoke.fixturePlace(1,190,384)') == 1
    assert call(page, 'multiplayerSmoke.fixturePlace(2,171,393,1,-1)') == 1
    assert call(page, 'multiplayerSmoke.commit([4,0,0])')
    call(page, 'multiplayerSmoke.ticks(90)')
    revived = status(page)
    assert revived[15] == 0 and revived[19] == 3 and revived[18] == 0, revived
    assert revived[8] == spirit[8] - 1 and revived[13] == spirit[13], (spirit, revived)
    page.close()
    return {'case': 'three-player-spirit-priority', 'passed': True,
            'spirit': spirit, 'revived': revived}


def three_player_power_gift(browser):
    page = boot(browser, [0, 1, 2])
    for seat, x, power in ((0, 180, 20), (1, 190, 50), (2, 170, 0)):
        assert call(page, f'multiplayerSmoke.fixturePlace({seat},{x},384)') == 1
        assert call(page, f'multiplayerSmoke.fixturePower({seat},{power})') == 1
    initial = call(page, 'multiplayerSmoke.status()')
    assert [initial[10], initial[22], initial[34]] == [20, 50, 0], initial
    for _ in range(8):
        assert call(page, 'multiplayerSmoke.commit([1,0,0])')
        call(page, 'multiplayerSmoke.ticks(1)')
        assert call(page, 'multiplayerSmoke.commit([0,0,0])')
        call(page, 'multiplayerSmoke.ticks(1)')
    donated = call(page, 'multiplayerSmoke.status()')
    assert donated[10] == 0 and donated[22] == 50, (initial, donated)
    call(page, 'multiplayerSmoke.ticks(45)')
    received = call(page, 'multiplayerSmoke.status()')
    assert received[34] == 20 and received[22] == 50, (donated, received)
    page.close()
    return {'case': 'three-player-targeted-power-gift', 'passed': True,
            'initial_power': [initial[10], initial[22], initial[34]],
            'donated_power': [donated[10], donated[22], donated[34]],
            'received_power': [received[10], received[22], received[34]]}


def targeted_item_lifetime(browser):
    page=boot(browser,[0,1,2])
    for seat,x,y in [(0,350,416),(1,180,340),(2,220,384)]:
        assert call(page,f'multiplayerSmoke.fixturePlace({seat},{x},{y})')
        assert call(page,f'multiplayerSmoke.fixturePower({seat},0)')
    assert call(page,'multiplayerSmoke.fixtureItems(5)')
    assert call(page,'multiplayerSmoke.fixtureItems(1,2)')
    assert call(page,'multiplayerSmoke.fixtureItems(2)')
    assert call(page,'multiplayerSmoke.fixtureItems(3)')
    call(page,'multiplayerSmoke.ticks(25)')
    value=call(page,'multiplayerSmoke.status()')
    assert value[22]==0 and value[34]==20,('gift stolen after native cancel/collect',value)
    page.close()
    return {'case':'gift-affinity-survives-native-homing-changes','passed':True,'power':[value[10],value[22],value[34]]}

def full_item_pool(browser):
    page=boot(browser)
    assert call(page,'multiplayerSmoke.fixturePlace(0,180,384)')
    assert call(page,'multiplayerSmoke.fixturePlace(1,190,384)')
    assert call(page,'multiplayerSmoke.fixturePower(0,20)')
    assert call(page,'multiplayerSmoke.fixturePower(1,0)')
    assert call(page,'multiplayerSmoke.fixtureItems(4)')
    before=call(page,'multiplayerSmoke.fixtureItemStatus()')
    assert before[1]==2091,before
    assert call(page,'multiplayerSmoke.fixtureItems(1,1)')==0
    assert call(page,'multiplayerSmoke.fixtureItemStatus()')==before,'failed gift changed pool cursor, items or RNG'
    for _ in range(8):
        assert call(page,'multiplayerSmoke.commit([1,0])');call(page,'multiplayerSmoke.ticks(1)')
        assert call(page,'multiplayerSmoke.commit([0,0])');call(page,'multiplayerSmoke.ticks(1)')
    value=call(page,'multiplayerSmoke.status()')
    assert value[10]==20 and value[22]==0,('failed gift debited resources',value)
    page.close()
    return {'case':'full-pool-gift-is-transactional','passed':True,'before':before,'power':[value[10],value[22]]}

def released_gift_and_reuse(browser):
    page=boot(browser,[0,1,2])
    for seat,x,y in [(0,350,416),(1,180,340),(2,220,384)]:
        assert call(page,f'multiplayerSmoke.fixturePlace({seat},{x},{y})')
        assert call(page,f'multiplayerSmoke.fixturePower({seat},0)')
    assert call(page,'multiplayerSmoke.fixtureItems(5)')
    assert call(page,'multiplayerSmoke.fixtureItems(1,2)')
    assert call(page,'multiplayerSmoke.fixtureDie(2)')
    assigned=call(page,'multiplayerSmoke.fixtureItemStatus()')
    assert assigned[5]==assigned[8]==6,assigned
    call(page,'multiplayerSmoke.ticks(40)')
    assert status(page)[15]==1,'recipient did not enter native Spirit state'
    assert call(page,'multiplayerSmoke.fixtureItems(3)')
    call(page,'multiplayerSmoke.ticks(45)')
    released=call(page,'multiplayerSmoke.status()')
    items=call(page,'multiplayerSmoke.fixtureItemStatus()')
    # Native final death also drops a full-power item. Preserve that reward;
    # inspect these six gift slots and their affinity, not total power alone.
    assert items[5]==items[8]==0 and released[22]>=20 and released[34]==0,(items,released)
    # Reusing the same cleared pool for an ordinary item must not inherit a
    # prior donation's affinity, including a now unavailable old recipient.
    assert call(page,'multiplayerSmoke.fixtureItems(5)')
    assert call(page,'multiplayerSmoke.fixturePower(1,0)')
    assert call(page,'multiplayerSmoke.fixtureItems(6)')
    call(page,'multiplayerSmoke.ticks(15)')
    reused=call(page,'multiplayerSmoke.status()')
    assert reused[22]==8 and reused[34]==0,('ordinary reuse retained gift owner',reused)
    page.close()
    return {'case':'gift-released-on-spirit-and-cleared-on-reuse','passed':True,
            'assigned':assigned,'released_items':items,
            'released_power':[released[10],released[22],released[34]],'reused_power':[reused[10],reused[22],reused[34]]}

with sync_playwright() as playwright:
    browser = playwright.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        report['browser']=browser.version
        cases=(rescue,three_player_rescue,three_player_power_gift,wipe_retry,targeted_item_lifetime,full_item_pool,released_gift_and_reuse)
        selected=[case for case in cases if args.case=='all' or case.__name__==args.case]
        assert selected,'unknown native cooperation case'
        for case in selected:
            result = case(browser)
            report['cases'].append(result)
            print(result['case'] + ': PASS', flush=True)
        assert not report['errors'],report['errors']
        report['passed']=True
    except BaseException as error:
        report['failure']=str(error)
        raise
    finally:
        browser.close()
        target = Path(args.output)
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
