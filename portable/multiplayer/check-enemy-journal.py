"""Real TH08 ECL owner rollback: async lifetime, slot reuse, mutable bytecode."""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright
parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--output',type=Path,required=True)
args=parser.parse_args()
report={'passed':False,'scope':'TH08 native ECL, screen callbacks and fixed pools/geometry journals; not full-world rollback acceptance'}
with sync_playwright() as p:
    browser=p.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        page=browser.new_page();page.goto(args.url)
        page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
        report['identity']=page.evaluate('multiplayerSmoke.identity()')
        report['probe']=page.evaluate('multiplayerSmoke.enemyJournalProbe()')
        assert report['probe'][0:3]==[1,1,0],report['probe']
        assert report['probe'][4:]==[1]*6,report['probe']
        page.evaluate('multiplayerSmoke.start(1234,[0,1])')
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
        browser.close();args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
