"""TH10 confirmed-input spectator over the real BrowserPeerTransport/Relay."""
import argparse
import json
import time
import uuid
from pathlib import Path

from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--relay',required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--mode',choices=['rtc','relay'],default='rtc')
parser.add_argument('--players',type=int,choices=[2,3],default=2)
parser.add_argument('--stage2-only',action='store_true',help='diagnose a fresh Stage2 viewer without the Stage1 control/future-frame prelude')
args=parser.parse_args()
SPECTATOR_BUFFER_FRAMES=90
report={'passed':False,'scope':'TH10 independent native stage restore and live spectator playback over real relay',
        'mode':args.mode,'players':args.players,'errors':[],'diagnosticOnly':args.stage2_only}
PLAYER_IDS=['player_one_0001','player_two_0002','player_three_0003'][:args.players]
player_timeline={}
live_player_history=None

class Stage2OnlyComplete(Exception):
    pass

def call(page,expression,arg=None):
    return page.evaluate(expression,arg) if arg is not None else page.evaluate(expression)

def renderer(page):
    return call(page,"""()=>{const canvas=document.querySelector('canvas');if(!canvas)return null;
      const gl=canvas.getContext('webgl2')||canvas.getContext('webgl');
      const extension=gl?.getExtension('WEBGL_debug_renderer_info');
      return {vendor:extension?gl.getParameter(extension.UNMASKED_VENDOR_WEBGL):null,
        renderer:extension?gl.getParameter(extension.UNMASKED_RENDERER_WEBGL):null,
        version:gl?.getParameter(gl.VERSION)||null};}""")

def bootstrap_title(page,label):
    for tick in range(600):
        native=call(page,'multiplayerSmoke.nativeStatus()')
        if native[0]==4:return {'ticks':tick,'native':native}
        assert native[2]==0,(label,'native startup error',native)
        call(page,'multiplayerSmoke.netTicks(1)')
    raise AssertionError((label,'did not reach the native title screen',call(page,'multiplayerSmoke.nativeStatus()')))

def admit(setup,room,spectator_id):
    return setup.evaluate(
        """async ({relay,room,spectatorId,playerIds})=>{
          const clients=[];
          const open=(id,member,intent)=>new Promise((resolve,reject)=>{
            const ws=new WebSocket(relay+'/?room='+room+'&lobby='+id+'&member='+member+'&intent='+intent+'&players='+playerIds.length+'&difficulty=1'),queue=[],waiters=[];
            ws.onmessage=e=>{const v=JSON.parse(String(e.data)),w=waiters.shift();w?w(v):queue.push(v);};
            ws.onerror=()=>reject(new Error('lobby socket failed '+id));
            ws.onopen=()=>resolve({ws,next:()=>queue.length?Promise.resolve(queue.shift()):
              new Promise((done,fail)=>{const t=setTimeout(()=>fail(new Error('lobby timeout '+id)),10000);
                waiters.push(v=>{clearTimeout(t);done(v);});})});
          });
          const nextType=async(c,type)=>{for(;;){const v=await c.next();if(v.type==='error')throw Error(v.error||'lobby error');if(v.type===type)return v;}};
          const players=[];
          for(const [seat,id] of playerIds.entries()){
            const player=await open(id,'member_'+id,seat?'join':'create');players.push(player);clients.push(player.ws);await player.next();
          }
          const sp=await open(spectatorId,'member_spectator_0003','join');clients.push(sp.ws);await sp.next();
          players.forEach((player,seat)=>player.ws.send(JSON.stringify({type:'take-seat',seat,loadout:seat})));
          sp.ws.send(JSON.stringify({type:'spectate'}));
          let version=0;
          for(;;){const v=await sp.next();if(v.type==='error')throw Error(v.error||'spectate error');
            if(v.type==='state'&&v.room?.spectatorCount===1&&players.every((_,seat)=>v.room?.seats?.[seat])){
              version=Number(v.room.settingsVersion||0);break;}}
          players.forEach(player=>player.ws.send(JSON.stringify({type:'set-ready',ready:true,settingsVersion:version})));
          for(;;){const v=await sp.next(),seats=v.room?.seats||[];
            if(v.type==='error')throw Error(v.error||'ready error');
            if(v.type==='state'&&players.every((_,seat)=>seats[seat]?.ready))break;}
          const started=nextType(sp,'start');players[0].ws.send(JSON.stringify({type:'start'}));
          const value=await started;globalThis.__th10SpectatorLobbySockets=clients;
          return {serial:Number(value.serial),spectatorCount:Number(value.room?.spectatorCount)};
        }""",{'relay':args.relay,'room':room,'spectatorId':spectator_id,'playerIds':PLAYER_IDS})

def admit_late(setup,room,spectator_id):
    return setup.evaluate(
        """async ({relay,room,spectatorId,players})=>{
          const member='member_'+spectatorId,clientId=spectatorId;
          const ws=new WebSocket(relay+'/?room='+room+'&lobby='+clientId+'&member='+member+'&intent=join&players='+players+'&difficulty=1');
          const queue=[],waiters=[];
          ws.onmessage=e=>{const v=JSON.parse(String(e.data)),w=waiters.shift();w?w(v):queue.push(v);};
          await new Promise((resolve,reject)=>{const timeout=setTimeout(()=>reject(new Error('late lobby open timeout')),10000);
            ws.onopen=()=>{clearTimeout(timeout);resolve();};ws.onerror=()=>{clearTimeout(timeout);reject(new Error('late spectator lobby socket failed '+clientId));};});
          const next=()=>queue.length?Promise.resolve(queue.shift()):new Promise((resolve,reject)=>{
            const timeout=setTimeout(()=>reject(new Error('late lobby message timeout')),10000);
            waiters.push(v=>{clearTimeout(timeout);resolve(v);});});
          let first=await next();if(first.type==='error')throw new Error(first.error||'late lobby rejected');
          ws.send(JSON.stringify({type:'spectate'}));
          for(;;){const value=await next();if(value.type==='error')throw new Error(value.error||'late spectate rejected');
            if(value.type==='spectator-start'){
              (globalThis.__th10SpectatorLobbySockets||=[]).push(ws);
              return {serial:Number(value.serial),spectatorCount:Number(value.room?.spectatorCount)};
            }}
        }""",{'relay':args.relay,'room':room,'spectatorId':spectator_id,'players':args.players})

