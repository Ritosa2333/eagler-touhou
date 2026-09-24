"""Native TH08 admission and per-seat keyboard input; no transport/rollback claim."""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--output',type=Path,required=True)
args=parser.parse_args()
report={'passed':False,'scope':'TH08 native frame-zero admission and exact input handoff; no world rollback or transport acceptance','cases':[],'errors':[]}
def call(page,expr,arg=None):return page.evaluate(expr,arg)
def apply(page,packet):assert call(page,'p=>multiplayerSmoke.applyWire(p)',packet)==0
def logical_native(words):
    # 5..8 are the physical touch/controller UI, allowed to update while a
    # simulation frame waits. 0..4 and 9 are native state and authored ticks.
    return words[:5]+words[9:]
with sync_playwright() as p:
    browser=p.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        for count in (2,3):
            pages=[]
            try:
                for seat in range(count):
                    page=browser.new_page();pages.append(page)
                    page.on('pageerror',lambda error:report['errors'].append(str(error)))
                    page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
                    report.setdefault('identities',[]).append(call(page,'multiplayerSmoke.identity()'))
                    call(page,'v=>multiplayerSmoke.start(...v)',[1234,[0,1,2][:count],seat,[0x55667788,0x10203040]])
                    before=call(page,'multiplayerSmoke.nativeStatus()')
                    call(page,'multiplayerSmoke.ticks(6)')
                    assert call(page,'multiplayerSmoke.netStatus()')[3:5]==[0,0xffffffff]
                    after=call(page,'multiplayerSmoke.nativeStatus()')
                    assert logical_native(after)==logical_native(before),('Gate changed native frame/draw state',before,after)
                    assert not call(page,'multiplayerSmoke.markReady()')
                hello=[call(page,'multiplayerSmoke.sessionPacket(1)') for page in pages]
                for seat,page in enumerate(pages):
                    for remote,packet in enumerate(hello):
                        if remote!=seat:apply(page,packet)
                    assert call(page,'multiplayerSmoke.markReady()')
                ready=[call(page,'multiplayerSmoke.sessionPacket(2)') for page in pages]
                for seat,page in enumerate(pages):
                    for remote,packet in enumerate(ready):
                        if remote!=seat:apply(page,packet)
                # Each tick executes the original update + authored Draw. The
                # test transports encoded packets, not a second simulation.
                for frame in range(360):
                    for seat,page in enumerate(pages):
                        buttons=(1|(64 if seat%2 else 128)) if frame>=240 else 0
                        assert call(page,'v=>multiplayerSmoke.capture(...v)',[frame,buttons])
                    for seat,page in enumerate(pages):
                        for target,receiver in enumerate(pages):
                            if target!=seat:apply(receiver,call(page,'v=>multiplayerSmoke.inputPacket(...v)',[target,frame,frame+1]))
                    for page in pages:call(page,'multiplayerSmoke.ticks(1)')
                    if frame in (0,119,239,359):
                        states=[call(page,'multiplayerSmoke.status()') for page in pages]
                        assert all(s[:2]+s[3:]==states[0][:2]+states[0][3:] for s in states),states
                        assert all(call(page,'multiplayerSmoke.netStatus()')[3]==frame+1 for page in pages)
                states=[call(page,'multiplayerSmoke.status()') for page in pages]
                assert states[0][3]==1 and states[0][4]==0 and states[0][7]>120,states
                assert states[0][13]!=states[0][25],'Independent seat movement was lost'
                # One local capture is not permission to mutate the world.
                assert call(pages[0],'multiplayerSmoke.capture(360,1)')
                before=call(pages[0],'multiplayerSmoke.status()')
                native_before=call(pages[0],'multiplayerSmoke.nativeStatus()')
                call(pages[0],'multiplayerSmoke.ticks(8)')
                assert call(pages[0],'multiplayerSmoke.status()')==before
                assert logical_native(call(pages[0],'multiplayerSmoke.nativeStatus()'))==logical_native(native_before)
                assert call(pages[0],'multiplayerSmoke.netStatus()')[3]==360
                assert not call(pages[0],'multiplayerSmoke.commit([1,1])')
                report['cases'].append({'players':count,'passed':True,'logicalFrames':360,'states':states,'stallPreservedUpdateAndDraw':True})
                print(f'TH08 {count}P native session admission/input/stall: PASS',flush=True)
            finally:
                for page in pages:page.close()
        assert not report['errors'],report['errors']
        report['passed']=True
    except BaseException as error:
        report['error']=str(error);raise
    finally:
        browser.close();args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
