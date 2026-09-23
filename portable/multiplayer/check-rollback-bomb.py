"""Verify an unpredicted remote Bomb edge corrects through title rollback."""
import json
import argparse
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser();parser.add_argument('--url',default='http://127.0.0.1:8138/');parser.add_argument('--output',default='artifacts/multiplayer-tests/browser-rollback-bomb.json');args=parser.parse_args()
URL=args.url;LOADOUTS=[[0,0],[1,1]]
def call(page,expr,arg=None):return page.evaluate(expr,arg) if arg is not None else page.evaluate(expr)
def open_net(context,local):
    page=context.new_page();errors=[];page.on('pageerror',lambda e:errors.append(str(e)));page.goto(URL);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
    call(page,'(x)=>multiplayerSmoke.startNet(x[0],x[1],4,8642,0x778899aa,0x55667788)',[LOADOUTS,local]);return page,errors
def net(page):return call(page,'multiplayerSmoke.netStatus()')
def status(page):return call(page,'multiplayerSmoke.status()')
def canonical(page):return call(page,'multiplayerSmoke.canonical()')
def auth(v):return v[1:28]
def tick(page):return call(page,'multiplayerSmoke.netTicks(1)')
def submit(page,frame,buttons=0):
    value=call(page,'(x)=>multiplayerSmoke.submitRemote(1,x[0],x[1])',[frame,buttons]);assert value in (1,2,3,4),(frame,buttons,value,net(page))
def ready(host,peer):
    for phase in (1,2):
        a=call(host,f'multiplayerSmoke.sessionPacket({phase})');b=call(peer,f'multiplayerSmoke.sessionPacket({phase})')
        assert call(host,'(p)=>multiplayerSmoke.applySession(p)',b)
        assert call(peer,'(p)=>multiplayerSmoke.applySession(p)',a)
        if phase==1:
            assert call(host,'multiplayerSmoke.markReady()');assert call(peer,'multiplayerSmoke.markReady()')
    assert call(host,'multiplayerSmoke.canStart()') and call(peer,'multiplayerSmoke.canStart()')

report={'passed':False,'scope':'TH10 remote Bomb rollback'}
with sync_playwright() as p:
    browser=p.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        contexts=[browser.new_context(service_workers='block') for _ in range(3)]
        peer,ep=open_net(contexts[0],1);reference,er=open_net(contexts[1],0);late,el=open_net(contexts[2],0);ready(reference,peer);ready(late,peer)
        for _ in range(900):
            frame=net(reference)[2];submit(reference,frame);tick(reference)
            lf=net(late)[2];submit(late,lf);tick(late)
            if net(reference)[3]>=19 and net(late)[3]>=19:break
        assert net(reference)[3]==net(late)[3]==19,(net(reference),net(late))
        before=status(late);assert before[23]==80,before
        submit(reference,20,2);tick(reference);tick(late)
        predicted=status(late);assert predicted[23]==80,predicted
        submit(late,20,2);assert net(late)[5]==20,net(late)
        submit(reference,21,0);submit(late,21,0);tick(reference);tick(late)
        corrected=status(late);assert corrected[23]==60,corrected
        assert status(reference)==corrected,(status(reference),corrected)
        assert auth(canonical(reference))==auth(canonical(late)),(canonical(reference),canonical(late))
        while net(reference)[2]<80:
            frame=net(reference)[2];submit(reference,frame);submit(late,frame);tick(reference);tick(late)
            assert status(reference)==status(late),(frame,status(reference),status(late));assert auth(canonical(reference))==auth(canonical(late)),(frame,canonical(reference),canonical(late))
        assert not(ep+er+el),(ep,er,el);report.update(passed=True,frame=net(reference)[3],before=before,predicted=predicted,corrected=corrected)
    finally:
        browser.close();path=Path(args.output);path.parent.mkdir(parents=True,exist_ok=True);path.write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'passed':report['passed'],'frame':report.get('frame')}))
