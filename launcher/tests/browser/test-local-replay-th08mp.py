"""Play existing native all-seat recordings through the production shell.
This is not a transport or retail determinism gate."""
import argparse,json
from pathlib import Path
from playwright.sync_api import sync_playwright
p=argparse.ArgumentParser();p.add_argument('--url',required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--recordings',type=Path,required=True);a=p.parse_args()
recordings=sorted(a.recordings.glob('*.rpyx'));assert recordings,'Provide real native TH08 MP recordings'
report={'passed':False,'scope':'Existing native all-seat files and production local Replay controls; no transport or retail determinism claim','cases':[]}
def call(page,js,arg=None):return page.evaluate(js,arg)
def request(page,**fields):return call(page,'fields=>host.request("playback",fields)',fields)
latest="host.events.filter(e=>e.event==='runtime-info'&&e.playback).at(-1)?.playback"
with sync_playwright() as pw:
 browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader']);errors=[]
 try:
  for recording_file in recordings:
   context=browser.new_context(service_workers='block')
   try:
    viewer=context.new_page();viewer.on('pageerror',lambda e:errors.append(str(e)));viewer.goto(a.url+'host.html');call(viewer,'host.open()');viewer.wait_for_function('host.ready()',timeout=120000)
    call(viewer,'v=>host.request("write",v)',{'path':'replay/th8_01.rpyx','bytes':list(recording_file.read_bytes())})
    call(viewer,'host.request("configure",{music:"none",options:{runtimeVariant:"multiplayer",replayViewer:true,touchEnabled:false}})');call(viewer,'host.request("launch")')
    saved=call(viewer,'async()=>{const result={};for(const path of ["score.dat","th08.cfg"]){try{result[path]=(await host.request("read",{path})).bytes;}catch{result[path]=null;}}return result;}')
    metadata=request(viewer,action='inspect',path='replay/th8_01.rpyx')['description'];stage=metadata['stages'][0]['id'];request(viewer,action='open',path='replay/th8_01.rpyx',stage=stage)
    viewer.wait_for_function(f'(()=>{{const p={latest};return p&&(p.frame>=60||p.ended);}})()',timeout=60000);request(viewer,action='pause');before=call(viewer,latest)['frame'];viewer.wait_for_timeout(200);assert call(viewer,latest)['frame']==before
    target=min(20,max(0,call(viewer,latest)['total']-1));request(viewer,action='seek',value=target);viewer.wait_for_function(f'(()=>{{const p={latest};return p&&!p.seeking&&p.frame==={target};}})()',timeout=60000)
    request(viewer,action='rate',value=4);request(viewer,action='play');viewer.wait_for_function(f'(()=>{{const p={latest};return p&&(p.frame>=120||p.ended);}})()',timeout=60000);request(viewer,action='rate',value=0);viewer.wait_for_function(f'({latest})?.ended',timeout=60000)
    playback=call(viewer,latest);assert playback['frame']==playback['total'],playback
    after=call(viewer,'async()=>{const result={};for(const path of ["score.dat","th08.cfg"]){try{result[path]=(await host.request("read",{path})).bytes;}catch{result[path]=null;}}return result;}');assert after==saved
    assert not any(e.get('event') in ['error','exit'] for e in call(viewer,'host.events'))
    report['cases'].append({'file':str(recording_file),'passed':True,'metadata':metadata,'playback':playback,'savesUnchanged':True});print(f'TH08 {recording_file.name} production workspace: PASS',flush=True)
   finally:context.close()
  assert not errors,errors;report['passed']=True
 except BaseException as error:report['error']=str(error);raise
 finally:browser.close();a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
