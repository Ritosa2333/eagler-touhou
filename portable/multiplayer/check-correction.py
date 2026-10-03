"""Late encoded inputs through the native TH08 runtime; no device-output claim."""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--output',type=Path,required=True)
args=parser.parse_args()
report={'passed':False,'scope':'stable-stage native input correction and confirmed native audio commands; transport, startup, physical audio/graphics/files and cross-endpoint canonical state not accepted','identities':[],'cases':[],'errors':[]}
def save():
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
with sync_playwright() as p:
    browser=p.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        for count in (2,3):
            for local in range(count):
                page=browser.new_page()
                try:
                    report['running']={'players':count,'local':local,'phase':'boot'};save()
                    page.on('pageerror',lambda error:report['errors'].append(str(error)))
                    page.on('console',lambda message:print(message.text,flush=True) if message.type=='log' and 'correction' in message.text else None)
                    page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
                    report['identities'].append(page.evaluate('multiplayerSmoke.identity()'))
                    page.evaluate('v=>multiplayerSmoke.start(1234,v.loadouts,v.local)',{'loadouts':list(range(count)),'local':local})
                    page.evaluate('multiplayerSmoke.ticks(120)')
                    for _ in range(3):
                        page.evaluate("multiplayerSmoke.key('KeyZ',true)");page.evaluate('multiplayerSmoke.ticks(2)')
                        page.evaluate("multiplayerSmoke.key('KeyZ',false)");page.evaluate('multiplayerSmoke.ticks(120)')
                    clock=page.evaluate('multiplayerSmoke.audioClockIndependent()')
                    assert clock,'Native audio clock incorrectly follows rewindable game time'
                    routing=page.evaluate('multiplayerSmoke.audioRouting()')
                    assert routing,'Native WAV/MIDI routing or logical music unlock was not preserved'
                    textures=page.evaluate('multiplayerSmoke.textureRestore()')
                    assert textures,'Texture before-write, release/replace or screenshot owner restore failed'
                    report['running']['phase']='native-correction';save()
                    probe=page.evaluate('multiplayerSmoke.correctionProbe()')
                    case={'players':count,'local':local,'audioClockIndependent':clock,'audioRouting':routing,'textureRestore':textures,'probe':probe};report['cases'].append(case);save()
                    assert probe[:3]==[1,1,0] and probe[3:5]==[8,8] and probe[32:36]==[1]*4,case
                    assert probe[6]&8 and probe[38]==1,case
                    assert probe[39]==1 and probe[40]>0 and probe[41]==probe[42]>16 and probe[43]==probe[44] and probe[45]==1,case
                    print('TH08 native correction:',count,'players, local',local,'PASS',flush=True)
                finally:page.close()
        assert not report['errors'],report['errors']
        report['passed']=True;report.pop('running',None)
    except BaseException as error:
        report['error']=str(error);raise
    finally:
        browser.close();save()
