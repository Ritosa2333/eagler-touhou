"""Actual TH08 shell, managed data, room/run admission, touch and IDBFS lifecycle."""
import argparse,json,time,uuid
from pathlib import Path
from playwright.sync_api import sync_playwright
p=argparse.ArgumentParser();p.add_argument('--url',required=True);p.add_argument('--relay',required=True)
p.add_argument('--output',type=Path,required=True);p.add_argument('--mode');p.add_argument('--players')
p.add_argument('--input-delay',type=int,choices=range(10),default=0)
p.add_argument('--adonis-mode',type=int,choices=range(3),default=0)
args=p.parse_args();report={'passed':False,'errors':[],'scope':'Runtime host protocol, actual native touch and live relay admission; not public Launcher product declaration'}
player_count=3 if args.players=='3' else 2
report.update({'players':player_count,'adonisMode':args.adonis_mode,'inputDelay':args.input_delay})
def save():args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
def call(page,code,arg=None):return page.evaluate(code,arg)
def request(page,command,**fields):return call(page,'v=>host.request(v.command,v.fields)',{'command':command,'fields':fields})
with sync_playwright() as pw:
 browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
 contexts=[];pages=[]
 try:
  for seat in range(player_count):
   context=browser.new_context(service_workers='block',viewport={'width':640,'height':480});contexts.append(context)
   page=context.new_page();pages.append(page);page.on('pageerror',lambda e:report['errors'].append(str(e)))
   page.on('requestfailed',lambda r:report['errors'].append('request failed '+r.url+' '+str(r.failure)))
   page.on('response',lambda r:report['errors'].append('HTTP '+str(r.status)+' '+r.url) if r.status>=400 else None)
   report['phase']='boot-seat-'+str(seat);save()
   page.goto(args.url+'host.html');call(page,'host.open()');page.wait_for_function('host.ready()',timeout=120000)
  room='th08mp-'+f'{uuid.uuid4().int%10000:04d}'
  timing={'adonisMode':args.adonis_mode,'inputDelay':args.input_delay,'predictionLimit':8}
  admitted=call(pages[0],"v=>host.lobby(v.relay,v.room,v.count,'',v.timing)",{'relay':args.relay,'room':room,'count':player_count,'timing':timing})
  assert all(admitted['room'][k]==v for k,v in timing.items()),admitted
  report['admission']=admitted;report['phase']='admitted';save()
  for seat,page in enumerate(pages):
   options={'runtimeVariant':'multiplayer','netplayMode':'lan','netplayUrl':args.relay+'/?room='+room+'&run='+str(admitted['serial']),
    'netplayPlayer':seat,'netplayPlayerCount':player_count,'netplayDifficulty':1,'netplaySeed':1234,
    'netplayInputDelay':admitted['room']['inputDelay'],'netplayPredictionLimit':admitted['room']['predictionLimit'],'netplayAdonisMode':admitted['room']['adonisMode'],
    'netplayLoadouts':[{'character':p,'shot':0} for p in range(player_count)], 'netplayIceServers':[],
    'touchEnabled':True,'touchMovementMode':'touch','touchSensitivity':100,'thpracEnabled':True}
   request(page,'configure',options=options,music='none');request(page,'launch')
  def advance(target):
   deadline=time.monotonic()+120
   while True:
    states=[call(page,'t=>host.tickTo(t)',target) for page in pages]
    if all(s['net'][4]==target and s['net'][5]>=target and s['net'][6]==0xffffffff for s in states):
     assert all(s['canonical']==states[0]['canonical'] for s in states),states
     if args.adonis_mode==1:
      assert all(s['driver'][3:6]==[0,0,0] and s['driver'][10]==0 and s['net'][7]==0 for s in states),states
     return states
    assert time.monotonic()<deadline,states
    pages[0].wait_for_timeout(2)
  settled=advance(179)
  assert all(s['net'][12]==args.input_delay for s in settled),settled
  assert all(s['net'][14]==args.adonis_mode for s in settled),settled
  # Keep the peer's simulation idle until prediction is exhausted. Repeated
  # native callbacks must not build an undeclared physical-input queue.
  for _ in range(30):stalled=call(pages[0],'host.physicalTick()')
  assert stalled['net'][13]<=stalled['net'][3]+1,stalled
  baseline=advance(219)
  assert all(s['net'][13]==s['net'][3] for s in baseline),baseline
  request(pages[1],'keyboard',code='ArrowRight',down=True)
  timing=[]
  for offset in range(args.input_delay+1):
   edge=advance(220+offset);timing.append(edge[0]['state'][25])
   if offset<args.input_delay:assert edge[0]['state'][25]==baseline[0]['state'][25],(offset,baseline,edge)
   else:assert edge[0]['state'][25]>baseline[0]['state'][25],(offset,baseline,edge)
  request(pages[1],'keyboard',code='ArrowRight',down=False)
  report['inputTiming']={'configured':args.input_delay,'firstMovementOffset':args.input_delay,
   'positions':timing,'captureAfterStall':stalled['net'][13],'nextAfterStall':stalled['net'][3]}
  before=advance(249);report['beforeTouch']=before;save()
  # Raw coordinates are in the iframe viewport, not already-adjusted player
  # positions. A P2 drag must reference P2's ship on both endpoints.
  request(pages[1],'direct-touch',type='down',id=91,x=.5,y=.7)
  request(pages[1],'direct-touch',type='move',id=91,x=.58,y=.7)
  touch_timing=[]
  for offset in range(args.input_delay+1):
   edge=advance(250+offset);touch_timing.append(edge[0]['state'][25])
   if offset<args.input_delay:assert edge[0]['state'][25]==before[0]['state'][25],(offset,before,edge)
   else:assert edge[0]['state'][25]>before[0]['state'][25],(offset,before,edge)
  report['touchInputTiming']={'firstMovementOffset':args.input_delay,'positions':touch_timing}
  moved=advance(289);report['afterP2Drag']=moved;save()
  assert moved[0]['state'][25]>before[0]['state'][25]+30,(before,moved)
  assert moved[0]['state'][13]==before[0]['state'][13],(before,moved)
  request(pages[1],'direct-touch',type='up',id=91,x=.58,y=.7)
  # Local focus/bomb controls travel through the same native input and wire.
  request(pages[1],'touch-controls',controls={'focusEnabled':True,'fireEnabled':True,'bombSerial':1})
  bombed=advance(319);report['afterTouchBomb']=bombed;save()
  assert bombed[0]['state'][23]<moved[0]['state'][23],(moved,bombed)
  request(pages[1],'touch-cancel');request(pages[1],'touch-controls',controls={'focusEnabled':False,'fireEnabled':False,'bombSerial':1})
  final=advance(349);report['final']=final
  saved_files=[]
  for page in pages:
   dirs=call(page,'host.directories()');assert 'savesth08-multiplayer' in dirs and 'savesth08' not in dirs,dirs
   request(page,'sync')
   try:request(page,'write',path='score.dat',bytes=[1]);raise AssertionError('Live import was accepted')
   except Exception as error:assert 'Cannot import saves during a multiplayer run' in str(error),str(error)
   exits=call(page,'host.close()');assert len(exits)==1,exits
   data=request(page,'read',path='score.dat')['bytes'];assert len(data)>64,'No committed native score file'
   saved_files.append(data);request(page,'write',path='score.dat',bytes=data)
   assert request(page,'read',path='score.dat')['bytes']==data
  assert call(pages[0],'host.lobbyOpen()'),'Runtime exit destroyed parent lobby ownership'
  # Recreate the real runtime iframe and recover isolated IDBFS contents.
  call(pages[1],'host.open()');pages[1].wait_for_function('host.ready()',timeout=120000)
  assert request(pages[1],'read',path='score.dat')['bytes']==saved_files[1]
  report.update({'storageReload':True,'roomRetained':True,'passed':True})
  assert not report['errors'],report['errors'];print('TH08 host protocol, P2 native touch/bomb, exit and persistence: PASS',flush=True)
 except BaseException as error:
  report['error']=str(error)
  report['events']=[call(page,'host.events') for page in pages];raise
 finally:
  for context in contexts:context.close()
  browser.close();save()
