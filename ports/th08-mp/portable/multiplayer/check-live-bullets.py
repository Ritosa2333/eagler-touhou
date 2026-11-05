"""Complete Bullet-byte restore through native clear, reuse and transform writers."""
import argparse,json
from pathlib import Path
from playwright.sync_api import sync_playwright
p=argparse.ArgumentParser();p.add_argument('--url',required=True);p.add_argument('--output',type=Path,required=True);args=p.parse_args()
report={'passed':False}
with sync_playwright() as playwright:
    browser=playwright.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        page=browser.new_page();page.goto(args.url)
        page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
        report['identity']=page.evaluate('multiplayerSmoke.identity()')
        page.evaluate('multiplayerSmoke.start(1234,[0,1])');page.evaluate('multiplayerSmoke.ticks(120)')
        for _ in range(3):
            page.evaluate("multiplayerSmoke.key('KeyZ',true)");page.evaluate('multiplayerSmoke.ticks(2)')
            page.evaluate("multiplayerSmoke.key('KeyZ',false)");page.evaluate('multiplayerSmoke.ticks(120)')
        report['probe']=page.evaluate('multiplayerSmoke.liveBulletProbe()')
        assert report['probe'][:4]==[1,1,0,5] and report['probe'][4:9]==[1]*5,report
        report['passed']=True
        print('PASS native Bullet full-byte oracle: reuse, extra transform, nearby/global ECL clear, Bullet clear',flush=True)
    except BaseException as error:
        report['error']=str(error);raise
    finally:
        browser.close();args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2))
