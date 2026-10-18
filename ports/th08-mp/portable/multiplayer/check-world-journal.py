"""Actual TH08 Update+authored Draw restore; separate from late-input acceptance."""
import argparse
import json
import os
from pathlib import Path
from playwright.sync_api import sync_playwright
def browser_options(args):
    launch_args=['--ignore-gpu-blocklist'] if args.native_gpu else ['--enable-unsafe-swiftshader']
    if args.native_gpu and os.name=='nt':launch_args+=['--use-gl=angle','--use-angle=d3d11']
    return {'headless':not args.headed,'channel':None if args.browser_channel=='default' else args.browser_channel,'args':launch_args}

def game_renderer(page):
    actual=page.evaluate("""()=>{
      const canvas=document.querySelector('canvas'),gl=canvas?.getContext('webgl2');
      if(!gl)return null;
      const ext=gl.getExtension('WEBGL_debug_renderer_info');
      return {unmasked:!!ext,renderer:gl.getParameter(ext?ext.UNMASKED_RENDERER_WEBGL:gl.RENDERER),
        vendor:gl.getParameter(ext?ext.UNMASKED_VENDOR_WEBGL:gl.VENDOR),version:gl.getParameter(gl.VERSION)};
    }""")
    renderer=(actual or {}).get('renderer','').lower()
    verified=bool(actual and actual['unmasked'] and renderer and
      not any(name in renderer for name in ('swiftshader','llvmpipe','software','basic render','warp')))
    return {'actualGL':actual,'nativeGpuVerified':verified}

def save_report(output,report):
    output.parent.mkdir(parents=True,exist_ok=True)
    temporary=output.with_name(output.name+'.'+str(os.getpid())+'.tmp')
    temporary.write_text(json.dumps(report,indent=2),encoding='utf-8')
    temporary.replace(output)

parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--browser-channel',choices=('default','chromium','chrome'),default='default')
parser.add_argument('--headed',action='store_true',help='Use a visible desktop browser window')
parser.add_argument('--native-gpu',action='store_true',help='Require the actual game WebGL context to expose a non-software renderer')
parser.add_argument('--loadouts',action='store_true')
parser.add_argument('--players',type=int,choices=(2,3),help='Select one player count for a targeted diagnostic')
parser.add_argument('--scenario',choices=('all','shooting','focused-bombs','native-death','unfocused-bombs'),default='all')
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
report['selectedScenario']=args.scenario
report['selectedPlayers']=args.players
report['browserChannel']=args.browser_channel
report['headed']=args.headed
report['nativeGpu']=args.native_gpu
report['completedCases']=0
probe_method='updateOptimizationProbe' if args.optimization_oracle else 'worldJournalProbe'
save_report(args.output,report)
with sync_playwright() as p:
    browser=None
    try:
        options=browser_options(args)
        browser=p.chromium.launch(**options)
        report['browser']={'version':browser.version,'launchArgs':options['args']}
        layouts=([0,1],[2,3],[4,5],[6,7],[8,9],[10,11],[0,1,2],[3,4,5],[6,7,8],[9,10,11]) if args.loadouts else ([0,1],[0,1,2])
        scenarios=[(layout,mode) for layout in layouts for mode in ((1,3) if args.loadouts else (0,1,2,3))]
        scenario_names=('shooting','focused-bombs','native-death','unfocused-bombs')
        scenarios=[(layout,mode) for layout,mode in scenarios if (args.players is None or len(layout)==args.players) and (args.scenario=='all' or scenario_names[mode]==args.scenario)]
        if not scenarios:raise ValueError('No selected world-journal scenarios')
        report['totalCases']=len(scenarios)
        save_report(args.output,report)
        for layout,mode in scenarios:
            count=len(layout)
            page=browser.new_page()
            try:
                page.on('pageerror',lambda error:report['errors'].append(str(error)))
                page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
                identity=page.evaluate('multiplayerSmoke.identity()')
                identity.update({'players':count,'loadouts':layout,'mode':scenario_names[mode]})
                report['identities'].append(identity)
                page.evaluate('chars=>multiplayerSmoke.start(1234,chars)',layout)
                identity.update(game_renderer(page))
                save_report(args.output,report)
                if args.native_gpu and not identity['nativeGpuVerified']:raise AssertionError('Native game WebGL renderer not verified')
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
                case={'passed':False,'players':count,'loadouts':layout,'mode':['shooting','focused-bombs','native-death','unfocused-bombs'][mode],'nativeState':state,'probe':probe,'later':[]}
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
                case['passed']=True
                report['completedCases']+=1
                # A killed later case cannot erase the completed raw oracle.
                # Overall passed stays false until all scenarios finish.
                save_report(args.output,report)
                print('TH08 world journal:',layout,case['mode'],'PASS max checkpoint bytes',max([probe[3]]+[x['probe'][3] for x in case['later']]),flush=True)
            finally:page.close()
        assert not report['errors'],report['errors'];report['passed']=True
    except BaseException as error:
        report['error']=str(error);raise
    finally:
        save_report(args.output,report)
        if browser is not None:browser.close()