def buttons(seat,frame):
    if frame<20:return 0
    return 1|(0x80 if ((frame//30+seat)%2) else 0x40)|(4 if frame%40<20 else 0)

def advance_sources(players):
    global live_player_history
    states=[call(page,'multiplayerSmoke.netStatus()') for page in players]
    frame=states[0][2]
    assert all(value[2]==frame for value in states),('player frontier mismatch',states)
    inputs=[buttons(seat,frame) for seat in range(len(players))]
    for page,value in zip(players,inputs):
        assert call(page,'v=>multiplayerSmoke.captureLocalInput(...v)',[frame,value]),('local input capture',frame,value)
    for seat,page in enumerate(players):
        for remote,value in enumerate(inputs):
            if remote==seat:continue
            result=call(page,'v=>multiplayerSmoke.submitRemote(...v)',[remote,frame,value])
            assert result in (1,2,3,4),('remote input seed',seat,remote,frame,result)
    for page in players:assert call(page,'multiplayerSmoke.pollNetwork()')
    for page in players:call(page,'multiplayerSmoke.netTicks(1)')
    advanced=[call(page,'multiplayerSmoke.netStatus()') for page in players]
    frame=advanced[0][3]
    if 0<=frame<=10 and frame not in player_timeline:
        player_timeline[frame]=[snapshot(page) for page in players]
    if live_player_history is not None and frame>=0:
        live_player_history[frame]=[snapshot(page) for page in players]
        cutoff=frame-256
        for old in [old for old in live_player_history if old<cutoff]:del live_player_history[old]
    return advanced

pointer_words={1,10,11,28,29,30,40,41,42}
semantic_canonical_words=set([12,13,14,15,16,*range(31,38)])
canonical_field_names={
    0:'canonical schema',1:'composite hash',2:'team/economy/input-lane hash',
    3:'script and visual RNG hash',4:'player semantic hash',5:'enemy semantic hash',
    6:'bullet semantic hash',7:'laser semantic hash',8:'item/owner semantic hash',
    9:'authored ANM semantic hash',10:'scene and stage semantic hash',11:'rollback-owner chain hash',
    12:'session started flag',13:'script RNG seed',14:'script RNG call count',
    15:'visual RNG seed',16:'visual RNG call count',
    17:'ANM metadata-pool hash',18:'ANM pooled-payload hash',19:'ANM overflow count',
    20:'ANM started-script counter',21:'ANM processed-count counter',22:'ANM cursor',23:'ANM last ID',
    24:'ANM occupied slots',25:'ANM world VM count',26:'ANM UI VM count',27:'ANM overflow slots',
    28:'EnemyManager raw hash',29:'enemy rollback-pool raw hash',30:'ECL rollback-pool raw hash',
    31:'EnemyManager active count',32:'EnemyManager spawn count',33:'EnemyManager lifetime',
    34:'first boss slot active',35:'enemy pool active count',36:'ECL pool active count',
    37:'engine speed',
    38:'replay diagnostic hash',39:'CommonResources field-group hash',
    40:'HUD dialogue pool hash',41:'ANM geometry allocator hash',42:'camera/engine pointer hash',
    43:'ScoreSettings and CharacterRecords hash'}
common_debug_names=['schema','flags','state','frames','introduction_animation','loading_animation','text_count']
score_debug_names=['schema','settings.header','settings.last_name','settings.statistics',
                   'settings.random_words','settings.reserved']+[
                   f'character[{seat}].{field}' for seat in range(7)
                   for field in ('header','character','high_scores','statistics','spells')]
item_debug_header_names=['schema','manager.flags','manager.state','manager.active_count',
                         'manager.faith_cursor','manager.faith_count','regular_active','faith_active',
                         'row_count','truncated_rows']
item_debug_field_names=['pool','slot','state','kind','sprite_kind','position.x','position.y','position.z',
                        'velocity.x','velocity.y','velocity.z','motion_3c4','timer.previous','timer.current',
                        'timer.fractional','timer.rate_value','timer_flags','attraction_speed','value_variant',
                        'owner.homing','owner.recipient','animation.semantic_hash','animation.id',
                        'animation.owner_tag','animation.script_index','animation.file_index',
                        'animation.instruction_offset','animation.script_timer.previous',
                        'animation.script_timer.current','animation.resource_name_hash',
                        'animation.geometry_pointer','raw_item_hash',
                        'animation.rotation_angular_velocity','animation.scale_velocity',
                        'animation.sprite_uv_state','animation.script_timer_state',
                        'animation.script_timer_flags','animation.position_interpolation',
                        'animation.color_interpolation','animation.alpha_interpolation',
                        'animation.rotation_interpolation','animation.scale_interpolation',
                        'animation.color2_interpolation','animation.alpha2_interpolation',
                        'animation.colors_pending_interrupt','animation.integer_variables',
                        'animation.float_variables','animation.extra_integer_variables',
                        'animation.draw_script_position','animation.position','animation.child_position',
                        'animation.authored_flags','animation.saved_timer','animation.saved_timer_flags',
                        'animation.saved_instruction_offset','animation.sprite_frame',
                        'animation.sprite_file_script_indices','animation.matrices',
                        'animation.resource_id','animation.script_timer_rate']
bullet_debug_header_names=['schema','manager.field0','manager.field1','manager.cursor_slot',
                           'manager.last_cancel_position','manager.last_cancel_size','manager.active_count',
                           'manager.live_slots','row_count','truncated_rows']
bullet_debug_field_names=['slot','state','id','flags','cancel_protection','motion.position.x',
                          'motion.position.y','motion.position.z','motion.velocity.x','motion.velocity.y',
                          'motion.velocity.z','motion.speed','motion.angle','cancel_size','hitbox_height',
                          'cancel_timer','cancel_timer_flags','secondary_timer','secondary_timer_flags',
                          'outside_delay','cancel_script','active_features','spawn_flags','command_index',
                          'draw_layer','sprite_type','color','commands_hash','modifiers_hash',
                          'animation.semantic_hash','animation.id','animation.owner_tag','animation.script_index',
                          'animation.file_index','animation.instruction_offset','animation.script_timer',
                          'animation.resource_name_hash','raw_bullet_hash','animation.geometry_pointer',
                          'animation.draw_script_position.x','animation.draw_script_position.y',
                          'animation.draw_script_position.z','animation.flags','animation.position.x',
                          'animation.position.y','animation.position.z','animation.sprite_index',
                          'animation.sprite_frame']

def group_diffs(expected,actual,names):
    return [{'field':name,'player':a,'viewer':b} for name,a,b in zip(names,expected,actual) if a!=b]

def item_debug_diffs(expected,actual):
    expected_rows={tuple(row[:2]):row for row in
                   [expected[10+i*64:10+(i+1)*64] for i in range(expected[8])]}
    actual_rows={tuple(row[:2]):row for row in
                 [actual[10+i*64:10+(i+1)*64] for i in range(actual[8])]}
    rows=[]
    for key in sorted(set(expected_rows)|set(actual_rows)):
        left=expected_rows.get(key);right=actual_rows.get(key)
        if left is None or right is None:
            rows.append({'pool_slot':key,'player':left,'viewer':right});continue
        diff=[{'field':name,'player':a,'viewer':b} for name,a,b in zip(item_debug_field_names,left,right) if a!=b]
        if diff:rows.append({'pool_slot':key,'fields':diff})
    return {'header':group_diffs(expected[:10],actual[:10],item_debug_header_names),'rows':rows}
def bullet_debug_diffs(expected,actual):
    expected_rows={row[0]:row for row in
                   [expected[10+i*48:10+(i+1)*48] for i in range(expected[8])]}
    actual_rows={row[0]:row for row in
                 [actual[10+i*48:10+(i+1)*48] for i in range(actual[8])]}
    rows=[]
    for slot in sorted(set(expected_rows)|set(actual_rows)):
        left=expected_rows.get(slot);right=actual_rows.get(slot)
        if left is None or right is None:
            rows.append({'slot':slot,'player':left,'viewer':right});continue
        diff=[{'field':name,'player':a,'viewer':b} for name,a,b in zip(bullet_debug_field_names,left,right) if a!=b]
        if diff:rows.append({'slot':slot,'fields':diff})
    return {'header':group_diffs(expected[:10],actual[:10],bullet_debug_header_names),'rows':rows}
def snapshot(page):
    value=call(page,"""()=>({net:multiplayerSmoke.netStatus(),
        portable:multiplayerSmoke.portableCanonical(),canonical:multiplayerSmoke.canonical(),
        stageScene:multiplayerSmoke.stageSceneCanonical(),stageFields:multiplayerSmoke.stageSceneFields(),
        state:multiplayerSmoke.status(),audio:multiplayerSmoke.audioStatus(),
        enemyDebug:multiplayerSmoke.enemyDebug(),eclContextDebug:multiplayerSmoke.eclContextDebug(),
        spectatorParityDebug:multiplayerSmoke.spectatorParityDebug(),
        spectatorOptionsDebug:multiplayerSmoke.spectatorOptionsDebug(),
        spectatorEntityCounts:multiplayerSmoke.spectatorEntityCounts(),
        spectatorItemDebug:multiplayerSmoke.spectatorItemDebug(),
        spectatorBulletDebug:multiplayerSmoke.spectatorBulletDebug(),
        spectatorCommonDebug:multiplayerSmoke.spectatorCommonDebug(),
        spectatorScoresDebug:multiplayerSmoke.spectatorScoresDebug(),
        spectatorAnmDebug:multiplayerSmoke.spectatorAnmDebug()})""")
    if args.stage2_only:value['spectatorAnmFields']=call(page,'multiplayerSmoke.spectatorAnmFields()')
    return value

def assert_hash_equal(expected,actual,label):
    assert expected['portable']==actual['portable'],(label,'portable canonical mismatch',expected['portable'],actual['portable'])
    assert expected['stageScene']==actual['stageScene'],(label,'STD/embedded ANM canonical mismatch',expected['stageScene'],actual['stageScene'])
    mismatches=[(i,expected['canonical'][i],actual['canonical'][i]) for i in semantic_canonical_words
                if expected['canonical'][i]!=actual['canonical'][i]]
    assert not mismatches,(label,'logical scalar canonical mismatch',mismatches)

def assert_sources_equal(sources,label):
    assert len(sources)==args.players,(label,'source count',len(sources),args.players)
    for seat,source in enumerate(sources[1:],1):assert_hash_equal(sources[0],source,f'{label} source{seat}')

def hash_diff(expected,actual):
    return {'portable':[(i,a,b) for i,(a,b) in enumerate(zip(expected['portable'],actual['portable'])) if a!=b],
            'stageScene':[(i,a,b) for i,(a,b) in enumerate(zip(expected['stageScene'],actual['stageScene'])) if a!=b],
            'canonical':[(i,a,b) for i,(a,b) in enumerate(zip(expected['canonical'],actual['canonical']))
                         if a!=b and i in semantic_canonical_words],
            'canonicalDiagnostics':[(i,expected['canonical'][i],actual['canonical'][i]) for i in range(44)
                                    if expected['canonical'][i]!=actual['canonical'][i] and i not in semantic_canonical_words]}

def snap_next_values(snapshot):
    words=snapshot['spectatorOptionsDebug'];values=[]
    for seat in range(args.players):
        base=4+seat*84
        values.append([words[base+2+option*20+13] for option in range(4)])
    return values

def assert_enemy_ecl_equal(expected,actual,label):
    # Pool index and ANM handles are local graph identities. ECL stack words
    # which hold return-instruction addresses are normalized by the portable
    # context hash at row word 40; compare all named timing/instruction fields
    # and that complete portable hash, omitting only the four raw diagnostic
    # words per context that may contain address-valued return slots.
    e_expected=expected['eclContextDebug'];e_actual=actual['eclContextDebug']
    assert e_expected[224]==e_actual[224],(label,'active ECL enemy count',e_expected[224],e_actual[224])
    for row in range(e_expected[224]):
        base=1+row*56
        semantic=[*range(1,15),*range(19,31),*range(35,56)]
        left=[e_expected[base+i] for i in semantic];right=[e_actual[base+i] for i in semantic]
        assert left==right,(label,'ECL semantic context mismatch',row,semantic,left,right)
    e_expected=expected['enemyDebug'];e_actual=actual['enemyDebug']
    assert e_expected[80]==e_actual[80],(label,'active enemy count',e_expected[80],e_actual[80])
    for row in range(e_expected[80]):
        base=1+row*20
        semantic=[*range(3,10),*range(15,20)]
        assert [e_expected[base+i] for i in semantic]==[e_actual[base+i] for i in semantic],(
            label,'enemy semantic fields mismatch',row,
            [e_expected[base+i] for i in semantic],[e_actual[base+i] for i in semantic])

with sync_playwright() as p:
    # Use Chromium's default ANGLE/D3D11 selection so multiple native game
    # instances can use the host GPU during checkpoint restoration tests.
    browser=p.chromium.launch(headless=True,args=['--enable-gpu','--use-gl=angle','--use-angle=d3d11'])
    # Keep pages restricted to source players; viewers have separate owners.
    contexts=[];pages=[];started_at=time.monotonic();phase='setup'
    try:
        report['browser']=browser.version
        # This exact-frame fixture pumps players synchronously from Python.
        # Loading a third Runtime AFTER frame 89 otherwise freezes both live
        # players for the entire asset/JIT load and trips their legitimate
        # confirmed-input watchdog. Preload code/assets only, while no active
        # game clock exists; the native spectator app and actual transport
        # admission still start after frame 89. Do not weaken game timeouts.
        spectator_context=browser.new_context(service_workers='block');contexts.append(spectator_context)
        spectator=spectator_context.new_page()
        spectator.on('pageerror',lambda error:report['errors'].append({'endpoint':'spectator','error':str(error)}))
        before_load=time.monotonic()
        spectator.goto(args.url);spectator.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
        report['spectatorRuntimePreloaded']=True
        report['spectatorRuntimeLoadMs']=(time.monotonic()-before_load)*1000
        late_context=browser.new_context(service_workers='block');contexts.append(late_context)
        late_viewer=late_context.new_page()
        late_viewer.on('pageerror',lambda error:report['errors'].append({'endpoint':'late-spectator','error':str(error)}))
        before_late_load=time.monotonic()
        late_viewer.goto(args.url);late_viewer.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
        report['lateSpectatorRuntimePreloaded']=True
        report['lateSpectatorRuntimeLoadMs']=(time.monotonic()-before_late_load)*1000
        loadouts=[[0,0],[1,1],[0,1]][:args.players]
        session_low,session_high=0x71625344,0x18273645
        for page in (spectator,late_viewer):
            call(page,'multiplayerSmoke.setSpectatorCount(1)')
            call(page,'v=>multiplayerSmoke.startNet(...v)',[loadouts,0,1,1234,session_low,session_high])
        report['viewerConfiguredBeforeFirstNativeTick']=True
        setup=browser.new_context().new_page()
        room='th10mp-'+uuid.uuid4().hex[:12]
        spectator_id='spectator_0003'
        started=admit(setup,room,spectator_id)
        assert started=={'serial':1,'spectatorCount':1},started
        run_url=[f"{args.relay}/?room={room}&run=1&member=member_{player_id}" for player_id in PLAYER_IDS]
        spectator_url=f"{args.relay}/?room={room}&run=1&member=member_spectator_0003"
        for seat in range(args.players):
            context=browser.new_context(service_workers='block');contexts.append(context)
            if args.mode=='relay':
                context.add_init_script("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true});")
            page=context.new_page();pages.append(page)
            page.on('pageerror',lambda error,i=seat:report['errors'].append({'endpoint':i,'error':str(error)}))
            page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
            call(page,'multiplayerSmoke.setSpectatorCount(1)')
            call(page,'v=>multiplayerSmoke.startNet(...v)',[loadouts,seat,1,1234,session_low,session_high])
            assert call(page,'u=>multiplayerSmoke.connect(u)',run_url[seat])
        deadline=time.monotonic()+60
        while not all(call(page,'multiplayerSmoke.canStart()') for page in pages):
            for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()'),call(page,'multiplayerSmoke.networkError()')
            assert time.monotonic()<deadline,'player handshake timeout'
            pages[0].wait_for_timeout(5)
        expected_route=1 if args.mode=='rtc' else 2
        assert all(call(page,'multiplayerSmoke.transportStatus()')[13]==expected_route for page in pages)
        report['runtimeIdentities']=[call(page,'multiplayerSmoke.identity()') for page in pages]

        if args.stage2_only:
            live_player_history={}
            phase='stage2-only-source-ready'
            source_ready_deadline=time.monotonic()+90
            stage2_seeded_frames=set()
            while True:
                for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()')
                native=[call(page,'multiplayerSmoke.nativeStatus()') for page in pages]
                lifecycle=[call(page,'multiplayerSmoke.lifecycle()') for page in pages]
                net=[call(page,'multiplayerSmoke.netStatus()') for page in pages]
                source_ready=(all(value[0]==7 and value[1]==1 and value[2]==0 for value in native) and
                    all(not(value[3]&4) and value[6]>=-1 for value in lifecycle) and
                    len({value[6] for value in lifecycle})==1 and len({value[2] for value in net})==1 and
                    len({value[3] for value in net})==1 and all(value[3]>=0 and value[4]>=value[3] for value in net))
                if source_ready:
                    break
                # The browser's normal RAF advances native resource loading.
                # Do not synchronously drive the pair while one player is
                # still constructing its session: the already-loaded player
                # may have reached frame zero while the other is still at -1.
                load=[call(page,'multiplayerSmoke.spectatorLoadDebug()') for page in pages]
                loading=[len(value)>3 and bool(value[3]) for value in load]
                same_frontier=len({value[2] for value in net})==1
                if same_frontier:
                    frame=net[0][2]
                    if frame not in stage2_seeded_frames:
                        inputs=[buttons(seat,frame) for seat in range(args.players)]
                        for page,value in zip(pages,inputs):
                            assert call(page,'v=>multiplayerSmoke.captureLocalInput(...v)',[frame,value]),(
                                'stage2 diagnostic local input capture',frame,value)
                        for seat,page in enumerate(pages):
                            for remote,value in enumerate(inputs):
                                if remote==seat:continue
                                result=call(page,'v=>multiplayerSmoke.submitRemote(...v)',[remote,frame,value])
                                assert result in (1,2,3,4),('stage2 diagnostic remote seed',seat,remote,frame,result)
                        stage2_seeded_frames.add(frame)
                        for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()')
                if any(loading):
                    # Advance only peers whose native GameSessionResources are
                    # still loading. Their already-seeded frame input lets a
                    # peer that finishes first stop on the ordinary lockstep
                    # boundary while the other completes its resource graph.
                    for index,is_loading in enumerate(loading):
                        if is_loading:call(pages[index],'multiplayerSmoke.netTicks(1)')
                elif same_frontier:
                    advance_sources(pages)
                elif min(value[2] for value in net) in stage2_seeded_frames:
                    # One peer may have completed the first native frame just
                    # before the other finished loading. Let only the lagging
                    # peer consume the already-seeded frame.
                    lagging=[index for index,value in enumerate(net)
                             if value[2]==min(peer[2] for peer in net)]
                    for index in lagging:call(pages[index],'multiplayerSmoke.netTicks(1)')
                else:
                    pages[0].wait_for_timeout(5)
                assert time.monotonic()<source_ready_deadline,('native Stage1 source was not ready for complete_stage',
                    native,lifecycle,net,load)
            report['gpuRenderer']={f'player{seat}':renderer(page) for seat,page in enumerate(pages)}
            report['stage2Only']={'sourceReadyNative':native,'sourceReadyLifecycle':lifecycle,
                                  'sourceReadyNet':net}
            phase='stage2-only-native-complete-stage'
            fixture_results=[call(page,'()=>multiplayerSmoke.fixture(6)') for page in pages]
            assert len(fixture_results)==args.players and all(fixture_results),('native complete_stage fixture rejected',fixture_results,
                [call(page,'multiplayerSmoke.nativeStatus()') for page in pages])
            report['stage2Only']['completeStageFixtureResults']=fixture_results
            transition_initial_lifecycle=[call(page,'multiplayerSmoke.lifecycle()') for page in pages]
            before_stage2_frame=[value[3] for value in net]
            phase='stage2-only-native-screen-fence'
            transition_deadline=time.monotonic()+90
            while True:
                for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()')
                # Resource loading can finish on different host polls. Freeze
                # an endpoint once it has returned to the native game screen;
                # another loading poll must not become its next gameplay tick.
                boundary=[]
                for page in pages:
                    life=call(page,'multiplayerSmoke.lifecycle()')
                    boundary.append({'error':0,'arrived':True} if life[1]==7 and life[2]==7
                                    else call(page,'multiplayerSmoke.fixtureTransitionTick()'))
                assert all(value['error']==0 for value in boundary),('native complete_stage screen-fence error',boundary)
                lifecycle=[call(page,'multiplayerSmoke.lifecycle()') for page in pages]
                if all(value[1]==7 and value[2]==7 for value in lifecycle):break
                assert time.monotonic()<transition_deadline,('native Stage2 screen fence timeout',lifecycle,boundary)
                pages[0].wait_for_timeout(2)
            transition_frame=[call(page,'multiplayerSmoke.netStatus()')[3] for page in pages]
            assert len(set(transition_frame))==1 and all(
                call(page,'multiplayerSmoke.netStatus()')[4]==transition_frame[index]
                for index,page in enumerate(pages)),('Stage2 screen fence was not mutually confirmed',transition_frame)
            report['stage2Only'].update({'initialLifecycle':transition_initial_lifecycle,
                'frontierImmediatelyBeforeFixture':before_stage2_frame,'frontierAfterScreenFence':transition_frame,
                'screenFence':lifecycle,'screenFenceNative':[call(page,'multiplayerSmoke.nativeStatus()') for page in pages]})
            phase='stage2-only-native-stage2-ready'
            stage2_deadline=time.monotonic()+120
            saw_native_fade=False
            while True:
                advance_sources(pages)
                native=[call(page,'multiplayerSmoke.nativeStatus()') for page in pages]
                host_frames=[call(page,'multiplayerSmoke.netStatus()')[3] for page in pages]
                activation_lifecycle=[call(page,'multiplayerSmoke.lifecycle()') for page in pages]
                saw_native_fade=saw_native_fade or any(value[3]&0x800 for value in activation_lifecycle)
                if all(value[0]==7 and value[1]==2 and value[2]==0 for value in native) and all(
                    value>transition_frame[index] for index,value in enumerate(host_frames)) and saw_native_fade and all(
                    not(value[3]&0x800) and value[6]>0 for value in activation_lifecycle):
                    break
                assert time.monotonic()<stage2_deadline,('native Stage2 source was not ready',native,host_frames)
            report['stage2Only'].update({'stage2NativeReady':native,'stage2HostFrames':host_frames,
                'activationLifecycle':activation_lifecycle,'sawNativeFade':saw_native_fade,
                'stage2HostPortable':[snapshot(page)['portable'] for page in pages]})
            assert_sources_equal([snapshot(page) for page in pages],'Stage2 diagnostic hosts')
            # Publish the confirmed replacement before the new admission, so
            # this case really bootstraps Stage2 rather than receiving a live
            # Stage2 replacement during a Stage1 bootstrap.
            for _ in range(4):advance_sources(pages)
            pages[0].wait_for_timeout(20)

            phase='stage2-only-late-viewer-admission'
            late_spectator_id='spectator_0004'
            late_start=admit_late(setup,room,late_spectator_id)
            assert late_start['serial']==1,('late spectator joined wrong run',late_start)
            if args.mode=='relay':late_context.add_init_script("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true});")
            late_url=f"{args.relay}/?room={room}&run=1&member=member_{late_spectator_id}"
            assert call(late_viewer,'v=>multiplayerSmoke.spectatorConnect(...v)',[late_url,late_spectator_id])
            phase='stage2-only-await-checkpoint'
            late_deadline=time.monotonic()+180
            while True:
                assert call(late_viewer,'multiplayerSmoke.pollNetwork()'),call(late_viewer,'multiplayerSmoke.networkError()')
                current=call(late_viewer,'multiplayerSmoke.spectatorPlaybackState()')
                stage2_node=next((node for node in current.get('nodes',[]) if node.get('stage')==2),None) if current else None
                if stage2_node:break
                assert time.monotonic()<late_deadline,('new viewer did not receive source Stage2 checkpoint',current,
                    call(late_viewer,'multiplayerSmoke.spectatorStatus()'))
                # Spectator transport has a 15 s no-input watchdog. Keep the
                # authoritative source moving while a newly admitted viewer
                # waits for the next confirmed Stage2 checkpoint.
                advance_sources(pages)
                # A nonzero first CP is not exposed by the view until native
                # initialization has reached it. Pump the actual viewer too.
                call(late_viewer,'multiplayerSmoke.spectatorTick()')
                load_status=call(late_viewer,'multiplayerSmoke.spectatorLoadDebug()')
                assert not any(load_status[:3]),('Stage2 viewer native loading failed',load_status)
                late_viewer.wait_for_timeout(5)
            assert current['min']==stage2_node['frame'],('new viewer did not bootstrap only Stage2',current)
            report['stage2Only'].update({'lobby':late_start,'checkpointNode':stage2_node,
                'viewerStateBeforeStage2Seek':current,
                'viewerStatusBeforeStage2Seek':call(late_viewer,'multiplayerSmoke.spectatorStatus()'),
                'viewerLoadDebugBeforeStage2Seek':call(late_viewer,'multiplayerSmoke.spectatorLoadDebug()'),
                'viewerNativeTitle':call(late_viewer,'multiplayerSmoke.nativeStatus()')})
            # Compare after the checkpoint's first input. The source graph can
            # finish loading between its last committed tick and CP capture.
            target_position=stage2_node['frame']+1
            while current['edge']<target_position:
                advance_sources(pages)
                assert call(late_viewer,'multiplayerSmoke.pollNetwork()')
                current=call(late_viewer,'multiplayerSmoke.spectatorPlaybackState()')
                assert time.monotonic()<late_deadline,('Stage2 diagnostic buffered edge timeout',current)
            checkpoint_source=live_player_history[stage2_node['frame']]
            assert call(late_viewer,"()=>multiplayerSmoke.spectatorPlaybackControl('pause')")
            assert call(late_viewer,f"()=>multiplayerSmoke.spectatorPlaybackControl('seek',{target_position})")
            phase='stage2-only-native-stage2-restore'
            restore_deadline=time.monotonic()+240
            restore_ticks=0
            next_restore_log=time.monotonic()
            while True:
                assert call(late_viewer,'multiplayerSmoke.pollNetwork()'),call(late_viewer,'multiplayerSmoke.networkError()')
                current=call(late_viewer,'multiplayerSmoke.spectatorPlaybackState()')
                if current and current['position']==target_position and not current['seeking']:break
                if time.monotonic()>=next_restore_log:
                    print('TH10 Stage2 restore',json.dumps({'ticks':restore_ticks,'playback':current,
                        'native':call(late_viewer,'multiplayerSmoke.nativeStatus()'),
                        'loading':call(late_viewer,'multiplayerSmoke.spectatorLoadDebug()')}),flush=True)
                    next_restore_log=time.monotonic()+5
                assert time.monotonic()<restore_deadline,('Stage2 native restore seek timeout',current)
                advance_sources(pages)
                call(late_viewer,'multiplayerSmoke.spectatorTick()')
                restore_ticks+=1
            report['stage2Only'].update({'nativeStage2Restore':{'playback':current,'restoreTicks':restore_ticks,
                'viewerLoadDebug':call(late_viewer,'multiplayerSmoke.spectatorLoadDebug()'),
                'viewerNativeStatus':call(late_viewer,'multiplayerSmoke.nativeStatus()'),
                'viewerCanonical':call(late_viewer,'multiplayerSmoke.canonical()'),
                'viewerPortable':call(late_viewer,'multiplayerSmoke.portableCanonical()')}})
            report['gpuRenderer']['lateSpectator']=renderer(late_viewer)
            restored=snapshot(late_viewer)
            report['stage2Only']['firstInputComparison']={'position':target_position,
                'sources':checkpoint_source,'viewer':restored,'diff':hash_diff(checkpoint_source[0],restored)}
            assert call(late_viewer,'multiplayerSmoke.nativeStatus()')[:3]==[7,2,0],(
                'Stage2 viewer did not construct the native game',restored['state'])
            assert_sources_equal(checkpoint_source,'Stage2 checkpoint sources')
            assert_hash_equal(checkpoint_source[0],restored,'Stage2 independent checkpoint restore')
            report['diagnosticComplete']=True
            raise Stage2OnlyComplete()

        target_before_join=89
        phase='players-before-join'
        deadline=time.monotonic()+90
        at_44=None
        while True:
            if at_44 is None and all(call(page,'multiplayerSmoke.netStatus()')[3]==44 and
                                     call(page,'multiplayerSmoke.netStatus()')[5]==-1 for page in pages):
                at_44=[snapshot(page) for page in pages]
            ready=all(call(page,'multiplayerSmoke.netStatus()')[3]==target_before_join and
                      call(page,'multiplayerSmoke.netStatus()')[5]==-1 for page in pages)
            if ready:break
            assert time.monotonic()<deadline,'players did not reach delayed-join boundary'
            advance_sources(pages)
            pages[0].wait_for_timeout(2)
        for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()')

        host_at_89=[snapshot(page) for page in pages]
        assert_sources_equal(at_44,'players frame 44')
        assert_sources_equal(host_at_89,'players frame 89')

        phase='spectator-connect'
        assert call(spectator,'v=>multiplayerSmoke.spectatorConnect(...v)',[spectator_url,spectator_id])
        assert call(spectator,'multiplayerSmoke.canStart()')
        write_probe=call(spectator,'multiplayerSmoke.spectatorWriteProbe()')
        assert write_probe=={'capture':False,'remote':7,'packetBytes':0,'helloBytes':0,'markReady':False},write_probe

        phase='spectator-catchup-to-frame-90'
        deadline=time.monotonic()+180
        while True:
            advance_sources(pages)
            spectator_value=call(spectator,'n=>multiplayerSmoke.spectatorAdvance(n)',90)
            playback=spectator_value.get('playback') or call(spectator,'multiplayerSmoke.spectatorPlaybackState()')
            if playback and playback['position']>=90:
                if playback['position']!=90:
                    assert call(spectator,"()=>multiplayerSmoke.spectatorPlaybackControl('seek',90)")
                    exact_deadline=time.monotonic()+60
                    while True:
                        advance_sources(pages)
                        exact=call(spectator,'multiplayerSmoke.spectatorTick()')['playback']
                        if exact['position']==90 and not exact['seeking']:break
                        assert time.monotonic()<exact_deadline,('exact frame90 seek timeout',exact)
                assert call(spectator,"()=>multiplayerSmoke.spectatorPlaybackControl('pause')")
                break
            assert time.monotonic()<deadline,('spectator checkpoint timeout',spectator_value)
            pages[0].wait_for_timeout(2)

        viewer_at_90=snapshot(spectator)
        report['earlyPlayerFrames']={str(frame):value[0] for frame,value in player_timeline.items()}
        early_restore={}
        for target in (1,2,3,5,10):
            assert call(spectator,f"()=>multiplayerSmoke.spectatorPlaybackControl('seek',{target})")
            seek_deadline=time.monotonic()+30
            while True:
                advance_sources(pages)
                sample=call(spectator,'multiplayerSmoke.spectatorTick()')
                if sample['playback']['position']==target and not sample['playback']['seeking']:break
                assert time.monotonic()<seek_deadline,('early checkpoint seek timeout',target,sample['playback'])
            expected=player_timeline.get(target-1,[None])[0]
            actual={'portable':sample['portable'],'canonical':sample['canonical'],'stageScene':sample['stageScene'],
                    'stageFields':call(spectator,'multiplayerSmoke.stageSceneFields()'),
                    'enemyDebug':call(spectator,'multiplayerSmoke.enemyDebug()'),
                    'eclContextDebug':call(spectator,'multiplayerSmoke.eclContextDebug()'),
                    'spectatorParityDebug':call(spectator,'multiplayerSmoke.spectatorParityDebug()'),
                    'spectatorOptionsDebug':call(spectator,'multiplayerSmoke.spectatorOptionsDebug()'),
                    'spectatorEntityCounts':call(spectator,'multiplayerSmoke.spectatorEntityCounts()'),
                    'spectatorCommonDebug':call(spectator,'multiplayerSmoke.spectatorCommonDebug()'),
                    'spectatorScoresDebug':call(spectator,'multiplayerSmoke.spectatorScoresDebug()'),
                    'spectatorAnmDebug':call(spectator,'multiplayerSmoke.spectatorAnmDebug()')}
            expected_snap=snap_next_values(expected) if expected else None
            actual_snap=snap_next_values(actual)
            snap_match=expected_snap==actual_snap
            assert snap_match,('snap_next restore mismatch',target,expected_snap,actual_snap)
            early_restore[str(target)]={'playerFrame':target-1,'diff':hash_diff(expected,actual) if expected else None,
                                        'snapNextMatch':snap_match,'playerSnapNext':expected_snap,
                                        'viewerSnapNext':actual_snap,'player':expected,'viewer':actual}
            report['earlyRestore']=early_restore
            assert expected is not None,('missing exact early source snapshot',target)
            assert_hash_equal(expected,actual,f'early seek position {target}')
        assert call(spectator,"()=>multiplayerSmoke.spectatorPlaybackControl('seek',90)")
        seek_deadline=time.monotonic()+30
        while True:
            advance_sources(pages)
            seek_sample=call(spectator,'multiplayerSmoke.spectatorTick()')
            if seek_sample['playback']['position']==90 and not seek_sample['playback']['seeking']:break
            assert time.monotonic()<seek_deadline,('return to frame90 timeout',seek_sample['playback'])
        assert call(spectator,"()=>multiplayerSmoke.spectatorPlaybackControl('pause')")
        report['earlyRestore']=early_restore
        report['restoreHashes']={'sourcesAt89':[value['portable'] for value in host_at_89],
                                 'diffAllSourcesToViewer':[hash_diff(value,viewer_at_90) for value in host_at_89],
                                 'player0At89':host_at_89[0]['portable'],
                                 'player1At89':host_at_89[1]['portable'],
                                 'viewerAt90':viewer_at_90['portable'],
                                 'diffPlayer0ToViewer':hash_diff(host_at_89[0],viewer_at_90),
                                 'diffPlayer1ToViewer':hash_diff(host_at_89[1],viewer_at_90),
                                 'canonicalPlayer0At89':host_at_89[0]['canonical'],
                                 'canonicalViewerAt90':viewer_at_90['canonical'],
                                 'enemyDebugPlayers':[call(page,'multiplayerSmoke.enemyDebug()') for page in pages],
                                 'enemyDebugViewer':call(spectator,'multiplayerSmoke.enemyDebug()')}
        assert_sources_equal(host_at_89,'player frame 89')
        report['frame90MatchesHost89']=hash_diff(host_at_89[0],viewer_at_90)
        hashes=[value['canonical'] for value in host_at_89]+[viewer_at_90['canonical']]
        portable=[value['portable'] for value in host_at_89]+[viewer_at_90['portable']]
        states=[value['state'] for value in host_at_89]+[viewer_at_90['state']]
        audio=[value['audio'] for value in host_at_89]+[viewer_at_90['audio']]
        enemies=[call(page,'multiplayerSmoke.enemyDebug()') for page in pages]
        spectator_status=call(spectator,'multiplayerSmoke.spectatorStatus()')
        transport=call(spectator,'multiplayerSmoke.transportStatus()')
        replay=call(spectator,'multiplayerSmoke.replayStatus()')
        playback=call(spectator,'multiplayerSmoke.spectatorPlaybackState()')
        peer_state=call(spectator,"()=>({route:globalThis.__eaglerPeerTransport?.route||'',peers:globalThis.__eaglerPeerTransport?.peers?.size||0,canSend:typeof globalThis.__eaglerPeerTransport?.send==='function'||typeof globalThis.__eaglerPeerTransport?.sendTo==='function'})")
        report.update({'room':room,'checkpointFrame':89,'playerFrame44':at_44,'playerFrame89':host_at_89,
                       'viewerFrame90':viewer_at_90,'states':states,'hashes':hashes,
                       'portableHashes':portable,'audio':audio,
                       'enemyDebug':enemies,'spectatorStatus':spectator_status,
                       'spectatorTransport':transport,'spectatorPeerState':peer_state,
                       'spectatorPlayback':playback,'writeProbe':write_probe})
        # Exact-layout hashes deliberately include graph pointers and therefore
        # differ for the separately bootstrapped spectator. The portable
        # oracle covers gameplay, ECL, authored ANM, RNG and lifecycle state.
        assert len({value[1] for value in portable[:args.players]})==1,(portable,hashes,states,enemies)
        assert playback['position']==90 and playback['edge']>=90 and playback['received']>=180,playback
        assert spectator_status[1]==1 and transport[13]==3,(spectator_status,transport)
        assert peer_state=={'route':'spectator','peers':0,'canSend':False},peer_state
        assert replay[1]==0 and replay[2]==0 and replay[9]==1,replay

        phase='spectator-pause-and-back-seek'
        paused_before=call(spectator,'multiplayerSmoke.spectatorPlaybackState()')
        for _ in range(12):
            advance_sources(pages)
            paused_after=call(spectator,'multiplayerSmoke.spectatorTick()')['playback']
        assert paused_after['paused'] and paused_after['position']==paused_before['position'],(paused_before,paused_after)
        assert call(spectator,"()=>multiplayerSmoke.spectatorPlaybackControl('seek',45)")
        deadline=time.monotonic()+60
        while True:
            advance_sources(pages)
            seek_value=call(spectator,'multiplayerSmoke.spectatorTick()')
            current=seek_value['playback']
            if current['position']==45 and not current['seeking']:break
            assert time.monotonic()<deadline,('back seek timeout',current)
        viewer_at_45={'portable':seek_value['portable'],'canonical':seek_value['canonical'],'stageScene':seek_value['stageScene']}
        back_seek_diff=hash_diff(at_44[0],viewer_at_45)
        report['backSeekHashDiff']=back_seek_diff
        report['backSeekHashes']={'playerAt44':at_44[0],'viewerAt45':viewer_at_45}
        report['backSeek']={'position':current['position'],'target':current['target'],
                            'portable':seek_value['portable'],'canonical':seek_value['canonical'],
                            'matchesPlayerFrame44':not back_seek_diff['portable'] and not back_seek_diff['canonical']}

        phase='players-advance-and-live'
        target_last_simulated=599
        live_player_history={}
        target_snapshot=None
        deadline=time.monotonic()+300
        while True:
            player_states=[call(page,'multiplayerSmoke.netStatus()') for page in pages]
            if target_snapshot is None and target_last_simulated in live_player_history:
                target_snapshot=live_player_history[target_last_simulated]
            player_ready=all(value[3]>=target_last_simulated and value[5]==-1 for value in player_states)
            current=call(spectator,'multiplayerSmoke.spectatorPlaybackState()')
            counts=call(pages[0],'multiplayerSmoke.spectatorEntityCounts()')
            has_live_projectiles=counts[1]>0 and counts[2]>0
            if target_snapshot and has_live_projectiles and current['edge']>=target_last_simulated+1:break
            advance_sources(pages)
            call(spectator,'multiplayerSmoke.spectatorTick()')
            assert time.monotonic()<deadline,('enemy/projectile live edge timeout',current,counts,player_states)
            pages[0].wait_for_timeout(2)
        assert target_snapshot and target_snapshot[0]['net'][3]==target_last_simulated and \
               target_snapshot[1]['net'][3]==target_last_simulated,('missing exact frame-599 host snapshot',target_snapshot)
        assert call(spectator,"()=>multiplayerSmoke.spectatorPlaybackControl('seek',600)")
        deadline=time.monotonic()+120
        while True:
            live_value=call(spectator,'multiplayerSmoke.spectatorTick()')
            current=live_value['playback']
            if current['position']==600 and not current['seeking']:break
            assert time.monotonic()<deadline,('frame-600 seek timeout',current)
        viewer_at_600=snapshot(spectator)
        report['frame600']={'playerFrame':target_last_simulated,'spectatorPosition':current['position'],
                            'player0':target_snapshot[0],'player1':target_snapshot[1],'viewer':viewer_at_600,
                            'portableMatch':target_snapshot[0]['portable']==viewer_at_600['portable'],
                            'itemDebug':{'player':target_snapshot[0]['spectatorItemDebug'],
                                         'viewer':viewer_at_600['spectatorItemDebug'],
                                         'diff':item_debug_diffs(target_snapshot[0]['spectatorItemDebug'],
                                                                 viewer_at_600['spectatorItemDebug'])}}
        assert_sources_equal(target_snapshot,'host frame 599')
        assert_hash_equal(target_snapshot[0],viewer_at_600,'spectator frame 600 vs host frame 599')
        assert_enemy_ecl_equal(target_snapshot[0],viewer_at_600,'spectator frame 600 vs host frame 599')
        assert snap_next_values(target_snapshot[0])==snap_next_values(viewer_at_600),(
            'frame-600 snap_next mismatch',snap_next_values(target_snapshot[0]),snap_next_values(viewer_at_600))
        assert target_snapshot[0]['spectatorEntityCounts']==viewer_at_600['spectatorEntityCounts'],(
            'frame-600 entity count mismatch',target_snapshot[0]['spectatorEntityCounts'],viewer_at_600['spectatorEntityCounts'])
        assert target_snapshot[0]['canonical'][13:17]==viewer_at_600['canonical'][13:17],(
            'frame-600 RNG mismatch',target_snapshot[0]['canonical'][13:17],viewer_at_600['canonical'][13:17])
        report['frame600'].update({'snapNext':snap_next_values(target_snapshot[0]),
                                   'entityCounts':viewer_at_600['spectatorEntityCounts'],
                                   'gameAndVisualRng':viewer_at_600['canonical'][13:17]})
        edge=current['edge']
        assert call(spectator,"()=>multiplayerSmoke.spectatorPlaybackControl('live')")
        live_start_edge=edge;live_start_position=current['position'];max_live_lag=0;live_trace=[]
        live_rounds=120;player_frames_per_viewer_tick=2
        for round_index in range(live_rounds):
            for _ in range(player_frames_per_viewer_tick):advance_sources(pages)
            live_value=call(spectator,'multiplayerSmoke.spectatorTick()')
            current=live_value['playback'];lag=current['edge']-current['position']
            max_live_lag=max(max_live_lag,lag)
            live_trace.append({'round':round_index,'position':current['position'],'edge':current['edge'],'lag':lag})
            assert current['following'] and not current['seeking'],('live mode lost follow',current)
            assert lag>=0,('playback advanced beyond received edge',current)
            assert lag<=12,('live spectator fell behind moving edge',current,'maxLag',max_live_lag)
        deadline=time.monotonic()+120
        while True:
            player_states=[call(page,'multiplayerSmoke.netStatus()') for page in pages]
            current=call(spectator,'multiplayerSmoke.spectatorPlaybackState()')
            if (current['position']==current['edge'] and current['following'] and not current['seeking']
                and all(value[3]>=current['edge']-1 for value in player_states)):
                break
            live_value=call(spectator,'multiplayerSmoke.spectatorTick()');current=live_value['playback']
            assert time.monotonic()<deadline,('live edge settle timeout',current,'players',player_states)
        edge=current['edge']
        final_target_frame=edge-1
        final_players=live_player_history.get(final_target_frame)
        assert final_players and all(value['net'][3]==final_target_frame for value in final_players),(
            'missing exact host snapshot for live edge',final_target_frame,
            sorted(live_player_history)[-8:])
        final_viewer=snapshot(spectator)
        report['liveFollowCandidate']={'position':current['position'],'edge':current['edge'],
                                       'targetLastSimulated':final_target_frame,
                                       'playerPortable':[v['portable'] for v in final_players],
                                       'viewerPortable':final_viewer['portable'],
                                       'playerEntityCounts':[v['spectatorEntityCounts'] for v in final_players],
                                       'viewerEntityCounts':final_viewer['spectatorEntityCounts'],
                                       'bulletDebug':{'player':final_players[0]['spectatorBulletDebug'],
                                                      'player1':final_players[1]['spectatorBulletDebug'],
                                                      'viewer':final_viewer['spectatorBulletDebug'],
                                                      'playerViewerDiff':bullet_debug_diffs(
                                                          final_players[0]['spectatorBulletDebug'],
                                                          final_viewer['spectatorBulletDebug'])}}
        report['liveFollowCandidate']['eclContextDebug']={'player':final_players[0]['eclContextDebug'],
                                                          'viewer':final_viewer['eclContextDebug']}
        assert_sources_equal(final_players,f'players frame {edge-1}')
        assert_hash_equal(final_players[0],final_viewer,f'live spectator frame {edge}')
        assert_enemy_ecl_equal(final_players[0],final_viewer,f'live spectator frame {edge}')
        player_snap=snap_next_values(final_players[0]);viewer_snap=snap_next_values(final_viewer)
        assert player_snap==viewer_snap,('live snap_next mismatch',player_snap,viewer_snap)
        player_counts=final_players[0]['spectatorEntityCounts']
        viewer_counts=final_viewer['spectatorEntityCounts']
        assert player_counts==viewer_counts,('live entity counts mismatch',player_counts,viewer_counts)
        assert player_counts[1]>0 and player_counts[2]>0,('no active enemy/projectile at comparison frame',player_counts)
        player_rng=final_players[0]['canonical'][13:17]
        viewer_rng=final_viewer['canonical'][13:17]
        assert player_rng==viewer_rng,('game/visual RNG mismatch',player_rng,viewer_rng)
        common_differences=group_diffs(final_players[0]['spectatorCommonDebug'],
                                       final_viewer['spectatorCommonDebug'],common_debug_names)
        score_differences=group_diffs(final_players[0]['spectatorScoresDebug'],
                                      final_viewer['spectatorScoresDebug'],score_debug_names)
        component_names=['schema','composite','economy','rng','players','enemies','bullets','lasers','items','anm','lifecycle','replay']
        report['liveFollow']={'position':current['position'],'edge':current['edge'],
                              'following':current['following'],'portable':live_value['portable'],
                              'movingEdge':{'startEdge':live_start_edge,'startPosition':live_start_position,
                                            'endEdge':current['edge'],'endPosition':current['position'],
                                            'playerFramesPerViewerTick':player_frames_per_viewer_tick,
                                            'rounds':live_rounds,'maxLag':max_live_lag,'trace':live_trace},
                              'targetLastSimulated':edge-1,'playerLastSimulated':[v['net'][3] for v in final_players],
                              'latestHostLastSimulated':[call(page,'multiplayerSmoke.netStatus()')[3] for page in pages],
                              'player0':final_players[0],'player1':final_players[1],'viewer':final_viewer,
                              'portableComponents':dict(zip(component_names,final_viewer['portable'])),
                              'gameAndVisualRng':{'player':player_rng,'viewer':viewer_rng},
                              'enemyAndProjectileCounts':{'player':player_counts,'viewer':viewer_counts},
                              'snapNext':{'player':player_snap,'viewer':viewer_snap,'matches':player_snap==viewer_snap},
                              'commonDebug':{'player':final_players[0]['spectatorCommonDebug'],
                                             'viewer':final_viewer['spectatorCommonDebug'],
                                             'differences':common_differences},
                              'scoreFieldHashes':{'player':final_players[0]['spectatorScoresDebug'],
                                                  'viewer':final_viewer['spectatorScoresDebug'],
                                                  'differences':score_differences},
                              'canonicalDiagnostics':[
                                  {'index':i,'field':canonical_field_names.get(i,f'canonical word {i}'),
                                   'player':final_players[0]['canonical'][i],'viewer':final_viewer['canonical'][i]}
                                  for i in range(44) if final_players[0]['canonical'][i]!=final_viewer['canonical'][i]
                                  and i not in semantic_canonical_words]}

        phase='native-stage2-transition'
        before_stage2_frame=[call(page,'multiplayerSmoke.netStatus()')[3] for page in pages]
        assert len(set(before_stage2_frame))==1,('Stage1 transition frontier mismatch',before_stage2_frame)
        fixture_results=[call(page,'()=>multiplayerSmoke.fixture(6)') for page in pages]
        assert len(fixture_results)==args.players and all(fixture_results),('native complete-stage fixture rejected',fixture_results,
                                             [call(page,'multiplayerSmoke.nativeStatus()') for page in pages])
        transition_initial_lifecycle=[call(page,'multiplayerSmoke.lifecycle()') for page in pages]
        transition_deadline=time.monotonic()+90
        while True:
            for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()')
            # Keep already-arrived peers at this exact boundary while the
            # remaining native resource owners finish loading.
            boundary=[]
            for page in pages:
                life=call(page,'multiplayerSmoke.lifecycle()')
                boundary.append({'error':0,'arrived':True} if life[1]==7 and life[2]==7
                                else call(page,'multiplayerSmoke.fixtureTransitionTick()'))
            assert all(value['error']==0 for value in boundary),('native stage transition error',boundary)
            lifecycle=[call(page,'multiplayerSmoke.lifecycle()') for page in pages]
            if all(value[1]==7 and value[2]==7 for value in lifecycle):break
            assert time.monotonic()<transition_deadline,('native Stage2 screen fence timeout',lifecycle,boundary)
            pages[0].wait_for_timeout(2)
        transition_frame=[call(page,'multiplayerSmoke.netStatus()')[3] for page in pages]
        assert len(set(transition_frame))==1 and all(
            call(page,'multiplayerSmoke.netStatus()')[4]==transition_frame[index]
            for index,page in enumerate(pages)),('native transition frames were not mutually confirmed',transition_frame)
        transition_snapshots=[snapshot(page) for page in pages]
        # The retail stage-clear/screen-loading path can keep confirmed native
        # frames running until the next GameSession is ready. Retain that actual
        # boundary state rather than comparing it with the previous live edge.
        live_player_history[transition_frame[0]]=transition_snapshots
        stage2_deadline=time.monotonic()+90
        while True:
            advance_sources(pages)
            call(spectator,'multiplayerSmoke.spectatorTick()')
            native=[call(page,'multiplayerSmoke.nativeStatus()') for page in pages]
            host_frames=[call(page,'multiplayerSmoke.netStatus()')[3] for page in pages]
            if all(value[1]==2 for value in native) and all(
                value>before_stage2_frame[index] for index,value in enumerate(host_frames)):
                break
            assert time.monotonic()<stage2_deadline,('native Stage2 first tick timeout',native)
        stage2_node=None
        node_deadline=time.monotonic()+90
        while stage2_node is None:
            current=call(spectator,'multiplayerSmoke.spectatorPlaybackState()')
            stage2_node=next((node for node in current.get('nodes',[]) if node.get('stage')==2),None)
            if stage2_node is None:
                advance_sources(pages);call(spectator,'multiplayerSmoke.spectatorTick()')
                assert time.monotonic()<node_deadline,('Stage2 checkpoint was not published',current)
        stage2_checkpoint_frame=stage2_node['frame']
        stage2_target_frame=stage2_checkpoint_frame
        assert stage2_target_frame in live_player_history,('Stage2 source frame fell out of history',
            stage2_target_frame,sorted(live_player_history)[-10:])
        stage2_host=live_player_history[stage2_target_frame]
        assert all(value['state'][6]==2 for value in stage2_host),(
            'Stage2 checkpoint did not restore the native Stage2 world',stage2_target_frame,
            [value['state'][6] for value in stage2_host])
        assert_sources_equal(stage2_host,f'players at Stage2 frame {stage2_target_frame}')
        report['stage2Transition']={'lastStage1ComparisonFrame':max(value['net'][3] for value in final_players),
            'frontierImmediatelyBeforeFixture':before_stage2_frame,
            'initialLifecycle':transition_initial_lifecycle,
            'framesDuringNativeScreenFence':[transition_frame[i]-before_stage2_frame[i] for i in range(args.players)],
            'frontierAfterScreenFence':transition_frame,
            'screenFence':lifecycle,'firstStage2NativeStatus':[call(page,'multiplayerSmoke.nativeStatus()') for page in pages],
            'checkpointNode':stage2_node,'hostFrame':stage2_target_frame,
            'hostPortable':[value['portable'] for value in stage2_host]}

        phase='stage2-independent-latejoin'
        late_spectator_id='spectator_0004'
        late_start=admit_late(setup,room,late_spectator_id)
        assert late_start['serial']==1,('late spectator joined wrong run',late_start)
        if args.mode=='relay':late_context.add_init_script("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true});")
        late_url=f"{args.relay}/?room={room}&run=1&member=member_{late_spectator_id}"
        assert call(late_viewer,'v=>multiplayerSmoke.spectatorConnect(...v)',[late_url,late_spectator_id])
        late_deadline=time.monotonic()+180
        late_stage2_node=None
        while late_stage2_node is None:
            current=call(late_viewer,'multiplayerSmoke.spectatorPlaybackState()')
            if current:
                late_stage2_node=next((node for node in current.get('nodes',[]) if node.get('stage')==2),None)
            if late_stage2_node is not None:break
            advance_sources(pages)
            call(late_viewer,'multiplayerSmoke.spectatorTick()')
            assert time.monotonic()<late_deadline,('late spectator did not receive Stage2 checkpoint',current,
                                                   call(late_viewer,'multiplayerSmoke.spectatorStatus()'))
        report['stage2LateJoin']={'lobby':late_start,'checkpointNode':late_stage2_node,
            'spectatorStatus':call(late_viewer,'multiplayerSmoke.spectatorStatus()'),
            'transportStatus':call(late_viewer,'multiplayerSmoke.transportStatus()')}

        stage2_position=stage2_target_frame+1
        phase='stage2-buffered-edge-arrival'
        buffer_deadline=time.monotonic()+180
        while True:
            views=[call(page,'multiplayerSmoke.spectatorPlaybackState()') for page in [spectator,late_viewer]]
            if all(value['edge']>=stage2_position for value in views):break
            advance_sources(pages)
            for page in [spectator,late_viewer]:call(page,'multiplayerSmoke.spectatorTick()')
            assert time.monotonic()<buffer_deadline,('Stage2 target did not clear the 90-frame relay buffer',
                                                      stage2_position,views)

        # The original viewer was already following Stage1. No playback control
        # has been sent since that live check: it must enter Stage2 on its own,
        # before the explicit history/seek tests below can conceal a handoff bug.
        phase='stage2-automatic-follow'
        automatic_deadline=time.monotonic()+120
        automatic_trace=[]
        while True:
            current=call(spectator,'multiplayerSmoke.spectatorPlaybackState()')
            assert current['following'] and not current['paused'] and not current['seeking'],(
                'automatic stage transition lost following',current)
            automatic_trace.append({key:current[key] for key in ('position','edge','following','paused','seeking')})
            if current['position']>=stage2_position:
                automatic_last=current['position']-1
                automatic_sources=live_player_history.get(automatic_last)
                assert automatic_sources,('missing source frame for automatic stage transition',automatic_last)
                automatic_viewer=snapshot(spectator)
                assert_sources_equal(automatic_sources,f'automatic Stage2 sources frame {automatic_last}')
                assert automatic_viewer['state'][6]==2,('automatic viewer did not construct Stage2',automatic_viewer['state'])
                assert_hash_equal(automatic_sources[0],automatic_viewer,f'automatic Stage2 frame {automatic_last}')
                assert_enemy_ecl_equal(automatic_sources[0],automatic_viewer,f'automatic Stage2 frame {automatic_last}')
                assert snap_next_values(automatic_sources[0])==snap_next_values(automatic_viewer),(
                    'automatic Stage2 snap_next mismatch',automatic_last)
                report['automaticStageFollow']={'passed':True,'checkpoint':stage2_node,
                    'position':current['position'],'sourceLastInput':automatic_last,
                    'sourceCount':len(automatic_sources),'trace':automatic_trace,
                    'portable':automatic_viewer['portable']}
                break
            call(spectator,'multiplayerSmoke.spectatorTick()')
            assert time.monotonic()<automatic_deadline,('automatic Stage2 follow timed out',current)

        def seek_viewer(page,target,label,deadline_seconds=120):
            assert call(page,f"()=>multiplayerSmoke.spectatorPlaybackControl('seek',{target})"),('seek rejected',label,target)
            deadline=time.monotonic()+deadline_seconds
            while True:
                sample=call(page,'multiplayerSmoke.spectatorTick()')
                current=sample['playback']
                if current['position']==target and not current['seeking']:
                    return {'sample':sample,'snapshot':snapshot(page),'playback':current}
                assert time.monotonic()<deadline,('seek timed out',label,target,current)

        phase='cross-stage-seek-back-to-stage1'
        old_stage1=seek_viewer(spectator,90,'original viewer Stage1 seek')
        assert_hash_equal(host_at_89[0],old_stage1['snapshot'],'original viewer Stage1 seek after Stage2')
        late_history=call(late_viewer,'multiplayerSmoke.spectatorPlaybackState()')
        assert late_history['min']==stage2_checkpoint_frame,('new viewer must bootstrap the current stage',late_history)
        report['crossStageStage1Seek']={'targetPosition':90,
            'originalDiff':hash_diff(host_at_89[0],old_stage1['snapshot']),
            'lateViewerMin':late_history['min']}

        phase='cross-stage-seek-to-stage2'
        old_stage2=seek_viewer(spectator,stage2_position,'original viewer Stage2 seek')
        late_stage2=seek_viewer(late_viewer,stage2_position,'late viewer Stage2 seek')
        report['crossStageStage2SeekCandidate']={'playerFrame':stage2_target_frame,
            'playerPortable':stage2_host[0]['portable'],'playerCanonical':stage2_host[0]['canonical'],
            'portableNames':['schema','composite','economy','rng','players','enemies',
                'bullets','lasers','items','anm','lifecycle','replay'],
            'playerStatus':stage2_host[0]['state'],'playerSemanticHash':stage2_host[0]['portable'][4],
            'originalViewerPortable':old_stage2['snapshot']['portable'],
            'lateViewerPortable':late_stage2['snapshot']['portable'],
            'originalViewerCanonical':old_stage2['snapshot']['canonical'],
            'lateViewerCanonical':late_stage2['snapshot']['canonical'],
            'originalDiff':hash_diff(stage2_host[0],old_stage2['snapshot']),
            'lateViewerDiff':hash_diff(stage2_host[0],late_stage2['snapshot']),
            'playerOptions':stage2_host[0]['spectatorOptionsDebug'],
            'originalOptions':old_stage2['snapshot']['spectatorOptionsDebug'],
            'lateOptions':late_stage2['snapshot']['spectatorOptionsDebug'],
            'playerEntities':stage2_host[0]['spectatorEntityCounts'],
            'originalEntities':old_stage2['snapshot']['spectatorEntityCounts'],
            'lateEntities':late_stage2['snapshot']['spectatorEntityCounts'],
            'playerEnemyDebug':stage2_host[0]['enemyDebug'],
            'originalEnemyDebug':old_stage2['snapshot']['enemyDebug'],
            'lateEnemyDebug':late_stage2['snapshot']['enemyDebug'],
            'playerEclContext':stage2_host[0]['eclContextDebug'],
            'originalEclContext':old_stage2['snapshot']['eclContextDebug'],
            'lateEclContext':late_stage2['snapshot']['eclContextDebug'],
            'playerItemDebug':stage2_host[0]['spectatorItemDebug'],
            'originalItemDebug':old_stage2['snapshot']['spectatorItemDebug'],
            'lateItemDebug':late_stage2['snapshot']['spectatorItemDebug'],
            'playerBulletDebug':stage2_host[0]['spectatorBulletDebug'],
            'originalBulletDebug':old_stage2['snapshot']['spectatorBulletDebug'],
            'lateBulletDebug':late_stage2['snapshot']['spectatorBulletDebug']}
        for label,actual in [('original viewer',old_stage2['snapshot']),('late viewer',late_stage2['snapshot'])]:
            assert_hash_equal(stage2_host[0],actual,label+' Stage2 native checkpoint restore')
            assert_enemy_ecl_equal(stage2_host[0],actual,label+' Stage2 native checkpoint restore')
            assert snap_next_values(stage2_host[0])==snap_next_values(actual),(
                label,'Stage2 snap_next mismatch',snap_next_values(stage2_host[0]),snap_next_values(actual))
        report['crossStageStage2Seek']={'targetPosition':stage2_position,
            'originalDiff':hash_diff(stage2_host[0],old_stage2['snapshot']),
            'lateViewerDiff':hash_diff(stage2_host[0],late_stage2['snapshot']),
            'playerShots':stage2_host[0]['portable'][4],
            'originalEntityCounts':old_stage2['snapshot']['spectatorEntityCounts'],
            'lateViewerEntityCounts':late_stage2['snapshot']['spectatorEntityCounts']}

        phase='stage2-all-future-frames'
        for page in [spectator,late_viewer]:
            assert call(page,"()=>multiplayerSmoke.spectatorPlaybackControl('pause')")
            assert call(page,"()=>multiplayerSmoke.spectatorPlaybackControl('play')")
        stage2_future_target=stage2_checkpoint_frame+600
        report['stage2FutureFrames']={'startPosition':stage2_position,'ticks':0,'emptyEdgeWaits':0,
            'sourcePlayers':args.players,'independentViewers':2,'firstMismatch':None}
        stage2_deadline=time.monotonic()+300
        for offset in range(1,601):
            advance_sources(pages)
            expected_position=stage2_position+offset
            expected=live_player_history[expected_position-1]
            assert_sources_equal(expected,f'Stage2 sources at position {expected_position}')
            for index,page in enumerate([spectator,late_viewer]):
                while True:
                    current=call(page,'multiplayerSmoke.spectatorTick()')['playback']
                    assert not current['following'] and not current['seeking'] and not current['paused'],(
                        'Historical Play changed playback mode',index,expected_position,current)
                    if current['position']==expected_position:break
                    # The relay may not have delivered the newly confirmed
                    # source input yet. A presentation at the current edge
                    # consumes zero inputs; an available input must consume one.
                    assert current['position']==expected_position-1 and current['edge']<expected_position,(
                        'Historical Play must consume one available input at most per presentation',
                        index,expected_position,current)
                    report['stage2FutureFrames']['emptyEdgeWaits']+=1
                    assert time.monotonic()<stage2_deadline,('Stage2 buffered input arrival timeout',
                        index,expected_position,current)
                actual=call(page,"""()=>({portable:multiplayerSmoke.portableCanonical(),stageScene:multiplayerSmoke.stageSceneCanonical(),
                    canonical:multiplayerSmoke.canonical(),enemyDebug:multiplayerSmoke.enemyDebug(),
                    eclContextDebug:multiplayerSmoke.eclContextDebug()})""")
                try:
                    assert_hash_equal(expected[0],actual,f'Stage2 position {expected_position} viewer {index}')
                    assert_enemy_ecl_equal(expected[0],actual,f'Stage2 position {expected_position} viewer {index}')
                except AssertionError:
                    report['stage2FutureFrames']['firstMismatch']={'position':expected_position,
                        'viewer':index,'diff':hash_diff(expected[0],actual),'viewerSnapshot':snapshot(page)}
                    raise
            report['stage2FutureFrames']['ticks']=offset
            assert time.monotonic()<stage2_deadline,('Stage2 600-frame comparison timeout',expected_position)
        stage2_future_host=live_player_history[stage2_future_target]
        stage2_future_views=[snapshot(spectator),snapshot(late_viewer)]
        for index,actual in enumerate(stage2_future_views):
            assert_hash_equal(stage2_future_host[0],actual,f'Stage2 frame {stage2_future_target} viewer {index}')
            assert_enemy_ecl_equal(stage2_future_host[0],actual,f'Stage2 frame {stage2_future_target} viewer {index}')
            assert snap_next_values(stage2_future_host[0])==snap_next_values(actual),(
                'Stage2 future snap_next mismatch',index,snap_next_values(stage2_future_host[0]),snap_next_values(actual))

        # The original viewer has now actually played the restored Stage2
        # world for 600 ticks. Exercise the old archive from that world, then
        # reconstruct Stage2 again before returning to sustained live play.
        phase='stage2-to-stage1-round-trip'
        assert stage2_future_views[0]['state'][6]==2,('round trip must start in Stage2',stage2_future_views[0]['state'])
        back_after_stage2=seek_viewer(spectator,90,'Stage1 after 600 Stage2 ticks')
        assert_hash_equal(host_at_89[0],back_after_stage2['snapshot'],'Stage1 after 600 Stage2 ticks')
        forward_after_stage1=seek_viewer(spectator,stage2_future_target+1,'return to Stage2 after Stage1')
        assert_hash_equal(stage2_future_host[0],forward_after_stage1['snapshot'],'Stage2 after Stage1 round trip')
        assert_enemy_ecl_equal(stage2_future_host[0],forward_after_stage1['snapshot'],'Stage2 after Stage1 round trip')
        assert snap_next_values(stage2_future_host[0])==snap_next_values(forward_after_stage1['snapshot']),(
            'Stage2 after Stage1 round trip snap_next mismatch')
        report['crossStageRoundTripAfterFuture']={
            'fromStage2Position':stage2_future_target+1,'stage1Position':90,
            'stage1Diff':hash_diff(host_at_89[0],back_after_stage2['snapshot']),
            'stage2Diff':hash_diff(stage2_future_host[0],forward_after_stage1['snapshot'])}

        phase='stage2-sustained-live'
        for page in [spectator,late_viewer]:assert call(page,"()=>multiplayerSmoke.spectatorPlaybackControl('live')")
        for page in [spectator,late_viewer]:
            deadline=time.monotonic()+120
            while True:
                current=call(page,'multiplayerSmoke.spectatorTick()')['playback']
                if current['following'] and not current['seeking'] and current['position']==current['edge']:break
                assert time.monotonic()<deadline,('Stage2 return to live timeout',current)
        stage2_live_trace=[];stage2_live_max_lag=[0,0];stage2_host_buffered_lag=[0,0]
        for round_index in range(60):
            for _ in range(2):advance_sources(pages)
            for index,page in enumerate([spectator,late_viewer]):
                current=call(page,'multiplayerSmoke.spectatorTick()')['playback']
                lag=current['edge']-current['position']
                stage2_live_max_lag[index]=max(stage2_live_max_lag[index],lag)
                assert current['following'] and not current['seeking'] and 0<=lag<=12,(
                    'Stage2 moving live edge fell behind',index,current)
                host_next=call(pages[0],'multiplayerSmoke.netStatus()')[2]
                buffered_lag=host_next-current['position']-SPECTATOR_BUFFER_FRAMES
                stage2_host_buffered_lag[index]=max(stage2_host_buffered_lag[index],buffered_lag)
                assert 0<=buffered_lag<=12,('Stage2 extra live backlog',index,host_next,current)
                expected=live_player_history[current['position']-1]
                actual=call(page,"()=>({portable:multiplayerSmoke.portableCanonical(),stageScene:multiplayerSmoke.stageSceneCanonical(),canonical:multiplayerSmoke.canonical()})")
                for source in expected:assert_hash_equal(source,actual,f'Stage2 live viewer {index}')
            stage2_live_trace.append({'round':round_index,'position':current['position'],
                'edge':current['edge'],'sourceNextFrame':host_next})
        for page in [spectator,late_viewer]:
            deadline=time.monotonic()+120
            while True:
                current=call(page,'multiplayerSmoke.spectatorTick()')['playback']
                if current['position']==current['edge']:break
                assert time.monotonic()<deadline,('Stage2 final live drain timeout',current)
        report['stage2SustainedLive']={'sourceFrames':120,'maxExtraLag':stage2_live_max_lag}
        report['stage2FutureFrame']={'target':stage2_future_target,
            'playerPortable':[value['portable'] for value in stage2_future_host],
            'viewerPortable':[value['portable'] for value in stage2_future_views],
            'originalDiff':hash_diff(stage2_future_host[0],stage2_future_views[0]),
            'lateViewerDiff':hash_diff(stage2_future_host[0],stage2_future_views[1]),
            'snapNext':snap_next_values(stage2_future_host[0]),
            'entityCounts':stage2_future_host[0]['spectatorEntityCounts'],
            'sourceFramesAdvanced':stage2_future_target-stage2_checkpoint_frame,
            'maxLiveLag':stage2_live_max_lag,
            'maxHostLagBeyondRelayBuffer':stage2_host_buffered_lag,
            'liveTraceSamples':stage2_live_trace[::max(1,len(stage2_live_trace)//20)]}
        assert not report['errors'],report['errors']
        passed=(not report['frame90MatchesHost89']['portable'] and
                not report['frame90MatchesHost89']['canonical'] and
                not report['frame90MatchesHost89']['stageScene'] and
                not back_seek_diff['portable'] and not back_seek_diff['canonical'] and
                not back_seek_diff['stageScene'] and
                current['position']==current['edge'] and max_live_lag<=12 and
                not report['crossStageStage1Seek']['originalDiff']['portable'] and
                not report['crossStageStage1Seek']['originalDiff']['stageScene'] and
                not report['crossStageStage2Seek']['originalDiff']['portable'] and
                not report['crossStageStage2Seek']['originalDiff']['stageScene'] and
                not report['crossStageStage2Seek']['lateViewerDiff']['portable'] and
                not report['crossStageStage2Seek']['lateViewerDiff']['stageScene'] and
                not report['stage2FutureFrame']['originalDiff']['portable'] and
                not report['stage2FutureFrame']['originalDiff']['stageScene'] and
                not report['stage2FutureFrame']['lateViewerDiff']['portable'] and
                not report['stage2FutureFrame']['lateViewerDiff']['stageScene'])
        report.update({'passed':passed,'readOnly':True,
                       'semanticCanonicalWords':sorted(semantic_canonical_words),
                       'canonicalDiagnosticFieldNames':canonical_field_names})
        print(f"TH10 spectator {args.mode}: {'PASS' if passed else 'CONTROL PASS, HASH DIVERGENCE'} restored=90 seek=45 live={edge} hash={portable[0][1]}",flush=True)
    except Stage2OnlyComplete:
        pass
    except BaseException as error:
        report['failure']=str(error)
        report['failurePhase']=phase;report['elapsedSeconds']=time.monotonic()-started_at
        report['failurePeers']=[]
        for page in dict.fromkeys([*pages,spectator,late_viewer]):
            try:
                peer=call(page,"()=>{const p=globalThis.__eaglerPeerTransport;let playback=null;try{playback=multiplayerSmoke.spectatorPlaybackState()}catch{}return {net:multiplayerSmoke.netStatus(),status:multiplayerSmoke.status(),native:multiplayerSmoke.nativeStatus(),lifecycle:multiplayerSmoke.lifecycle(),spectatorLoad:multiplayerSmoke.spectatorLoadDebug(),transport:multiplayerSmoke.transportStatus(),spectator:multiplayerSmoke.spectatorStatus(),playback,error:multiplayerSmoke.networkError(),errorDetail:multiplayerSmoke.errorDetail(),restoreDiagnostic:multiplayerSmoke.spectatorRestoreDiagnostic(),memory:multiplayerSmoke.memoryStatus(),peer:p?{route:p.route,failed:p.failed,error:p.error,relayReady:p.relay?.readyState,spectatorStopped:p.spectatorStopped,retain:p.retainSpectatorHistory,received:p.received?.length||0,head:p.receivedHead||0,receivedBytes:p.receivedBytes||0}:null}}")
                load=peer.get('spectatorLoad') or []
                phases={1:'replay',2:'stage',3:'gui',4:'player',5:'bullets',6:'items',7:'lasers',8:'results',
                        9:'text-overlay',10:'score-popups',11:'enemies',12:'effects',13:'bomb',14:'spell',
                        15:'music',16:'timer-reset',17:'pending-upload',18:'complete'}
                peer['sessionLoadPhase']=phases.get(load[4],'unknown') if len(load)>4 else 'unavailable'
                report['failurePeers'].append(peer)
            except Exception as diagnostic_error:report['failurePeers'].append({'diagnosticError':str(diagnostic_error)})
        raise
    finally:
        for context in contexts:
            try:context.close()
            except Exception:pass
        browser.close();args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
