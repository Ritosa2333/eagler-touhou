"""Compare unmodified th06/th07 multiplayer Runtime owners through real play.

Requires a built production Runtime, local music/font inputs and Playwright.
The source players use native scripted input. No forced stage, invulnerability
or spectator state fixture is applied. Native same-frame samples are compared
before and after timeline navigation, with real OGG music enabled.
"""
import argparse
import os,sys,threading,json,time,hashlib
from pathlib import Path
from http.server import ThreadingHTTPServer,SimpleHTTPRequestHandler
from urllib.parse import urlencode,urlsplit
root=Path(__file__).resolve().parents[3]
os.environ['EAGLER_WORKSPACE']=str(root);os.environ['EAGLER_RELAY_ROOT']=str(root/'services/relay')
sys.path.insert(0,str(root/'ports/th07/tests'))
import importlib.util
spec=importlib.util.spec_from_file_location('spectator_test',root/'ports/th07/tests/netplay-spectator-stage2-browser.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('game',choices=['th06','th07'])
parser.add_argument('--runtime-directory',type=Path,required=True)
parser.add_argument('--music-directory',type=Path,required=True)
parser.add_argument('--font',type=Path,required=True)
parser.add_argument('--relay',required=True)
parser.add_argument('--players',type=int,choices=[2,3],default=2)
parser.add_argument('--physical-input',action='store_true',help='drive native keyboard input with shooting, movement, Focus and Bomb edges')
parser.add_argument('--until-end',action='store_true',help='observe a native team wipe and consume the confirmed spectator tail')
parser.add_argument('--frames',type=int,default=2400)
parser.add_argument('--timeout',type=int,default=300)
parser.add_argument('--output',type=Path,required=True)
args=parser.parse_args()
assert args.frames>=1200 and args.frames%20==0
relaybase=args.relay.rstrip('/');game=args.game;count=args.players;endframe=args.frames
build=args.runtime_directory.resolve();music=args.music_directory.resolve()
assert (build/(game+'.wasm')).is_file() and args.font.is_file()
host=(root/('ports/'+game+'/tests/netplay-spectator-browser-host.html')).read_text()
bgmdir='/bgm/' if game=='th06' else '/bgm-ogg/'
resources=[{'path':bgmdir+n,'url':'/music/'+n} for n in [game+'_'+str(i).zfill(2)+'.ogg' for i in range(1,8)]]
host=host.replace('music: "none"','music: "ogg"').replace('resources: []','resources: '+json.dumps(resources))
host=host.replace('await send("configure", {','''const w=iframe.contentWindow;let fs=w.FS||w.Module?.FS;
      if(!fs){try{fs=w.eval('FS')}catch{}}
      w.__musicOpens=[];
      if(fs?.open){const open=fs.open;fs.open=function(path,...args){if(typeof path==='string'&&/\\.(?:ogg|wav)$/.test(path))w.__musicOpens.push(path);return open.call(this,path,...args);};}
      await send("configure", {''')
if args.physical_input:
 host=host.replace('netplayPhysicalInput: false','netplayPhysicalInput: true').replace('netplayScriptedInput: true','netplayScriptedInput: false')

class Handler(SimpleHTTPRequestHandler):
 def log_message(self,*args):pass
 def do_GET(self):
  path=urlsplit(self.path).path
  if path in ['/host.html','/setup.html']:
   data=host.encode() if path=='/host.html' else b'<!doctype html>'
   self.send_response(200);self.send_header('Content-Type','text/html');self.send_header('Content-Length',str(len(data)));self.end_headers();self.wfile.write(data)
  else:super().do_GET()
 def translate_path(self,path):
  path=urlsplit(path).path
  if path=='/assets/msgothic.ttc':return str(args.font.resolve())
  if path.startswith('/runtime/'):
   candidate=(build/path[len('/runtime/'):]).resolve()
   return str(candidate if candidate.is_relative_to(build) else build/'__missing__')
  if path.startswith('/music/'):
   candidate=(music/path[len('/music/'):]).resolve()
   return str(candidate if candidate.is_relative_to(music) else build/'__missing__')
  return str(build/'__missing__')
s=ThreadingHTTPServer(('127.0.0.1',0),Handler);threading.Thread(target=s.serve_forever,daemon=True).start()
report={'game':game,'players':count,'scope':'native live stage without gameplay fixtures','physicalInput':args.physical_input,'passed':False,'wasm':hashlib.sha256((build/(game+'.wasm')).read_bytes()).hexdigest(),'samples':[],'errors':[]}
from playwright.sync_api import sync_playwright
try:
 with sync_playwright() as pw:
  b=pw.chromium.launch(headless=True,channel='msedge',args=['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding'])
  try:
   setup=b.new_page();base='http://127.0.0.1:'+str(s.server_port);setup.goto(base+'/setup.html')
   room=game+'mp-natural-'+str(time.time_ns());m.admit_lobby(setup,relaybase,room,'natural_view',count)
   pages=[b.new_page() for _ in range(count+1)]
   for seat,p in enumerate(pages):
    p.add_init_script('''delete globalThis.RTCPeerConnection; addEventListener('message',e=>{const m=e.data||{};if(m.event==='runtime-info'&&m.spectatorPlayback)globalThis.__spectatorPlaybackReport=m.spectatorPlayback;});''');p.on('pageerror',lambda e:report['errors'].append(str(e)))
    if seat==count:pages[0].wait_for_timeout(2000)
    q={'runtimeBuild':'runtime','player':seat if seat<count else 0,'players':count,'frames':12000,
       'relay':relaybase+'/?room='+room+'&run=1&member='+('member_p'+str(seat+1)+'_00000'+str(seat+1) if seat<count else 'member_view_0003')}
    if seat==count:
     q.update(spectator=1,spectatorId='natural_view');q['relay']+='&spectator=natural_view'
    p.goto(base+'/host.html?'+urlencode(q),wait_until='domcontentloaded')
   deadline=time.monotonic()+args.timeout;frame=20;next_log=0;retired_at=None
   while frame<=endframe:
    values=[m.snapshot(p,frame) for p in pages]
    report['latest']=values
    retired=all(not v['active'] and v['confirmed']>=0 for v in values[:count])
    if retired:
     retired_at=retired_at or time.monotonic()
     playback=values[-1]['playback'] or {}
     if args.until_end and playback.get('ended') and playback['position']==playback['received'] and frame>playback['received']:
      report['terminal']={'confirmedEnd':playback['received'],'viewer':playback,'lastCompared':frame-20};break
     assert time.monotonic()-retired_at<10,{'retiredSourcesWithoutCompletedTail':values}

    if args.physical_input:
     for seat,p in enumerate(pages[:count]):
      f=values[seat]['frame'];phase=(f//90+seat)%2
      keys=['KeyZ','ControlLeft', 'ArrowRight' if phase else 'ArrowLeft']
      if (f//120+seat)%2:keys.append('ShiftLeft')
      if 300< f%900 <330:keys.append('KeyX')
      p.evaluate('''keys=>{const old=globalThis.nativeTestKeys||[],w=document.getElementById('runtime').contentWindow;
       const codes={KeyZ:90,KeyX:88,ControlLeft:17,ShiftLeft:16,ArrowRight:39,ArrowLeft:37};
       for(const code of new Set([...old,...keys])){if(old.includes(code)===keys.includes(code))continue;
        w.postMessage({protocol:'eagler-touhou/1',game:EXPECTED_GAME,epoch:1,command:'keyboard',request:'key-'+code,
          code,key:code,keyCode:codes[code],location:code.endsWith('Left')&&!code.startsWith('Arrow')?1:0,
          down:keys.includes(code),inputSource:'physical'},location.origin);}
       globalThis.nativeTestKeys=keys;}'''.replace('EXPECTED_GAME',repr(game)),keys)
    if time.monotonic()>=next_log:
     print('progress:',[(v['frame'],v['confirmed'],v['route']) for v in values],flush=True);next_log=time.monotonic()+10
    assert not report['errors'],report['errors']
    assert not any(v['failed'] or v['hostFailure'] for v in values),values
    if all((v['frame'] if i==count or v['active'] else v['confirmed']+1)>=frame for i,v in enumerate(values)) and min(v['confirmed'] for v in values[:count])>=frame:
     if all(v['hash'] for v in values):
      row={'frame':frame,'hashes':[v['hash'] for v in values]};report['samples'].append(row)
      if len(set(row['hashes']))!=1:
       report['firstDifference']=values;raise AssertionError('native state differs at '+str(frame))
      if frame%300==0:
       args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
       print(game+' natural OGG native agreement:',frame,flush=True)
      frame+=20;continue
    assert time.monotonic()<deadline,{'timeoutAt':frame,'values':values}
    pages[0].wait_for_timeout(50)
   if args.until_end:assert report.get('terminal'),report['latest']
   # Backward restore and forward seek must reconstruct the same native owners.
   viewer=pages[-1];state=m.snapshot(viewer,300)['playback'];assert state,state
   m.spectator_command(viewer,game,state['run'],'pause')
   targets=[300,900,600]+[20*(n['frame']//20+1) for n in state['nodes'] if n['kind']=='boss' and 20*(n['frame']//20+1)<=endframe]
   for target in targets:
    m.spectator_command(viewer,game,state['run'],'seek',target)
    m.wait_playback(viewer,lambda v:v['position']==target and v['paused'] and not v['seeking'])
    values=[m.snapshot(p,target) for p in pages]
    hashes=[v['hash'] for v in values];assert all(hashes) and len(set(hashes))==1,{'seek':target,'values':values}
    report.setdefault('navigation',[]).append({'frame':target,'hashes':hashes})
   for node in state['nodes']:
    if node['kind']!='boss' or node['frame']>endframe:continue
    m.spectator_command(viewer,game,state['run'],'seek',node['frame'])
    m.wait_playback(viewer,lambda v:v['position']==node['frame'] and not v['seeking'])
    # Resume from the actual Boss button target to the next sampled game frame.
    target=20*(node['frame']//20+1)
    m.spectator_command(viewer,game,state['run'],'play')
    m.wait_playback(viewer,lambda v:v['position']>=target)
    m.spectator_command(viewer,game,state['run'],'pause')
    values=[m.snapshot(p,target) for p in pages]
    hashes=[v['hash'] for v in values]
    assert all(hashes) and len(set(hashes))==1,{'boss':node,'values':values}
    report.setdefault('bossNavigation',[]).append({'node':node,'frame':target,'hashes':hashes})
   if game=='th07':
    report['musicStarted']=[p.evaluate("()=>document.getElementById('runtime').contentWindow.__eaglerNetplayBgmStarted||[]") for p in pages]
    assert all(any('th07_02' in path for path in paths) for paths in report['musicStarted']),report['musicStarted']
   report['music']=[p.evaluate("()=>{const w=document.getElementById('runtime').contentWindow;return w.__musicOpens}") for p in pages]
   report['music']=[[v for v in paths if v.startswith(bgmdir)] for paths in report['music']];print('music opens:',report['music'],flush=True);assert all(paths.count(bgmdir+game+'_02.ogg')>=2 for paths in report['music']),report['music'];report['passed']=True
  finally:b.close()
except BaseException as e:
 report['failure']=str(e);raise
finally:
 s.shutdown();args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
