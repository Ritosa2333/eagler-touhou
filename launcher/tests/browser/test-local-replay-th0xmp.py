"""Record native TH06/TH07 EAGX files with the existing scripted live profile,
then open the file in a fresh production Replay viewer, matching Launcher."""
import argparse,hashlib,json,threading,uuid
from http.server import SimpleHTTPRequestHandler,ThreadingHTTPServer
from pathlib import Path
from playwright.sync_api import sync_playwright
p=argparse.ArgumentParser();p.add_argument('--game',choices=['th06','th07'],required=True);p.add_argument('--relay',required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--workspace',type=Path,default=Path('D:/workspace/eagler'));p.add_argument('--players',choices=['2','3','all'],default='all');a=p.parse_args()
repo=a.workspace/(a.game+'-eagler');build=repo/('build-web-netplay-th06-replay-audit' if a.game=='th06' else 'build-web-th07-netplay-replay-audit');frames=180
files={'/font':a.workspace/'games'/a.game/'msgothic.ttc','/unifont':a.workspace/'dependencies/unifont-15.1.05/unifont-15.1.05.otf'}
for suffix in ['html','js','wasm','data']:files['/runtime/'+a.game+'.'+suffix]=build/(a.game+'.'+suffix)
for suffix in ['html','js','wasm','data']:files['/viewer-runtime/'+a.game+'.'+suffix]=repo/'build-web-release-multiplayer'/(a.game+'.'+suffix)
host='''<!doctype html><iframe id="runtime" width="640" height="480"></iframe><script>
const protocol='eagler-touhou/1',game=GAME,frame=document.querySelector('iframe'),pending=new Map();let serial=0;window.errors=[];
window.send=(command,payload={})=>new Promise((resolve,reject)=>{const request=String(++serial);pending.set(request,{resolve,reject});setTimeout(()=>{if(pending.delete(request))reject(Error(command+' timeout'));},30000);frame.contentWindow.postMessage({protocol,game,epoch:1,command,request,...payload},location.origin);});
addEventListener('message',async e=>{if(e.source!==frame.contentWindow||e.origin!==location.origin)return;const m=e.data;
if(m.request&&pending.has(m.request)){const p=pending.get(m.request);pending.delete(m.request);m.ok?p.resolve(m):p.reject(Error(m.error));return;}
if(m.event==='runtime-info'&&m.playback)window.playback=m.playback;if(m.event==='error')errors.push(m.error||m.message);
if(m.event==='ready'){try{const q=new URLSearchParams(location.search),seat=+q.get('seat'),count=+q.get('count');
await send('configure',{music:'none',resources:[],thpracCatalog:[],sharedResources:[{path:'/msgothic.ttc',url:'/font'},{path:'/unifont.otf',url:'/unifont'}],options:q.has('viewer')?{replayViewer:true,touchEnabled:false,thpracEnabled:false}:{debugHarness:'netplay-lan-stage1',netplayMode:'lan',netplayUrl:RELAY+'/?room='+q.get('room')+'&player='+seat+'&players='+count,netplayPlayer:seat,netplayPlayerCount:count,netplaySeed:19005,netplayDifficulty:1,netplayIceServers:[],netplayLoadouts:[{character:0,shot:0},{character:1,shot:1},{character:game==='th07'?2:0,shot:0}].slice(0,count),netplayPhysicalInput:false,netplayScriptedInput:true,netplayRollbackProbe:true,netplayReplayPlaybackCycle:true,netplayTestIncrementalTouch:true,netplayTestFrames:180,touchEnabled:false,thpracEnabled:false}});await send('launch');window.launched=true;}catch(error){errors.push(String(error));}}
});frame.src=(new URLSearchParams(location.search).has('viewer')?'/viewer-runtime/':'/runtime/')+game+'.html?hosted=1&runtimeVariant=multiplayer&runtimeEpoch=1';</script>'''.replace('GAME',json.dumps(a.game)).replace('RELAY',json.dumps(a.relay))
class Handler(SimpleHTTPRequestHandler):
 def log_message(self,*args):pass
 def do_GET(self):
  path=self.path.split('?')[0]
  if path=='/':data=host.encode();kind='text/html'
  elif path in files:data=files[path].read_bytes();kind='application/wasm' if path.endswith('.wasm') else 'text/javascript' if path.endswith('.js') else 'text/html' if path.endswith('.html') else 'application/octet-stream'
  else:self.send_error(404);return
  self.send_response(200);self.send_header('Content-Type',kind);self.send_header('Content-Length',str(len(data)));self.end_headers();self.wfile.write(data)
server=ThreadingHTTPServer(('127.0.0.1',0),Handler);threading.Thread(target=server.serve_forever,daemon=True).start()
report={'passed':False,'scope':'Native live scripted EAGX recording and fresh production player controls/all-seat input observation; not retail determinism','recordingWasm':hashlib.sha256(files['/runtime/'+a.game+'.wasm'].read_bytes()).hexdigest(),'wasm':hashlib.sha256(files['/viewer-runtime/'+a.game+'.wasm'].read_bytes()).hexdigest(),'cases':[]}
with sync_playwright() as pw:
 browsers=[]
 try:
  for count in ([2,3] if a.players=='all' else [int(a.players)]):
   pages=[];room=a.game+'-replay-'+uuid.uuid4().hex[:12]
   for seat in range(count):
    browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding']);browsers.append(browser)
    context=browser.new_context();context.add_init_script('delete globalThis.RTCPeerConnection');page=context.new_page();pages.append(page);page.goto(f'http://127.0.0.1:{server.server_port}/?room={room}&seat={seat}&count={count}')
   for page in pages:
    page.wait_for_function("(()=>{const w=document.querySelector('iframe').contentWindow;if(errors.length||w.__eaglerNetplayFailed)throw Error(JSON.stringify({errors,error:w.__eaglerNetplayError}));return w.__eaglerNetplayReplaySaved;})()",timeout=180000)
   page=pages[-1];native=page.frames[1]
   rows=page.evaluate('send("list")')['files'];path=next(row['path'] for row in rows if row['path'].endswith('.rpyx'));data=page.evaluate('path=>send("read",{path})',path)['bytes']
   fixture=a.output.parent.parent/'fixtures'/(a.game+'mp')
   fixture.mkdir(parents=True,exist_ok=True);(fixture/f'recording-{count}p.rpyx').write_bytes(bytes(data))
   recording_audit=native.evaluate('({expected:__eaglerNetplayReplayExpectedFrames,coverage:__eaglerNetplayReplayInputCoverage})')
   assert recording_audit['expected']==frames and recording_audit['coverage'],recording_audit
   for browser in browsers:browser.close()
   browsers=[]
   browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']);browsers.append(browser)
   page=browser.new_page();pages=[page];page.goto(f'http://127.0.0.1:{server.server_port}/?viewer=1');page.wait_for_function('window.launched||errors.length',timeout=120000);assert page.evaluate('!errors.length'),page.evaluate('errors');native=page.frames[1]
   page.evaluate('v=>send("write",v)',{'path':path,'bytes':data})
   description=page.evaluate('async path=>(await send("playback",{action:"inspect",path})).description',path);stage=description['stages'][0]['id']
   page.evaluate('v=>send("playback",{action:"open",...v})',{'path':path,'stage':stage});page.wait_for_function('playback?.frame>=60||errors.length',timeout=60000);assert page.evaluate('!errors.length'),page.evaluate('errors')
   page.evaluate('send("playback",{action:"pause"})');before=page.evaluate('playback.frame');page.wait_for_timeout(200);assert page.evaluate('playback.frame')==before
   page.evaluate('send("playback",{action:"seek",value:20})');page.wait_for_function('!playback.seeking&&playback.frame===20||errors.length',timeout=60000);assert page.evaluate('!errors.length'),page.evaluate('errors')
   page.evaluate('send("playback",{action:"rate",value:4})');page.evaluate('send("playback",{action:"play"})');page.wait_for_function('playback.frame>=150||errors.length',timeout=60000);assert page.evaluate('!errors.length'),page.evaluate('errors')
   observed=native.evaluate('({players:__eaglerNetplayReplayPlaybackPlayerCount,independent:__eaglerNetplayReplayPlaybackIndependentInputs,frame:__eaglerNetplayReplayPlaybackFrame})')
   assert observed['players']==count and observed['independent'] and observed['frame']>=150,observed
   page.evaluate('send("playback",{action:"rate",value:0})');page.wait_for_function('playback.ended||errors.length',timeout=60000);assert page.evaluate('!errors.length'),page.evaluate('errors')
   assert page.evaluate('playback.frame===playback.total&&playback.total>0'),page.evaluate('playback')
   report['cases'].append({'players':count,'passed':True,'bytes':len(data),'description':description,'recordingAudit':recording_audit,'nativeInputs':observed,'playback':page.evaluate('playback')});print(a.game+f' {count}P native EAGX/production player: PASS',flush=True)
   for browser in browsers:browser.close()
   browsers=[]
  report['passed']=True
 except BaseException as error:
  report['error']=str(error)
  if pages:
   report['failureState']=[page.evaluate('({playback:window.playback,errors,frame:document.querySelector("iframe").contentWindow.__eaglerNetplayLanFrame,error:document.querySelector("iframe").contentWindow.__eaglerNetplayError})') for page in pages]
  raise
 finally:
  for browser in browsers:browser.close()
  server.shutdown();a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
