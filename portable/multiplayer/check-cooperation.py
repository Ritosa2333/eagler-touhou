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


def grazed_bullet_still_hits(browser):
    page = boot(browser)
    before = status(page)
    assert call(page, 'multiplayerSmoke.fixtureGrazedBulletHit(0)') is True
    after = status(page)
    assert after[9] == 2, after
    page.close()
    return {'case':'grazed-bullet-still-hits','passed':True,'before':before,'after':after}


def death_clear_uses_barrier_owner(browser):
    page = boot(browser)
    assert call(page, 'multiplayerSmoke.fixtureCancelRewardOwner()') is True
    page.close()
    return {'case':'death-clear-uses-barrier-owner','passed':True}


def point_of_collection_owner(browser):
    results=[]
    for collector in (0,1):
        page=boot(browser,[1,0] if collector==0 else [0,1])
        assert call(page,f'multiplayerSmoke.fixturePocSetup({collector})') is True
        call(page,'multiplayerSmoke.ticks(150)')
        value=call(page,'multiplayerSmoke.status()')
        power=[value[10],value[22]]
        assert power[collector]==16 and power[1-collector]==0,(collector,power,value)
        results.append(power);page.close()
    page=boot(browser,[1,1])
    assert call(page,'multiplayerSmoke.fixturePocSetup(2)') is True
    call(page,'multiplayerSmoke.ticks(150)')
    value=call(page,'multiplayerSmoke.status()');power=[value[10],value[22]]
    assert power==[16,0],(power,value)
    results.append(power);page.close()
    return {'case':'point-of-collection-claimant','passed':True,'power':results}


def no_global_power_conversion(browser):
    page=boot(browser)
    assert call(page,'multiplayerSmoke.fixturePowerTypeGuard()') is True
    page.close()
    return {'case':'no-global-power-to-point-conversion','passed':True}


def press(page, key):
    call(page, f"multiplayerSmoke.key('{key}',true)")
    call(page, 'multiplayerSmoke.ticks(2)')
    call(page, f"multiplayerSmoke.key('{key}',false)")


def rescue(browser):
    page = boot(browser)
    before_death = status(page)
    donor_bombs = call(page, 'multiplayerSmoke.status()')[11]
    assert call(page, 'multiplayerSmoke.fixtureDie(1)') == 1
    call(page, 'multiplayerSmoke.ticks(40)')
    initial = status(page)
    assert initial[10] == 1 and initial[14] == 4 and initial[13] == 0, initial
    assert initial[8] == before_death[8], (before_death, initial)
    assert call(page, 'multiplayerSmoke.fixturePlace(0,180,384)') == 1
    assert call(page, 'multiplayerSmoke.fixturePlace(1,171,393,1,-1)') == 1
    assert call(page, 'multiplayerSmoke.commit([4,0])')
    call(page, 'multiplayerSmoke.ticks(89)')
    pending = status(page)
    assert pending[6] == 89 and pending[10] == 1, pending
    call(page, 'multiplayerSmoke.ticks(1)')
    revived = status(page)
    assert revived[10] == 0 and revived[14] == 3 and revived[13] == 1, revived
    assert revived[8] == initial[8] - 1, (initial, revived)
    resources = call(page, 'multiplayerSmoke.status()')
    assert resources[11] == donor_bombs and resources[23] == 0 and resources[22] == 64, resources
    call(page, 'multiplayerSmoke.ticks(30)')
    assert status(page)[8] == revived[8], 'holding focus repeated life transfer'
    assert call(page, 'multiplayerSmoke.fixtureGrazedBulletHit(1)')
    call(page, 'multiplayerSmoke.ticks(40)')
    assert call(page, 'multiplayerSmoke.status()')[23] == 2, 'ordinary respawn must restore base Bombs'
    page.close()
    return {'case': 'native-final-death-and-rescue', 'passed': True,
            'spirit': initial, 'at_89': pending, 'revived': revived}


