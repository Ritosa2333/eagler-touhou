"""Compare native th06/th07 multiplayer Replay owners through real play.

Requires a built production Runtime, local music/font inputs and Playwright.
The default scenario uses production gameplay and native scripted input.
Private observation builds declare their stage and protected seats in the
receipt. Native same-frame samples cover playback and timeline navigation.
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
parser.add_argument('--difficulty',type=int,choices=range(5),default=1)
parser.add_argument('--music',choices=['ogg','none'],default='ogg')
parser.add_argument('--challenge',action='store_true')
parser.add_argument('--join-frame',type=int,default=0,help='wait for the source frame before admitting the spectator Runtime')
parser.add_argument('--join-stage',type=int,choices=range(9),default=0,help='wait for this source stage, then wait join-frame inputs before spectator admission')
parser.add_argument('--physical-input',action='store_true',help='drive native keyboard input with shooting, movement, Focus and Bomb edges')
parser.add_argument('--until-end',action='store_true',help='observe a native team wipe and enter the native result menu after consuming the confirmed tail')
parser.add_argument('--frames',type=int,default=2400)
parser.add_argument('--timeout',type=int,default=300)
parser.add_argument('--output',type=Path,required=True)
args=parser.parse_args()
assert args.frames>=1200 and args.frames%20==0
assert args.join_frame>=0
relaybase=args.relay.rstrip('/');game=args.game;count=args.players;endframe=args.frames
build=args.runtime_directory.resolve();music=args.music_directory.resolve()
fixture_path=build/'private-spectator.json'
fixture=json.loads(fixture_path.read_text(encoding='utf-8')) if fixture_path.is_file() else None
assert (build/(game+'.wasm')).is_file() and args.font.is_file()
host=(root/('ports/'+game+'/tests/netplay-spectator-browser-host.html')).read_text()
bgmdir='/bgm/' if game=='th06' else '/bgm-ogg/'
resources=[{'path':bgmdir+p.name,'url':'/music/'+p.name} for p in sorted(music.glob(game+'_??.ogg'))]
if args.music=='ogg':
 assert resources,'OGG inputs are required for the music scenario'
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
report={'game':game,'players':count,'difficulty':args.difficulty,'challenge':args.challenge,'joinFrame':args.join_frame,'musicMode':args.music,'scope':'native live stage without gameplay fixtures','physicalInput':args.physical_input,'passed':False,'wasm':hashlib.sha256((build/(game+'.wasm')).read_bytes()).hexdigest(),'samples':[],'errors':[]}
if fixture:
 assert fixture['privateSpectatorFixture'] is True and fixture['wasm']==report['wasm']
 report.update(scope='native stage with selected initial stage and mirrored invulnerability',runtimeFixture=fixture)
initial_stage=fixture['initialStage'] if fixture else (7 if args.difficulty==4 else 1)
initial_track=game+'_'+str(initial_stage*2).zfill(2)
from playwright.sync_api import sync_playwright
try:
 with sync_playwright() as pw:
  b=pw.chromium.launch(headless=True,channel='msedge',args=['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding'])
  try:
   setup=b.new_page();base='http://127.0.0.1:'+str(s.server_port);setup.goto(base+'/setup.html')
   room=game+'mp-natural-'+str(time.time_ns());m.admit_lobby(setup,relaybase,room,'natural_view',count,args.difficulty,args.challenge)
   pages=[b.new_page() for _ in range(count+1)]
   for seat,p in enumerate(pages):
    p.add_init_script('''delete globalThis.RTCPeerConnection; addEventListener('message',e=>{const m=e.data||{};if(m.event==='runtime-info'&&m.spectatorPlayback)globalThis.__spectatorPlaybackReport=m.spectatorPlayback;});''');p.on('pageerror',lambda e:report['errors'].append(str(e)))
    if seat==count:
     admission_deadline=time.monotonic()+args.timeout
     join_target=args.join_frame;waiting_stage=args.join_stage;next_admission_log=0
     while args.join_frame or args.join_stage:
      source_values=[m.snapshot(source_page,20) for source_page in pages[:count]]
      report['playersBeforeAdmission']=source_values
      assert not any(v['failed'] or v['hostFailure'] for v in source_values),source_values
      if time.monotonic()>=next_admission_log:
       print('waiting for spectator admission:',[(v['frame'],v['highestStage']) for v in source_values],flush=True)
       next_admission_log=time.monotonic()+10
      if waiting_stage and all(v['highestStage']>=waiting_stage for v in source_values):
       join_target=max(v['frame'] for v in source_values)+args.join_frame;waiting_stage=0
      if not waiting_stage and min(v['frame'] for v in source_values)>=join_target:break
      if args.physical_input:
       for source_page in pages[:count]:
        source_page.evaluate('''game=>{const w=document.getElementById('runtime')?.contentWindow;if(!w)return;
         for(const [code,keyCode] of [['KeyZ',90],['ControlLeft',17]])
          w.postMessage({protocol:'eagler-touhou/1',game,epoch:1,command:'keyboard',code,key:code,keyCode,down:true,inputSource:'physical'},location.origin);
        }''',game)
      assert time.monotonic()<admission_deadline,'source did not reach requested spectator admission frame'
      pages[0].wait_for_timeout(100)
     report['playersAtAdmission']=[m.snapshot(p,20)['frame'] for p in pages[:count]]
     pages[0].wait_for_timeout(2000)
    q={'runtimeBuild':'runtime','player':seat if seat<count else 0,'players':count,'difficulty':args.difficulty,'challenge':int(args.challenge),'frames':12000,
       'relay':relaybase+'/?room='+room+'&run=1&member='+('member_p'+str(seat+1)+'_00000'+str(seat+1) if seat<count else 'member_view_0003')}
    if seat==count:
     q.update(spectator=1,spectatorId='natural_view');q['relay']+='&spectator=natural_view'
    p.goto(base+'/host.html?'+urlencode(q),wait_until='domcontentloaded')
   entry=m.wait_playback(pages[-1],lambda v:v['position']>v['min'])
   # Late stage admission starts at the retained native Replay entry.
   base_frame=20*(entry['min']//20)
   endframe=base_frame+args.frames
   report['comparisonRange']={'first':base_frame+20,'last':endframe,'stageEntry':entry['min']}
   deadline=time.monotonic()+args.timeout;frame=base_frame+20;next_log=0;retired_at=None
   while frame<=endframe:
    values=[m.snapshot(p,frame) for p in pages]
    report['latest']=values
    if values[0]['nativePlayers']:
     report.setdefault('observedNativePlayers',{})[str(values[0]['frame'])]=values[0]['nativePlayers']
    retired=all(not v['active'] and v['confirmed']>=0 for v in values[:count])
    if retired:
     retired_at=retired_at or time.monotonic()
     playback=values[-1]['playback'] or {}
     if args.until_end and playback.get('ended') and playback['position']==playback['received'] and frame>playback['received'] and values[-1]['spectatorMenu']:
      report['terminal']={'confirmedEnd':playback['received'],'viewer':playback,'nativeResultMenu':True,'lastCompared':frame-20};break
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
       print(game+' natural native agreement:',frame,flush=True)
      frame+=20;continue
    assert time.monotonic()<deadline,{'timeoutAt':frame,'values':values}
    pages[0].wait_for_timeout(50)
   if args.until_end:assert report.get('terminal'),report['latest']
   # Backward restore and forward seek must reconstruct the same native owners.
   viewer=pages[-1];state=m.snapshot(viewer,300)['playback'];assert state,state
   if not args.until_end:m.spectator_command(viewer,game,state['run'],'pause')
   targets=[] if args.until_end else [base_frame+300,base_frame+900,base_frame+600]+[20*(n['frame']//20+1) for n in state['nodes'] if n['kind']=='boss' and 20*(n['frame']//20+1)<=endframe]
   for target in targets:
    m.spectator_command(viewer,game,state['run'],'seek',target)
    m.wait_playback(viewer,lambda v:v['position']==target and v['paused'] and not v['seeking'])
    values=[m.snapshot(p,target) for p in pages]
    hashes=[v['hash'] for v in values];assert all(hashes) and len(set(hashes))==1,{'seek':target,'values':values}
    report.setdefault('navigation',[]).append({'frame':target,'hashes':hashes})
   for node in ([] if args.until_end else state['nodes']):
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
   initial_track=game+'_'+str(state['nodes'][0]['stage']*2).zfill(2)
   if game=='th07' and args.music=='ogg':
    report['musicStarted']=[p.evaluate("()=>document.getElementById('runtime').contentWindow.__eaglerNetplayBgmStarted||[]") for p in pages]
    expected_track=initial_track
    assert all(any(expected_track in path for path in paths) for paths in report['musicStarted']),report['musicStarted']
   report['music']=[p.evaluate("()=>{const w=document.getElementById('runtime').contentWindow;return w.__musicOpens}") for p in pages]
   report['music']=[[v for v in paths if v.startswith(bgmdir)] for paths in report['music']]
   if args.music=='ogg':
    stage_music=bgmdir+initial_track+'.ogg'
    print('music opens:',report['music'],flush=True);assert all(paths.count(stage_music)>=(1 if args.until_end else 2) for paths in report['music']),report['music']
   report['passed']=True
  finally:b.close()
except BaseException as e:
 report['failure']=str(e);raise
finally:
 s.shutdown();args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
