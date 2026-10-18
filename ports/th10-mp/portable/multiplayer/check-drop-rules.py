"""TH10 real native 2P/3P drop quantities, edge separation and exact gifts.
Requires --multiplayer-fixtures; no transport or Replay acceptance.
"""
from pathlib import Path
import argparse,json
from playwright.sync_api import sync_playwright
p=argparse.ArgumentParser();p.add_argument('--url',default='http://127.0.0.1:8138/');p.add_argument('--output',default='artifacts/multiplayer-tests/native-drop-rules.json');args=p.parse_args()
report={'passed':False,'scope':'TH10 actual native World spawn/ItemManager; 2P and 3P; no transport or replay acceptance','cases':[]}
with sync_playwright() as pw:
    browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        for count in (2,3):
            page=browser.new_page();page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
            page.evaluate('s=>multiplayerSmoke.start(s,0,1)',[[0,0],[1,1],[0,2]][:count])
            for _ in range(60):
                status=page.evaluate('multiplayerSmoke.ticks(10)')
                if status[7]>=35:break
            assert status[7]>=35,status
            for kind in (1,4,10,11,7,5,9,100,101):
                for x in (-220,0,220) if kind in (1,4,7,10,11) else (0,):
                    r=page.evaluate('x=>multiplayerSmoke.fixtureDropRules(...x)',[kind,x])
                    expected=2 if count==3 and kind in (1,4,7,10,11) else 1
                    assert r[0]==1 and r[1]==expected,(count,kind,x,r)
                    if expected==2:assert r[2]==0 and r[4]-r[3]>=1800 and r[5]==0,(count,kind,x,r)
                    report['cases'].append({'players':count,'kind':kind,'x':x,'actual':r})
            page.close()
        report['passed']=True
    finally:
        browser.close();Path(args.output).parent.mkdir(parents=True,exist_ok=True);Path(args.output).write_text(json.dumps(report,indent=2)+'\n')
print('TH10 native 2P/3P resource and gift quantities: PASS')