def wipe_ends_run(browser):
    page = boot(browser)
    positions = call(page, 'multiplayerSmoke.status()')
    assert positions[13] != positions[25], 'pilots spawned on top of each other'
    assert call(page, 'multiplayerSmoke.fixtureDie(0)') == 1
    # Let P1 actually reach Spirit before forcing P2's final death.
    call(page, 'multiplayerSmoke.ticks(40)')
    first = status(page)
    assert first[5] == 1 and first[10] == 0, first
    assert call(page, 'multiplayerSmoke.fixtureDie(1)') == 1
    call(page, 'multiplayerSmoke.ticks(40)')
    spirit = status(page)
    assert spirit[5] == spirit[10] == 1 and spirit[2] == 10, spirit
    call(page, 'multiplayerSmoke.ticks(169)')
    before = status(page)
    assert before[2] == 179 and before[1] == before[3] == 0, before
    call(page, 'multiplayerSmoke.ticks(1)')
    ended = status(page)
    assert ended[2] == 180 and ended[3] == 1, ended
    assert ended[1] == 0, ('multiplayer wipe opened Continue/Retry menu', ended)
    assert ended[4] == 6, ('multiplayer wipe did not target Game Results', ended)
    page.close()
    return {'case': 'full-wipe-ends-without-continue', 'passed': True,
            'spirit': spirit, 'at_179': before, 'at_180': ended,
            'spawn_x': [positions[13], positions[25]]}


def three_player_rescue(browser):
    page = boot(browser, [0, 1, 2])
    before_death = status(page)
    assert call(page, 'multiplayerSmoke.fixtureDie(2)') == 1
    call(page, 'multiplayerSmoke.ticks(40)')
    spirit = status(page)
    assert spirit[15] == 1 and spirit[19] == 4, spirit
    # P3's terminal death must not change either survivor's life stock.
    assert spirit[8] == before_death[8], (before_death, spirit)
    assert spirit[13] == before_death[13], (before_death, spirit)
    assert call(page, 'multiplayerSmoke.fixturePlace(0,180,384)') == 1
    # Both alternatives really compete within the twenty-pixel radius. The
    # spirit must beat a closer living recipient, not just an out-of-range one.
    assert call(page, 'multiplayerSmoke.fixturePlace(1,190,384)') == 1
    assert call(page, 'multiplayerSmoke.fixturePlace(2,171,393,1,-1)') == 1
    assert call(page, 'multiplayerSmoke.commit([4,0,0])')
    call(page, 'multiplayerSmoke.ticks(90)')
    revived = status(page)
    assert revived[15] == 0 and revived[19] == 3 and revived[18] == 1, revived
    assert revived[8] == spirit[8] - 1 and revived[13] == spirit[13], (spirit, revived)
    resources = call(page, 'multiplayerSmoke.status()')
    assert resources[11] == 2 and resources[35] == 0 and resources[34] == 64, resources
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
    for _ in range(5):
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
    assert reused[22]==24 and reused[34]==0,('ordinary reuse retained gift owner',reused)
    page.close()
    return {'case':'gift-released-on-spirit-and-cleared-on-reuse','passed':True,
            'assigned':assigned,'released_items':items,
            'released_power':[released[10],released[22],released[34]],'reused_power':[reused[10],reused[22],reused[34]]}

def separated_power_drops(browser):
    results=[]
    for count in (2,3):
        page=boot(browser,list(range(count)))
        value=call(page,'multiplayerSmoke.status()')
        assert [value[11+12*seat] for seat in range(count)]==[2]*count,value
        for mode in (0,2):
            for kind,reward in ((0,1),(2,8),(4,128)):
                result=call(page,'v=>multiplayerSmoke.fixturePowerDrops(...v)',[kind,mode])
                copies=count if kind!=4 else 1
                assert result[0]==1 and result[1]==copies,(count,kind,mode,result)
                positions=sorted(result[3:3+copies])
                assert all(b-a>=1800 for a,b in zip(positions,positions[1:])),result
                assert result[6:6+copies]==[reward]*copies,result
                assert result[9:12]==[copies*reward,0,0],result
                if mode==0 and kind!=4:
                    assert result[21:21+copies]==[6]*copies,result
                    assert all(v<0 for v in result[18:18+copies]),result
                    assert all(v<34000 for v in result[15:15+copies]),result
                    spread=sorted(result[12:12+copies])
                    assert all(b-a>=800 for a,b in zip(spread,spread[1:])),result
                results.append({'players':count,'kind':kind,'nativeMode':mode,'detail':result})
        page.close()
    return {'case':'roster-sized-upward-power-drops-and-single-item-rewards','passed':True,'results':results}

with sync_playwright() as playwright:
    browser = playwright.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        report['browser']=browser.version
        cases=(separated_power_drops,grazed_bullet_still_hits,death_clear_uses_barrier_owner,point_of_collection_owner,no_global_power_conversion,rescue,three_player_rescue,three_player_power_gift,wipe_ends_run,targeted_item_lifetime,full_item_pool,released_gift_and_reuse)
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
