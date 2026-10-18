"""Actual TH10 Adonis game/RAF with a deterministic local relay substitute.

This verifies scheduling through the real WASM; it is not public RTC evidence.
"""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser()
parser.add_argument('--url',default='http://127.0.0.1:8138/')
parser.add_argument('--output',default='artifacts/multiplayer-tests/player-cadence.json')
args=parser.parse_args()
stub='''()=>{
 window.RTCPeerConnection=undefined;
 class LocalSocket {
  static OPEN=1;static CONNECTING=0;static CLOSING=2;static CLOSED=3;
  constructor(url){
   this.url=new URL(url);this.seat=Number(this.url.searchParams.get('player'));
   this.readyState=0;this.bufferedAmount=0;this.signal=this.url.searchParams.has('signal');
   if(!this.signal){this.channel=new BroadcastChannel('th10-cadence-relay');
    this.channel.onmessage=event=>{const p=event.data;if(p.from===this.seat||p.to!==null&&p.to!==this.seat)return;
     this.onmessage?.({data:new Uint8Array(p.bytes).buffer});};}
   setTimeout(()=>{this.readyState=1;this.onopen?.();
    this.onmessage?.({data:JSON.stringify({type:'route',mode:'relay'})});},0);
  }
  send(value){
   if(this.signal||typeof value==='string')return;
   let bytes=Array.from(new Uint8Array(value.buffer||value,value.byteOffset||0,value.byteLength));
   const to=bytes[0]===0xe7?bytes[1]:null;if(to!==null)bytes=bytes.slice(2);
   const packet={from:this.seat,to,bytes};
   const delay=Math.max(0,(window.__cadenceWireHoldUntil||0)-performance.now());
   if(delay)setTimeout(()=>this.channel?.postMessage(packet),delay);else this.channel?.postMessage(packet);
  }
  close(){this.readyState=3;this.channel?.close();}
 }
 window.WebSocket=LocalSocket;
}'''
report={'passed':False,'scope':'TH10 native Adonis Delay, actual browser RAF; local relay substitute; no public RTC claim','cases':[]}
with sync_playwright() as playwright:
 browser=playwright.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader','--disable-background-timer-throttling','--disable-renderer-backgrounding'])
 try:
  for count in (2,3):
   context=browser.new_context();context.add_init_script('('+stub+')()')
   pages=[context.new_page() for _ in range(count)]
   errors=[]
   for page in pages:
    page.on('pageerror',lambda error:errors.append(str(error)))
    page.goto(args.url);page.wait_for_function('window.multiplayerSmoke!==undefined',timeout=120000)
   loadouts=[[0,0],[1,1],[0,2]][:count]
   for seat,page in enumerate(pages):
    page.evaluate('(x)=>multiplayerSmoke.startAdonis(...x)',[loadouts,seat])
    assert page.evaluate("multiplayerSmoke.connect('ws://localhost/room?room=cadence&run=1')")
   for seat,page in enumerate(pages):
    page.evaluate('''seat=>{window.cadenceResult=null;window.cadenceError=null;
      multiplayerSmoke.playerRaf(90,seat===0?20:1000,seat===0?40:1000)
      .then(result=>window.cadenceResult=result).catch(error=>window.cadenceError=String(error));}''',seat)
   results=[]
   for page in pages:
    page.wait_for_function('window.cadenceResult||window.cadenceError',timeout=60000)
    assert page.evaluate('window.cadenceError') is None,page.evaluate('window.cadenceError')
    results.append(page.evaluate('window.cadenceResult'))
   assert not errors,errors
   for result in results:
    assert max(sample['advanced'] for sample in result['samples'])<=1,result
    assert result['net'][3]==90,result['net']
   assert results[0]['cpuStalled'] and results[0]['wireStalled']
   report['cases'].append({'players':count,'results':results,'maxForwardFramesPerCallback':1,'cpuStallMs':350,'wireStallMs':350})
   context.close()
  report['passed']=True
 finally:
  browser.close()
  Path(args.output).parent.mkdir(parents=True,exist_ok=True)
  Path(args.output).write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'passed':report['passed'],'cases':len(report['cases'])}))
