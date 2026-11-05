"""Real TH08 ECL owner rollback: async lifetime, slot reuse, mutable bytecode."""
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
args=parser.parse_args()
report={'passed':False,'scope':'TH08 native ECL, screen callbacks and fixed pools/geometry journals; not full-world rollback acceptance'}
report['browserChannel']=args.browser_channel
report['headed']=args.headed
report['nativeGpu']=args.native_gpu
save_report(args.output,report)
with sync_playwright() as p:
    browser=None
    try:
        options=browser_options(args)
        browser=p.chromium.launch(**options)
        report['browser']={'version':browser.version,'launchArgs':options['args']}
        page=browser.new_page();page.goto(args.url)
        page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
        report['identity']=page.evaluate('multiplayerSmoke.identity()')
        report['probe']=page.evaluate('multiplayerSmoke.enemyJournalProbe()')
        assert report['probe'][0:3]==[1,1,0],report['probe']
        assert report['probe'][4:]==[1]*6,report['probe']
        page.evaluate('multiplayerSmoke.start(1234,[0,1])')
        report['identity'].update(game_renderer(page))
        save_report(args.output,report)
        if args.native_gpu and not report['identity']['nativeGpuVerified']:raise AssertionError('Native game WebGL renderer not verified')
        report['screenProbe']=page.evaluate('multiplayerSmoke.screenJournalProbe()')
        assert report['screenProbe'][0:3]==[1,1,0],report['screenProbe']
        assert report['screenProbe'][4:]==[1]*6,report['screenProbe']
        page.evaluate('multiplayerSmoke.ticks(120)')
        for _ in range(3):
            page.evaluate("multiplayerSmoke.key('KeyZ',true)")
            page.evaluate('multiplayerSmoke.ticks(2)')
            page.evaluate("multiplayerSmoke.key('KeyZ',false)")
            page.evaluate('multiplayerSmoke.ticks(120)')
        report['poolsProbe']=page.evaluate('multiplayerSmoke.poolsJournalProbe()')
        assert report['poolsProbe'][0:3]==[1,1,0],report['poolsProbe']
        assert report['poolsProbe'][4:]==[1]*6,report['poolsProbe']
        report['resourcesProbe']=page.evaluate('multiplayerSmoke.resourcesJournalProbe()')
        assert report['resourcesProbe'][0:3]==[1,1,0],report['resourcesProbe']
        assert report['resourcesProbe'][4:]==[1]*6,report['resourcesProbe']
        report['worldProbe']=page.evaluate('multiplayerSmoke.worldJournalProbe()')
        assert report['worldProbe'][0:3]==[1,1,0],report['worldProbe']
        assert report['worldProbe'][32:36]==[1]*4,report['worldProbe']
        report['passed']=True;print(json.dumps(report),flush=True)
    except BaseException as error:
        report['error']=str(error);raise
    finally:
        save_report(args.output,report)
        if browser is not None:browser.close()
