"""Three-player browser rollback proof for TH10 multiplayer."""
import json
import argparse
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser();parser.add_argument('--url',default='http://127.0.0.1:8138/');parser.add_argument('--output',default='artifacts/multiplayer-tests/browser-rollback-3p.json');args=parser.parse_args()
URL=args.url
LOADOUTS=[[0,0],[1,1],[0,2]]

def call(page,expr,arg=None): return page.evaluate(expr,arg) if arg is not None else page.evaluate(expr)
def open_net(context,local):
    page=context.new_page();errors=[];page.on('pageerror',lambda e:errors.append(str(e)))
    page.goto(URL);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
    call(page,'(x)=>multiplayerSmoke.startNet(x[0],x[1],1,2468,0x33445566,0x11223344)',[LOADOUTS,local])
    return page,errors
def net(page): return call(page,'multiplayerSmoke.netStatus()')
def status(page): return call(page,'multiplayerSmoke.status()')
def canonical(page): return call(page,'multiplayerSmoke.canonical()')
def auth(values): return values[1:28]
def tick(page): return call(page,'multiplayerSmoke.netTicks(1)')
def submit(page,seat,frame,buttons):
    value=call(page,'(x)=>multiplayerSmoke.submitRemote(x[0],x[1],x[2])',[seat,frame,buttons])
    assert value in (1,2,3,4),(seat,frame,buttons,value,net(page))
def ready(host,peers):
    pages=[host,*peers]
    for phase in (1,2):
        packets=[call(page,f'multiplayerSmoke.sessionPacket({phase})') for page in pages]
        for seat,page in enumerate(pages):
            for peer,packet in enumerate(packets):
                if seat!=peer:assert call(page,'(p)=>multiplayerSmoke.applySession(p)',packet)
        if phase==1:
            for page in pages:assert call(page,'multiplayerSmoke.markReady()')
    for page in pages:assert call(page,'multiplayerSmoke.canStart()')

report={'passed':False,'scope':'TH10 3P title rollback'}
with sync_playwright() as p:
    browser=p.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    contexts=[]
    try:
        def make(local):
            context=browser.new_context(service_workers='block');contexts.append(context);return open_net(context,local)
        p1,e1=make(1);p2,e2=make(2);reference,er=make(0);late,el=make(0)
        ready(reference,[p1,p2]);ready(late,[p1,p2])
        for _ in range(900):
            frame=net(reference)[2];submit(reference,1,frame,0x80);submit(reference,2,frame,0x40);tick(reference)
            if net(reference)[3]>=5:break
        assert net(reference)[3]==5,(net(reference),status(reference))
        for _ in range(900):
            tick(late)
            if net(late)[3]>=4:break
        assert net(late)[3]==4,(net(late),status(late))
        for frame in range(5):submit(late,1,frame,0x80);submit(late,2,frame,0x40)
        assert net(late)[5]==0,net(late)
        submit(late,1,5,0x80);submit(late,2,5,0x40);tick(late)
        assert status(reference)==status(late),(status(reference),status(late))
        assert auth(canonical(reference))==auth(canonical(late)),(canonical(reference),canonical(late))
        while net(reference)[2]<140:
            frame=net(reference)[2];assert frame==net(late)[2]
            for page in (reference,late):submit(page,1,frame,0x80);submit(page,2,frame,0x40);tick(page)
            assert status(reference)==status(late),(frame,status(reference),status(late))
            assert auth(canonical(reference))==auth(canonical(late)),(frame,canonical(reference),canonical(late))
        assert not(e1+e2+er+el),(e1,e2,er,el)
        report.update(passed=True,frame=net(reference)[3],status=status(reference),net=net(reference))
    finally:
        browser.close();path=Path(args.output);path.parent.mkdir(parents=True,exist_ok=True);path.write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'passed':report['passed'],'frame':report.get('frame')}))
