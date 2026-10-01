"""Actual TH08 Update+authored Draw restore; separate from late-input acceptance."""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright
parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--loadouts',action='store_true')
parser.add_argument('--optimization-oracle',action='store_true',help='Compare legacy Update against optimized replay at every checkpoint')
parser.add_argument('--bomb-lazy',action='store_true',help='Use deferred Bomb snapshots in the fixture')
parser.add_argument('--shot-sparse',action='store_true',help='Use sparse PlayerShot snapshots in the fixture')
parser.add_argument('--record-sparse',action='store_true',help='Use sparse spell-record snapshots in the fixture')
args=parser.parse_args()
report={'passed':False,'scope':'TH08 stable-stage world rollback with native Update and semantic Draw; network correction/output fences not yet accepted','cases':[],'identities':[],'errors':[]}
report['optimizationOracle']=args.optimization_oracle
report['bombLazy']=args.bomb_lazy
report['shotSparse']=args.shot_sparse
report['recordSparse']=args.record_sparse
probe_method='updateOptimizationProbe' if args.optimization_oracle else 'worldJournalProbe'
with sync_playwright() as p:
    browser=p.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        layouts=([0,1],[2,3],[4,5],[6,7],[8,9],[10,11],[0,1,2],[3,4,5],[6,7,8],[9,10,11]) if args.loadouts else ([0,1],[0,1,2])
        scenarios=[(layout,mode) for layout in layouts for mode in ((1,3) if args.loadouts else (0,1,2,3))]
        for layout,mode in scenarios:
            count=len(layout)
            page=browser.new_page()
            try:
                page.on('pageerror',lambda error:report['errors'].append(str(error)))
                page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
                report['identities'].append(page.evaluate('multiplayerSmoke.identity()'))
                page.evaluate('chars=>multiplayerSmoke.start(1234,chars)',layout)
                if args.bomb_lazy:assert page.evaluate('multiplayerSmoke.bombLazy(true)')==1
                if args.shot_sparse:assert page.evaluate('multiplayerSmoke.shotSparse(true)')==1
                if args.record_sparse:assert page.evaluate('multiplayerSmoke.recordSparse(true)')==1
                page.evaluate('multiplayerSmoke.ticks(120)')
                for _ in range(3):
                    page.evaluate("multiplayerSmoke.key('KeyZ',true)");page.evaluate('multiplayerSmoke.ticks(2)')
                    page.evaluate("multiplayerSmoke.key('KeyZ',false)");page.evaluate('multiplayerSmoke.ticks(120)')
                page.evaluate('buttons=>multiplayerSmoke.commit(buttons)',[4 if mode==1 else 0]*count)
                page.evaluate('multiplayerSmoke.ticks(60)')
                state=page.evaluate('multiplayerSmoke.status()')
                if layout==[0,1] and mode==0 and args.record_sparse:
                    spell=page.evaluate('multiplayerSmoke.spellRecordProbe()')
                    report['spellRecordProbe']=spell
                    assert spell and spell[0]==spell[2]==spell[3]==1,spell
                if layout==[0,1] and mode==0:
                    title=page.evaluate('multiplayerSmoke.titleSpellsProbe()')
                    report['titleSpellsProbe']=title
                    assert title and title[0]==1,title
                probe=page.evaluate(f'mode=>multiplayerSmoke.{probe_method}(mode)',mode)
                case={'players':count,'loadouts':layout,'mode':['shooting','focused-bombs','native-death','unfocused-bombs'][mode],'nativeState':state,'probe':probe,'later':[]}
                report['cases'].append(case)
                assert probe[:3]==[1,1,0] and probe[32:36]==[1]*4,case
                if mode in (1,3):
                    assert probe[51:51+count]==[1]*count,case
                # Exercise the active spell/death lifetime, not just its first
                # allocation tick. Each probe still runs original Update+Draw.
                for advance in (24,60):
                    page.evaluate('n=>multiplayerSmoke.ticks(n)',advance)
                    later=page.evaluate(f'multiplayerSmoke.{probe_method}(0)')
                    case['later'].append({'advance':advance,'probe':later})
                    assert later[:3]==[1,1,0] and later[32:36]==[1]*4,case
                print('TH08 world journal:',layout,case['mode'],'PASS max checkpoint bytes',max([probe[3]]+[x['probe'][3] for x in case['later']]),flush=True)
            finally:page.close()
        assert not report['errors'],report['errors'];report['passed']=True
    except BaseException as error:
        report['error']=str(error);raise
    finally:
        browser.close();args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
