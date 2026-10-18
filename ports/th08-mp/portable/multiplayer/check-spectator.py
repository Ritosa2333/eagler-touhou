"""Actual TH08 Runtime shell, admitted observer stream, and native read-only path."""
import argparse
import base64
import hashlib
import json
import os
import time
import uuid
from pathlib import Path
from urllib.parse import parse_qs,urlparse
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--relay',required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--mode',choices=['spectator-rtc','spectator-relay'],required=True)
parser.add_argument('--players',choices=['2','3','all'],default='all')
parser.add_argument('--frames',type=int,default=239)
parser.add_argument('--timing',choices=['pure','hybrid'],default='pure')
parser.add_argument('--stage-two-controls',action='store_true')
parser.add_argument('--first-diff-only',action='store_true')
parser.add_argument('--old-viewer-cross-stage',action='store_true')
parser.add_argument('--lifecycle-reentry',action='store_true')
args=parser.parse_args()
assert args.frames>=239
if args.stage_two_controls:assert args.frames>=900,'Stage-two controls need at least 600 frames after the native Stage 2 checkpoint'
if args.first_diff_only:assert args.stage_two_controls,'First-difference probe requires Stage 2 setup'
if args.old_viewer_cross_stage:assert not args.stage_two_controls and not args.first_diff_only,'Old-viewer cross-stage proof is a separate case'
if args.lifecycle_reentry:
    assert args.players=='2','Lifecycle reentry is a focused two-player case'
    assert not (args.stage_two_controls or args.first_diff_only or args.old_viewer_cross_stage),\
        'Lifecycle reentry is a separate case from stage-two and old-viewer validation'
adonis_mode=1 if args.timing=='pure' else 2
input_overrides={}
report={'passed':False,'scope':'Production TH08 admitted read-only spectator using actual Runtime host protocol',
        'mode':args.mode,'cases':[],'errors':[]}
requested_case='lifecycle-reentry' if args.lifecycle_reentry else \
    'old-viewer-cross-stage' if args.old_viewer_cross_stage else \
    'stage-two-controls' if args.stage_two_controls else 'default'
expected_phase='repeated-runtime-admission-and-idle-pause' if args.lifecycle_reentry else \
    'old-viewer-stage-one-admission' if args.old_viewer_cross_stage else \
    'stage-two-late-admission' if args.stage_two_controls else 'delayed-admission'
report['spectatorRequest']={'case':requested_case,'stageTwoControls':args.stage_two_controls,
    'oldViewerCrossStage':args.old_viewer_cross_stage,'firstDiffOnly':args.first_diff_only,
    'lifecycleReentry':args.lifecycle_reentry,
    'frames':args.frames,'expectedPhase':expected_phase}
input_data=Path(os.environ.get('TH08_MP_DATA',''))
if input_data.is_file():
    data=input_data.read_bytes()
    report['inputData']={'path':str(input_data.resolve()),'bytes':len(data),
                         'sha256':hashlib.sha256(data).hexdigest()}

def save():args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
def call(page,code,arg=None):return page.evaluate(code,arg)
def request(page,command,**fields):return call(page,'v=>host.request(v.command,v.fields)',{'command':command,'fields':fields})
def snapshot(page,full=True):return call(page,'full=>host.snapshot(full)',full)
def input_at(seat,frame):
    if (seat,frame) in input_overrides:return input_overrides[(seat,frame)]
    if frame<180:return 0
    return 1|(64 if (seat+frame//12)%2 else 128)|(4 if frame%30<15 else 0)|(2 if frame==215 else 0)
def signed32(value):return value-(1<<32) if value&(1<<31) else value
def restore_metadata(native):
    return {'checkpointFrame':native[44],'wireStage':native[45],
            'checkpointState':{'stage':native[46],'difficulty':signed32(native[47]),
                'character':signed32(native[48]),'spell':signed32(native[49])},
            'loadRequest':{'stage':signed32(native[50]),'difficulty':signed32(native[51]),
                'character':signed32(native[52]),'spell':signed32(native[53]),
                'supervisorState':signed32(native[54]),'allowedStateRange':[native[55],native[56]]}}
def player_field_name(field):
    names={1:'PlayerSimulationState.analog',2:'PlayerSimulationState.touch_remainder',
        3:'PlayerSimulationState.unlimited_movement_used',4:'PlayerSimulationState.motion',
        5:'PlayerSimulationState.life',6:'PlayerSimulationState.context',7:'PlayerSimulationState.bomb',
        8:'PlayerSimulationState.bomb_input.unmapped_bytes',9:'PlayerSimulationState.shots',
        10:'PlayerSimulationState.bomb_objects',11:'PlayerSimulationState.frame',
        12:'PlayerSimulationState.input',13:'PlayerSimulationState.enemy_origin',
        14:'PlayerSimulationState.cancel_item',15:'PlayerSimulationState.item_gauge_lock',
        16:'PlayerSimulationState.stage_play_frames',90:'PlayerState trace metadata'}
    bomb_names={101:'buttons',102:'previous_buttons',103:'gui_blocked',104:'tampered',
        105:'cheats',106:'reserved',107:'regular_bombs',108:'last_spells'}
    if field in bomb_names:return 'PlayerSimulationState.bomb_input.'+bomb_names[field]
    if 109<=field<125:return 'PlayerSimulationState.bomb_input.bosses['+str(field-109)+']'
    context_names={201:'game_flags',202:'hud_flags',203:'pause',204:'bombs',205:'lives',
        206:'power',207:'time_orbs',208:'last_spell_requirement',209:'extent',210:'replay_flags',
        211:'character',212:'focused',213:'time_spell',214:'game_over',215:'miss_control',
        216:'cheats',217:'gauge',218:'padding',219:'challenge_mode'}
    if field in context_names:return 'PlayerSimulationState.context.'+context_names[field]
    return names.get(field,'unmapped PlayerSimulationState field '+str(field))
def supervisor_field_name(field):
    return {1:'supervisor.state.active',2:'supervisor.state.target',3:'game.globals.stage',
        10:'game.control.state.frames',11:'game.control.state.restore_viewport',
        12:'game.control.state.next_scene',13:'game.control.state.load_state',
        14:'game.control.state.load_frames',15:'game.control.state.start_music',
        16:'game.control.state.slow_frames',17:'game.control.state.play_frames',
        18:'game.control.state.demo_frames',19:'game.control.state.demo_index',
        20:'game.control.state.replay_mode',21:'game.control.state.stage_mask',
        22:'game.control.state.sticky_input',23:'game.control.state.loading_images_setup',
        30:'game.time_stopped',31:'game.paused',32:'game.retrying',
        40:'supervisor.input.current',41:'supervisor.input.previous',
        42:'supervisor.input.scrolling',43:'supervisor.input.held',
        90:'unnamed/padding byte'} .get(field,'unmapped supervisor field '+str(field))
def byte_difference_report(observer,players,frame,groups,playback_state,source_capture=None):
    viewer_bytes=call(observer,'()=>host.canonicalBytesNow()')
    source_bytes=([capture['groups'] for capture in source_capture] if source_capture is not None else
                  [call(page,'f=>host.canonicalBytesAt(f)',frame) for page in players])
    byte_diffs=[]
    for group,group_name in ((1,'players'),(8,'supervisor-control')):
        if group_name not in groups:continue
        by_seat=[source.get(str(group)) if source else None for source in source_bytes]
        p1=by_seat[0] if by_seat else None
        p2=by_seat[1] if len(by_seat)>1 else None
        actual=viewer_bytes.get(str(group)) if viewer_bytes else None
        item={'group':group_name,'groupId':group,
            'sourceBytesAvailable':bool(by_seat) and all(value is not None for value in by_seat),
            'viewerBytesAvailable':actual is not None,'sourceLengths':[len(value or []) for value in by_seat],
            'viewerLength':len(actual or [])}
        if by_seat and all(value is not None for value in by_seat) and actual is not None:
            reference=by_seat[0]
            source_divergence=[]
            for seat,value in enumerate(by_seat[1:],1):
                offset=next((i for i,(a,b) in enumerate(zip(reference,value)) if a!=b),None)
                source_divergence.append(None if offset is None else {
                    'seat':seat,'offset':offset,'reference':reference[offset],'seatValue':value[offset],
                    'p1':reference[offset],'p2':value[offset]})
            viewer_diffs=[]
            for seat,value in enumerate(by_seat):
                offset=next((i for i,(a,b) in enumerate(zip(value,actual)) if a!=b),None)
                if offset is None:
                    viewer_diffs.append(None);continue
                info=call(observer,'v=>host.canonicalFieldInfo(v.group,v.offset)',
                    {'group':group,'offset':offset})
                difference={'seat':seat,'offset':offset,'sourceValue':value[offset],
                    'viewer':actual[offset],'fieldInfo':info}
                if group==1 and info:
                    difference.update({'field':player_field_name(info[0]),
                        'memberByteOffset':info[1],'seat':info[2]})
                elif group==8 and info:
                    difference.update({'field':supervisor_field_name(info[0]),'memberByteOffset':info[1]})
                viewer_diffs.append(difference)
            item['sourceDivergenceBySeat']=source_divergence
            item['sourceViewerFirstByteDifferenceBySeat']=viewer_diffs
            item['firstP1P2ByteDifference']=source_divergence[0] if source_divergence else None
            item['firstSourceViewerByteDifference']=next((value for value in viewer_diffs if value is not None),None)
            item['sourceLengthsP1P2']=[len(p1 or []),len(p2 or [])]
        byte_diffs.append(item)
    return {'frame':frame,'playbackAtDifference':playback_state,'groups':byte_diffs}

def decode_capture(capture):
    capture['groups']={str(group):base64.b64decode(value) for group,value in capture['groups'].items()}
    return capture

def decode_checkpoint_capture(capture):
    if not capture:return None
    return {**capture,'groups':{str(group):base64.b64decode(value)
        for group,value in capture.get('groups',{}).items()}}

def capture_summary(captures):
    return [{'nextFrame':capture['net'][3],'canonical':capture['canonical'],
        'rng':capture['rng'],
        'groups':{group:{'length':len(data),'sha256':hashlib.sha256(data).hexdigest()}
            for group,data in capture['groups'].items()}} for capture in captures]

with sync_playwright() as pw:
    browser=pw.chromium.launch(headless=True,args=['--use-angle=d3d11'])
    try:
        for count in ([2,3] if args.players=='all' else [int(args.players)]):
            contexts=[];pages=[];case={'players':count,'passed':False,'phase':'boot',
                'requestedCase':requested_case,'expectedPhase':expected_phase}
            runtime_sockets={}
            source_checkpoint_captures=None
            def runtime_socket_query(record):
                return parse_qs(urlparse(record['url']).query)
            def runtime_socket_records(endpoint):
                return runtime_sockets.get(endpoint,[])
            def player_data_socket_records(seat):
                return [record for record in runtime_socket_records(seat)
                    if runtime_socket_query(record).get('player')==[str(seat)] and
                    'signal' not in runtime_socket_query(record)]
            def player_data_sockets(seat):
                return [dict(record) for record in player_data_socket_records(seat)]
            def observer_data_socket_records():
                return [record for record in runtime_socket_records(count)
                    if runtime_socket_query(record).get('spectator')==[observer_id] and
                    'signal' not in runtime_socket_query(record)]
            def observer_data_sockets():
                return [dict(record) for record in observer_data_socket_records()]
            def runtime_sockets_snapshot():
                return {endpoint:[dict(record) for record in records]
                    for endpoint,records in runtime_sockets.items()}
            def connection_status_snapshot():
                return {'players':{seat:player_data_sockets(seat) for seat in range(count)},
                    'observer':observer_data_sockets()}
            report['cases'].append(case);save()
            try:
                # Prepare all shell instances before the lobby starts its
                # finite observer connection window. The observer game itself
                # is deliberately launched after the players reach frame 89.
                for endpoint in range(count+1):
                    context=browser.new_context(service_workers='block',viewport={'width':640,'height':480});contexts.append(context)
                    if args.mode=='spectator-relay':
                        context.add_init_script("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true})")
                    page=context.new_page();pages.append(page)
                    runtime_sockets[endpoint]=[]
                    def observe_runtime_socket(socket,owner=endpoint):
                        if 'run=' not in socket.url:return
                        record={'url':socket.url,'closed':False}
                        runtime_sockets[owner].append(record)
                        socket.on('close',lambda _,row=record:row.update(closed=True))
                    page.on('websocket',observe_runtime_socket)
                    page.on('pageerror',lambda error:report['errors'].append(str(error)))
                    page.goto(args.url+'host.html');call(page,'host.open()')
                    page.wait_for_function('host.ready()',timeout=120000)
                    if endpoint==0:
                        case['webglRenderer']=call(page,"()=>{const c=document.createElement('canvas');const gl=c.getContext('webgl2')||c.getContext('webgl');if(!gl)return 'unavailable';const e=gl.getExtension('WEBGL_debug_renderer_info');return e?gl.getParameter(e.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER)}")
                players=pages[:count];observer=pages[-1]
                wire_probe={'entry':0,'packets':{},'closed':{}}
                observer_host_iframe=call(observer,'host.iframeState()') if args.lifecycle_reentry else None
                if args.lifecycle_reentry:
                    assert observer_host_iframe and observer_host_iframe['creationCount']==1,{
                        'observerHostIframe':observer_host_iframe}
                    def observe_spectator_socket(socket):
                        if 'run=' not in socket.url:return
                        entry=wire_probe['entry']
                        wire_probe['packets'].setdefault(entry,[])
                        wire_probe['closed'].setdefault(entry,False)
                        def record_frame(payload):
                            if not isinstance(payload,(bytes,bytearray)):return
                            packet=bytes(payload);kind=None;frame=None;session=None
                            if packet[:4]==b'E8TM':
                                kind='timing';session=int.from_bytes(packet[12:20],'little') if len(packet)>=20 else None
                            elif packet[:4]==b'E8SV' and len(packet)>=32:
                                kind={1:'checkpoint',2:'node',3:'end',4:'bootstrap'}.get(packet[5],'stage-unknown')
                                frame=int.from_bytes(packet[20:24],'little');session=int.from_bytes(packet[8:16],'little')
                            elif packet[:4]==b'E8NP' and len(packet)>=24 and packet[5]==3:
                                kind='input';frame=int.from_bytes(packet[16:20],'little')
                                session=int.from_bytes(packet[8:16],'little')
                            if kind:
                                wire_probe['packets'][entry].append({'kind':kind,'frame':frame,
                                    'sessionId':session,'bytes':len(packet)})
                        socket.on('framereceived',record_frame)
                        socket.on('close',lambda _:wire_probe['closed'].__setitem__(entry,True))
                    observer.on('websocket',observe_spectator_socket)
                room='th08mp-audit-'+uuid.uuid4().hex[:16];observer_id='observer_1234'
                started=call(players[0],'v=>host.lobby(...v)',[args.relay,room,count,observer_id,{'adonisMode':adonis_mode,'inputDelay':2,'inputDelayAuto':False,'predictionReserve':2}])
                assert started['room']['spectatorCount']==1,started
                url=args.relay+'/?room='+room+'&run='+str(started['serial'])
                common={'runtimeVariant':'multiplayer','netplayMode':'lan','netplayUrl':url,
                        'netplayPlayerCount':count,'netplayDifficulty':1,'netplaySeed':1234,
                        'netplayLoadouts':[{'character':seat,'shot':0} for seat in range(count)],
                        'netplayAdonisMode':adonis_mode,'netplayInputDelay':2,'netplayInputDelayAuto':False,'netplayPredictionLimit':8,'netplayPredictionReserve':2,'netplaySpectatorCount':1,'netplayIceServers':[],
                        'touchEnabled':True,'touchMovementMode':'touch','touchSensitivity':100,'thpracEnabled':True}
                for seat,page in enumerate(players):
                    request(page,'configure',options={**common,'netplayPlayer':seat,'netplayUrl':url+'&member=fixture_host_contract_p'+str(seat+1)},music='none')
                    request(page,'launch')
                def advance(target,watch=False):
                    deadline=time.monotonic()+(360 if args.stage_two_controls else 120);iterations=0
                    while True:
                        source_values=[]
                        for seat,page in enumerate(players):
                            s=snapshot(page,False);n=s['net']
                            if n[3]<=target:
                                if n[2]:call(page,'v=>host.buttons(v)',input_at(seat,n[3]))
                                source_values.append(call(page,'t=>host.tickTo(t,false)',target))
                            else:
                                # The inclusive target is already captured. Do
                                # not poll a stopped source while another seat
                                # catches up; its player watchdog is still live.
                                source_values.append(s)
                        values=list(source_values)
                        viewer_playback=None
                        if watch:
                            viewer_value=call(observer,'t=>host.tickTo(t,false)',target)
                            values.append(viewer_value)
                            viewer_playback=call(observer,'host.nativePlayback()')
                        iterations+=1
                        sources_done=all(s['net'][4]==target and s['net'][5]>=target and s['net'][6]==0xffffffff
                            for s in source_values)
                        viewer_done=not watch or (viewer_playback is not None and
                            viewer_playback['received']>=target+1)
                        if sources_done and viewer_done:
                            # Hash the stopped, fully confirmed checkpoint once.
                            # Progress polling need not hash the entire native world.
                            return [snapshot(page) for page in players+([observer] if watch else [])],iterations
                        assert time.monotonic()<deadline,{'targetLastFrame':target,'sources':source_values,
                            'viewerPlayback':viewer_playback,'viewerSnapshot':snapshot(observer,False) if watch else None}
                        players[0].wait_for_timeout(1)
                def drive_sources_to(target,expected_next):
                    for seat,page in enumerate(players):
                        call(page,'v=>host.autoStart(v.seat,v.target,true)',{'seat':seat,'target':target})
                    captures=[]
                    for seat,page in enumerate(players):
                        deadline=time.monotonic()+120;status=None
                        while True:
                            status=call(page,'host.autoStatus()')
                            assert not status or not status['error'],{'seat':seat,'sourceAutoDrive':status}
                            if status and status['stopped']:
                                capture=call(page,'host.autoCapture()')
                                assert capture and capture['net'][3]==expected_next,{'seat':seat,
                                    'expectedNextFrame':expected_next,'sourceStatus':status,'capture':capture}
                                captures.append(decode_capture(capture))
                                break
                            current=status['net'][3] if status else None
                            assert current is None or current<=expected_next,{'seat':seat,
                                'expectedNextFrame':expected_next,'sourceStatus':status}
                            assert time.monotonic()<deadline,{'seat':seat,'expectedNextFrame':expected_next,
                                'sourceStatus':status}
                            page.wait_for_timeout(1)
                    return captures
                def lifecycle_entry(target,values):
                    entry=wire_probe['entry'];packets=wire_probe['packets'].get(entry,[])
                    counts={name:sum(packet['kind']==name for packet in packets)
                        for name in ('timing','checkpoint','bootstrap','input')}
                    assert all(counts[name]>0 for name in counts),{'entry':entry,'wirePacketCounts':counts,
                        'wirePackets':packets[-32:]}
                    session_ids={packet['sessionId'] for packet in packets if packet['sessionId'] is not None}
                    assert len(session_ids)==1,{'entry':entry,'wireSessionIds':sorted(session_ids),
                        'wirePacketCounts':counts}
                    # Packet receipt only proves transport admission. The first
                    # native report is intentionally unavailable until the
                    # required checkpoint restore has completed, so keep
                    # stepping the healthy observer while sources remain at a
                    # fixed, fully captured boundary.
                    deadline=time.monotonic()+60;native=None;viewer=None
                    while True:
                        # Poll and execute at most one native callback, then
                        # read the playback cursor and canonical bytes inside
                        # the same synchronous host.snapshot call. This keeps
                        # a browser task boundary from pairing Position N with
                        # a canonical snapshot from N+1.
                        viewer=call(observer,'host.spectatorAdvanceOnce(true)')
                        native=viewer.get('spectatorPlayback')
                        if native and native['position']==0 and not native['seeking'] and \
                                not native['paused'] and not viewer['error'] and \
                                viewer['spectator'] and viewer['spectator'][1]==1 and \
                                viewer['spectator'][7]==1:
                            break
                        assert time.monotonic()<deadline,{'entry':entry,'targetLastFrame':target,
                            'nativePlayback':native,'nativeStatus':viewer,
                            'observerDiagnostics':call(observer,'host.diagnostics()'),
                            'spectatorDiagnostics':call(observer,'host.spectatorDiagnostics()'),
                            'sourceNextFrames':[snapshot(page,False)['net'][3] for page in players]}
                        observer.wait_for_timeout(5)
                    values=[snapshot(page) for page in players]+[viewer]
                    assert native and native['received']>=target+1 and not viewer['error'] and \
                        viewer['spectator'] and viewer['spectator'][1]==1 and viewer['spectator'][7]==1,{
                        'entry':entry,'targetLastFrame':target,'nativePlayback':native,
                        'nativeStatus':viewer}
                    assert values[0]['canonical'] and all(value['canonical']==values[0]['canonical']
                        for value in values[:count]),{'entry':entry,'sourceBoundary':values[:count]}
                    timeline_samples=[]
                    sample=viewer
                    for frame in range(5):
                        native=sample.get('spectatorPlayback')
                        assert native and native['position']==frame and not native['seeking'] and \
                            not native['paused'],{'entry':entry,'expectedPosition':frame,
                                'atomicSample':sample}
                        source_by_seat=[call(page,'f=>host.canonicalAt(f)',frame) for page in players]
                        row={'position':frame,'sourceCanonicalBySeat':source_by_seat,
                            'viewerCanonical':sample['canonical'],'nativePlayback':native,
                            'canonicalReadAtomicWithPosition':True}
                        row['sourceSeatsAgree']=bool(source_by_seat[0]) and all(
                            value==source_by_seat[0] for value in source_by_seat)
                        row['viewerMatchesSource']=row['sourceSeatsAgree'] and \
                            sample['canonical']==source_by_seat[0]
                        timeline_samples.append(row)
                        assert row['viewerMatchesSource'],{'entry':entry,
                            'firstDifferencePosition':frame,'timelineSamples':timeline_samples}
                        if frame<4:
                            sample=call(observer,'host.spectatorAdvanceOnce(true)')
                    native=sample['spectatorPlayback']
                    source_at_position=timeline_samples[-1]['sourceCanonicalBySeat']
                    sockets=observer_data_sockets()
                    assert sockets and not sockets[-1]['closed'],{'entry':entry,'runtimeSockets':sockets}
                    return {'entry':entry,'targetLastFrame':target,'run':native['run'],
                        'receivedNextFrame':native['received'],'position':native['position'],
                        'edge':native['edge'],'sourceCanonicalAtPosition':source_at_position[0],
                        'atomicTimelineSamples0To4':timeline_samples,
                        'wirePacketCounts':counts,
                        'wirePacketSample':packets[:12],'sessionId':next(iter(session_ids)),
                        'runtimeSocketOpen':True,'sourceViewerCanonicalEqual':True}
                case['phase']='delayed-admission';save()
                if args.stage_two_controls:
                    before,_=advance(179)
                    assert all(s['driver'][12]==(1 if args.mode=='spectator-rtc' else 2) for s in before),before
                    setups=[call(page,'host.stageClearSetup()') for page in players]
                    assert all(setups),{'stageClearSetup':setups,'values':before}
                    # The Stage 2 checkpoint can be published at the first
                    # safe post-transition frame (currently 187), before the
                    # fixture's frame-300 stage-clear setup. Retain its source
                    # canonical state as well as the entry and future frames.
                    tracked=list(range(180,args.frames+180))
                    for page in players:
                        call(page,'v=>host.trackFrames(v)',tracked)
                        call(page,'v=>host.trackCanonicalBytes(v)',[187])
                    source_217_captures=None
                    if args.stage_two_controls:
                        # Capture the source oracle at the exact CP boundary:
                        # NextFrame 187 means input 187 has not been consumed.
                        # Keep decoded canonical bytes in this Python process
                        # so later seeks cannot lose the source state to a
                        # browser timeline-map overwrite or off-by-one lookup.
                        source_checkpoint_captures=drive_sources_to(186,187)
                        assert all(capture['net'][3]==187 for capture in source_checkpoint_captures),{
                            'expectedPreInputCheckpointNextFrame':187,
                            'captures':capture_summary(source_checkpoint_captures)}
                        assert all(capture['canonical']==source_checkpoint_captures[0]['canonical']
                            for capture in source_checkpoint_captures),{
                                'sourceCheckpointFrame':187,'captures':capture_summary(source_checkpoint_captures)}
                        for group in ('1','8'):
                            assert all(group in capture['groups'] for capture in source_checkpoint_captures),{
                                'sourceCheckpointFrame':187,'missingGroup':group,
                                'captures':capture_summary(source_checkpoint_captures)}
                            assert all(capture['groups'][group]==source_checkpoint_captures[0]['groups'][group]
                                for capture in source_checkpoint_captures),{
                                    'sourceCheckpointFrame':187,'group':group,
                                    'captures':capture_summary(source_checkpoint_captures)}
                        case['sourceBoundaryCaptures']={'checkpointBoundary':'NextFrame 187, before input 187',
                            'checkpoint':capture_summary(source_checkpoint_captures)};save()
                    if args.first_diff_only:
                        source_217_captures=drive_sources_to(216,217)
                        assert all(capture['canonical']==source_217_captures[0]['canonical']
                            for capture in source_217_captures),{
                                'sourceCanonicalFrame':217,'captures':capture_summary(source_217_captures)}
                        for group in ('1','8'):
                            assert all(group in capture['groups'] for capture in source_217_captures),{
                                'sourceCanonicalFrame':217,'missingGroup':group,
                                'captures':capture_summary(source_217_captures)}
                            assert all(capture['groups'][group]==source_217_captures[0]['groups'][group]
                                for capture in source_217_captures),{
                                    'sourceCanonicalFrame':217,'group':group,
                                    'captures':capture_summary(source_217_captures)}
                        case['sourceBoundaryCaptures']['firstDifferenceCandidate']=capture_summary(source_217_captures)
                        save()
                    stage_two,_=advance(300)
                    assert all(s['state'][6]==1 for s in stage_two),{'expectedNativeStage2':stage_two}
                    case['stageTwoVerification']={'frame':stage_two[0]['net'][4],'canonical':[s['canonical'] for s in stage_two],
                        'stageClearFixtureSetup':setups}
                    case['phase']='stage-two-late-admission';save()
                    # Keep confirmed input flowing while a slow software
                    # renderer catches the late viewer up and exercises seek.
                    # Source driving is stopped explicitly after live follow.
                elif args.old_viewer_cross_stage:
                    # Keep the Stage 1 origin and transition/future frames in
                    # both player timelines so the old-viewer case can prove
                    # a genuine cross-segment seek after Stage 2 arrives.
                    tracked=list(range(0,args.frames+181))
                    for page in players:call(page,'v=>host.trackFrames(v)',tracked)
                    before,_=advance(89)
                    source_stage_one_capture=decode_checkpoint_capture(
                        call(players[0],'host.spectatorCheckpointCapture()'))
                elif args.lifecycle_reentry:
                    tracked=list(range(0,args.frames+1))
                    for page in players:call(page,'v=>host.trackFrames(v)',tracked)
                    before,_=advance(119)
                else:
                    before,_=advance(89)
                expected_route=1 if args.mode=='spectator-rtc' else 2
                assert all(s['driver'][12]==expected_route for s in before),before
                # A pre-existing imported record is never changed by observer
                # initialization, gameplay, results or exit, even if decoding
                # the old record yields the native empty-record fallback.
                sentinel=[84,72,48,56,0,0,0,0]+[0]*120
                request(observer,'write',path='score.dat',bytes=sentinel)
                if args.stage_two_controls:call(observer,'v=>host.trackFrames(v)',tracked)
                request(observer,'configure',options={**common,'netplayPlayer':-1,'netplaySpectator':True,'netplaySpectatorId':observer_id,'netplayUrl':url+'&member=fixture_'+observer_id},music='none')
                request(observer,'launch')
                writes=call(observer,'host.spectatorWriteProbe()')
                assert writes=={'local':0,'analog':0,'ready':0,'hello':0,'packet':0,'wire':1,'practice':0},writes
                for page in (players[0],observer):
                    rejection=call(page,"async()=>{try{await host.request('resources',{resources:[]});return '';}catch(e){return String(e);}}")
                    assert 'Cannot replace resources' in rejection,rejection
                case['liveResourceReplacementRejected']=True
                assert request(observer,'read',path='score.dat')['bytes']==sentinel
                if args.old_viewer_cross_stage:
                    case['phase']='old-viewer-stage-one-admission';save()
                    def old_playback_until(label,predicate,timeout=180):
                        deadline=time.monotonic()+timeout;last=None
                        while True:
                            call(observer,'t=>host.tickTo(t,false)',0xfffffffe)
                            last=call(observer,'host.nativePlayback()')
                            if not last:
                                native=call(observer,'host.spectatorDiagnostics()')
                                details={'waitingFor':label,'nativePlayback':last,'diagnostics':native}
                                if label in ('Stage 1 checkpoint report','restore Stage 1 checkpoint') and native and native[2]:
                                    checkpoint_canonical=native[18:31]
                                    captured_canonical=(source_stage_one_capture or {}).get('canonical')
                                    details['sourceCheckpointCapture']={
                                        'frame':(source_stage_one_capture or {}).get('frame'),
                                        'canonical':captured_canonical,
                                        'groupLengths':{group:len(value) for group,value in
                                            (source_stage_one_capture or {}).get('groups',{}).items()},
                                        'groupSha256':{group:hashlib.sha256(value).hexdigest() for group,value in
                                            (source_stage_one_capture or {}).get('groups',{}).items()}}
                                    details['embeddedCheckpointCanonical']=checkpoint_canonical
                                    details['sourceCaptureMatchesEmbeddedCheckpoint']=(
                                        source_stage_one_capture is not None and
                                        source_stage_one_capture.get('frame')==0 and
                                        captured_canonical==checkpoint_canonical)
                                    if details['sourceCaptureMatchesEmbeddedCheckpoint']:
                                        details['checkpointFirstByteDifference']=byte_difference_report(
                                            observer,players,0,['players','supervisor-control'],None,
                                            [source_stage_one_capture])
                                if native and native[2]:
                                    case['stageOneRestoreFailure']=details;save()
                                    raise AssertionError(details)
                                if time.monotonic()>=deadline:
                                    case['stageOneRestoreTimeout']=details;save()
                                    raise AssertionError(details)
                                observer.wait_for_timeout(1)
                                continue
                            if predicate(last):return last
                            assert time.monotonic()<deadline,{'waitingFor':label,'nativePlayback':last,
                                'diagnostics':call(observer,'host.spectatorDiagnostics()')}
                            observer.wait_for_timeout(1)
                    stage_one=old_playback_until('Stage 1 checkpoint report',
                        lambda p:any(n['kind']=='stage' and n['stage']%256==0 for n in p['nodes']))
                    stage_one_nodes=[n for n in stage_one['nodes'] if n['kind']=='stage' and n['stage']%256==0]
                    assert stage_one_nodes,stage_one
                    stage_one_frame=min(n['frame'] for n in stage_one_nodes)
                    assert stage_one_frame==0,{'expectedInitialStageOneCheckpoint':0,'playback':stage_one}
                    native_stage_one=call(observer,'host.spectatorDiagnostics()')
                    assert native_stage_one and native_stage_one[44]==stage_one_frame,{
                        'stageOneFrame':stage_one_frame,'nativeDiagnostics':native_stage_one}
                    embedded_stage_one=native_stage_one[18:31]
                    assert source_stage_one_capture and source_stage_one_capture['frame']==stage_one_frame and \
                        source_stage_one_capture['canonical']==embedded_stage_one,{
                            'sourceCheckpointCaptureMismatch':True,'sourceCheckpointCapture':{
                                'frame':source_stage_one_capture.get('frame') if source_stage_one_capture else None,
                                'canonical':source_stage_one_capture.get('canonical') if source_stage_one_capture else None},
                            'embeddedCheckpointCanonical':embedded_stage_one,
                            'nativeDiagnostics':native_stage_one}
                    case['stageOneCheckpointCapture']={'frame':source_stage_one_capture['frame'],
                        'canonicalMatchesEmbeddedCheckpoint':True,
                        'groupLengths':{group:len(value) for group,value in source_stage_one_capture['groups'].items()},
                        'groupSha256':{group:hashlib.sha256(value).hexdigest() for group,value in
                            source_stage_one_capture['groups'].items()}};save()
                    source_stage_one_by_player=[call(page,'f=>host.canonicalAt(f)',stage_one_frame)
                        for page in players]
                    source_stage_one=source_stage_one_by_player[0]
                    assert source_stage_one and all(value==source_stage_one
                        for value in source_stage_one_by_player),{
                        'stageOneCheckpointFrame':stage_one_frame,
                        'sourceCanonicalByPlayer':source_stage_one_by_player}
                    assert request(observer,'spectator-playback',run=stage_one['run'],action='pause')
                    paused_stage_one=old_playback_until('pause Stage 1 viewer',lambda p:p['paused'])
                    assert request(observer,'spectator-playback',run=paused_stage_one['run'],
                        action='seek',frame=stage_one_frame)
                    at_stage_one=old_playback_until('restore Stage 1 checkpoint',
                        lambda p:p['paused'] and not p['seeking'] and p['position']==stage_one_frame)
                    viewer_stage_one=snapshot(observer)['canonical']
                    assert viewer_stage_one==source_stage_one,{'stageOneCheckpointFrame':stage_one_frame,
                        'sourceCanonical':source_stage_one,'viewerCanonical':viewer_stage_one,
                        'playback':at_stage_one}
                    case['phase']='stage-one-checkpoint-strict-pass'
                    case['stageOneRestore']={'frame':stage_one_frame,
                        'sourceCanonicalByPlayer':source_stage_one_by_player,
                        'viewerCanonical':viewer_stage_one}
                    save()
                    assert request(observer,'spectator-playback',run=at_stage_one['run'],action='live')
                    following_stage_one=old_playback_until('follow Stage 1 before transition',
                        lambda p:p['following'] and not p['seeking'] and not p['paused'] and
                            p['position']>=stage_one_frame)

                    # This is a real in-session transition: both source games
                    # reach frame 179 before the fixture invokes native
                    # stageClearSetup, then continue through Stage 2. The
                    # observer stays Following across the segment boundary;
                    # no playback command may manually select the next node.
                    transition_before,_=advance(179,False)
                    stage_clear=[call(page,'host.stageClearSetup()') for page in players]
                    assert all(stage_clear),{'stageClearSetup':stage_clear,
                        'sourceFrame':transition_before[0]['net'][4]}
                    for page in players:
                        call(page,'v=>host.trackFrames(v)',list(range(180,args.frames+181)))
                    transition_after,_=advance(300,False)
                    assert all(s['state'][6]==1 for s in transition_after[:count]),{
                        'expectedNativeStageTwo':transition_after}
                    stage_two=old_playback_until('old viewer receives Stage 2 checkpoint',
                        lambda p:any(n['kind']=='stage' and n['stage']%256==1 for n in p['nodes']))
                    stage_two_nodes=[n for n in stage_two['nodes'] if n['kind']=='stage' and n['stage']%256==1]
                    assert stage_two_nodes,stage_two
                    stage_two_checkpoints=[n for n in stage_two_nodes
                        if n['frame']>stage_one_frame and n['frame']<transition_after[0]['net'][4]]
                    assert stage_two_checkpoints,{'stageTwoNodes':stage_two_nodes,
                        'stageClearSetupAfterFrame':transition_before[0]['net'][4],
                        'stageTwoVerifiedAtSourceFrame':transition_after[0]['net'][4],
                        'playback':stage_two}
                    checkpoint_ids={n['id'] for n in stage_two_checkpoints}
                    checkpoint_frames={n['frame'] for n in stage_two_checkpoints}
                    checkpoint_stage_keys={n['stage'] for n in stage_two_checkpoints}
                    assert len(checkpoint_ids)==len(checkpoint_frames)==len(checkpoint_stage_keys)==1,{
                        'ambiguousStageTwoCheckpointIdentity':stage_two_checkpoints}
                    stage_two_node=stage_two_checkpoints[0]
                    stage_two_frame=stage_two_node['frame']
                    stage_two_wire_key=stage_two_node['stage']
                    stage_two_route=(stage_two_wire_key>>8)&0xff
                    assert (stage_two_wire_key&0xff)==1 and stage_two_route<=2,{
                        'invalidStageTwoWireIdentity':stage_two_node}
                    source_stage_two_checkpoint_by_player=[
                        call(page,'f=>host.canonicalAt(f)',stage_two_frame) for page in players]
                    assert source_stage_two_checkpoint_by_player and all(value and len(value)==13 and value[0]==1 and
                        value==source_stage_two_checkpoint_by_player[0]
                        for value in source_stage_two_checkpoint_by_player),{
                            'stageTwoCheckpointSourceCaptureUnavailableOrDivergent':{
                                'frame':stage_two_frame,
                                'sourceCanonicalByPlayer':source_stage_two_checkpoint_by_player}}
                    case['phase']='stage-two-checkpoint-identified-awaiting-automatic-follow'
                    case['stageTwoCheckpointIdentity']={'frame':stage_two_frame,
                        'serial':stage_two_node['id'],'serialEvidence':'received playback node id',
                        'wireStageKey':stage_two_wire_key,'nativeStage':stage_two_wire_key&0xff,
                        'routeFromWireKey':stage_two_route}
                    case['stageTwoSourceCanonicalAtCheckpointByPlayer']=source_stage_two_checkpoint_by_player
                    save()
                    live_stage_two=old_playback_until('automatic Following transition into Stage 2',
                        lambda p:p['following'] and not p['seeking'] and not p['paused'] and
                            p['position']>=stage_two_frame+1 and any(
                                n['id']==stage_two_node['id'] and n['frame']==stage_two_frame and
                                n['stage']==stage_two_wire_key for n in p['nodes']))
                    same_boundary=live_stage_two['position']
                    source_stage_two_by_player=[call(page,'f=>host.canonicalAt(f)',same_boundary)
                        for page in players]
                    viewer_stage_two=snapshot(observer)['canonical']
                    assert source_stage_two_by_player and all(value and len(value)==13 and value[0]==1 and
                        value==source_stage_two_by_player[0]
                        for value in source_stage_two_by_player),{
                            'frame':same_boundary,'sourceCanonicalByPlayer':source_stage_two_by_player}
                    assert viewer_stage_two==source_stage_two_by_player[0],{
                        'frame':same_boundary,'viewerCanonical':viewer_stage_two,
                        'sourceCanonicalByPlayer':source_stage_two_by_player,'playback':live_stage_two}
                    assert live_stage_two['position']>stage_one_frame,live_stage_two
                    case['phase']='automatic-stage-two-follow-strict-canonical-pass'
                    case['oldViewerReachedStageTwo']=live_stage_two
                    case['stageTwoSameBoundaryFrame']=same_boundary
                    case['sourceStageTwoCanonicalByPlayer']=source_stage_two_by_player
                    case['viewerStageTwoCanonical']=viewer_stage_two
                    save()
                    assert request(observer,'spectator-playback',run=live_stage_two['run'],action='pause')
                    paused_stage_two=old_playback_until('pause after automatic Stage 2 transition',lambda p:p['paused'])
                    assert request(observer,'spectator-playback',run=paused_stage_two['run'],
                        action='seek',frame=stage_two_frame)
                    at_stage_two_checkpoint=old_playback_until('restore actual Stage 2 checkpoint after automatic transition',
                        lambda p:p['paused'] and not p['seeking'] and p['position']==stage_two_frame)
                    native_stage_two=call(observer,'host.spectatorDiagnostics()')
                    assert native_stage_two and native_stage_two[17]==0 and \
                        native_stage_two[44]==stage_two_frame and native_stage_two[45]==stage_two_wire_key and \
                        native_stage_two[46]==1,{'nativeCheckpointIdentityMismatch':{'node':stage_two_node,
                            'nativeDiagnostics':native_stage_two,'playback':at_stage_two_checkpoint}}
                    embedded_stage_two=native_stage_two[18:31]
                    viewer_stage_two_checkpoint=snapshot(observer)['canonical']
                    assert len(embedded_stage_two)==13 and embedded_stage_two[0]==1 and \
                        len(viewer_stage_two_checkpoint)==13 and viewer_stage_two_checkpoint[0]==1 and \
                        embedded_stage_two==source_stage_two_checkpoint_by_player[0] and \
                        viewer_stage_two_checkpoint==source_stage_two_checkpoint_by_player[0],{
                            'stageTwoCheckpointPayloadDoesNotMatchSourceCapture':{
                                'frame':stage_two_frame,'nodeIdSerial':stage_two_node['id'],
                                'wireStageKey':stage_two_wire_key,'decodedStage':stage_two_wire_key&0xff,
                                'routeFromWireKey':stage_two_route,'embeddedCanonical':embedded_stage_two,
                                'viewerCanonical':viewer_stage_two_checkpoint,
                                'sourceCanonicalByPlayer':source_stage_two_checkpoint_by_player,
                                'nativeDiagnostics':native_stage_two}}
                    case['phase']='stage-two-embedded-checkpoint-strict-pass'
                    case['stageTwoCheckpointRestore']={'playback':at_stage_two_checkpoint,
                        'embeddedCanonical':embedded_stage_two,
                        'viewerCanonical':viewer_stage_two_checkpoint,
                        'nativeDiagnostics':native_stage_two}
                    save()
                    assert request(observer,'spectator-playback',run=paused_stage_two['run'],
                        action='seek',frame=stage_one_frame)
                    crossback=old_playback_until('old viewer cross-seeks to Stage 1',
                        lambda p:p['paused'] and not p['seeking'] and p['position']==stage_one_frame)
                    viewer_crossback=snapshot(observer)['canonical']
                    assert viewer_crossback==source_stage_one,{'stageOneCheckpointFrame':stage_one_frame,
                        'sourceCanonical':source_stage_one,'viewerCanonical':viewer_crossback,
                        'stageTwoCheckpointFrame':stage_two_frame,'playback':crossback}
                    assert request(observer,'read',path='score.dat')['bytes']==sentinel
                    case['oldViewerCrossStage']={'joinedAfterSourceFrame':before[0]['net'][4],
                        'stageOneCheckpointFrame':stage_one_frame,
                        'sourceStageOneCanonicalByPlayer':source_stage_one_by_player,
                        'stageOneRestoreCanonical':viewer_stage_one,
                        'stageOneFollowingBeforeTransition':following_stage_one,
                        'stageClearSetupAfterSourceFrame':transition_before[0]['net'][4],
                        'stageClearSetupResults':stage_clear,'stageTwoVerifiedAtSourceFrame':transition_after[0]['net'][4],
                        'stageTwoCheckpointIdentity':{'frame':stage_two_frame,'serial':stage_two_node['id'],
                            'wireStageKey':stage_two_wire_key,'nativeStage':stage_two_wire_key&0xff,
                            'routeFromWireKey':stage_two_route,'serialEvidence':'received playback node id',
                            'nativeFrameStageAndEmbeddedPayloadVerifiedAfterAutomaticSwitch':True,
                            'embeddedCanonicalMatchesAllSources':True},
                        'stageTwoSourceCanonicalAtCheckpointByPlayer':source_stage_two_checkpoint_by_player,
                        'stageTwoCheckpointRestoreAfterAutomaticSwitch':{'playback':at_stage_two_checkpoint,
                            'embeddedCanonical':embedded_stage_two,
                            'viewerCanonical':viewer_stage_two_checkpoint,
                            'nativeDiagnostics':native_stage_two},
                        'stageTwoCheckpointFrame':stage_two_frame,'oldViewerReachedStageTwo':live_stage_two,
                        'stageTwoSameBoundaryFrame':same_boundary,
                        'sourceStageTwoCanonicalByPlayer':source_stage_two_by_player,
                        'viewerStageTwoCanonical':viewer_stage_two,
                        'crossbackPlayback':crossback,'crossbackCanonical':viewer_crossback}
                    case.update({'passed':True,'phase':'complete-old-viewer-cross-stage',
                        'persistentRecordsUnchanged':True,'observerPeer':call(observer,'host.observerPeer()')})
                    save()
                    print('TH08 observer',args.mode,count,'old viewer cross-stage: PASS',flush=True)
                    continue
                if args.stage_two_controls:
                    case['phase']='stage-two-playback-controls';save()
                    # Use the fresh native export for cursor/control assertions.
                    # Keep the runtime-info cache separate to verify the shell bridge.
                    def playback():return call(observer,'host.nativePlayback()')
                    def bridge_playback():return call(observer,'host.playback()')
                    def diagnostics():return call(observer,'host.spectatorDiagnostics()')
                    def audio_stats():return call(observer,'host.spectatorAudio()')
                    group_names=['economy','players','rng','enemies','projectiles','lasers','items','effects','supervisor-control','resources','spell-globals']
                    source_drive=[];seek_frame=None
                    def pump_until(label,predicate,timeout=360,keep_sources_until=None,source_target_cap=None):
                        deadline=time.monotonic()+timeout;last=None
                        while True:
                            call(observer,'t=>host.tickTo(t,false)',0xfffffffe)
                            last=playback()
                            # A live spectator stream has a native inactivity
                            # deadline. Keep the sources active in bounded
                            # one-frame windows while the observer catches up;
                            # stop extending as soon as the requested playback
                            # frame is reached, and never drive past the cap.
                            if keep_sources_until is not None and last and last['position']<keep_sources_until:
                                assert source_target_cap is not None,{'sourceWindowNeedsFiniteCap':last}
                                statuses=[];restart=[]
                                for seat,page in enumerate(players):
                                    status=call(page,'host.autoStatus()')
                                    assert status and not status['error'],{'seat':seat,'sourceAutoDrive':status}
                                    current=status['net'][3]
                                    statuses.append({'seat':seat,'status':status,'nextFrame':current})
                                    if status['stopped']:
                                        if current>source_target_cap:
                                            continue
                                        target=min(source_target_cap,current)
                                        if target>=current:
                                            restart.append({'seat':seat,'page':page,'from':current,'target':target})
                                exhausted=[row for row in statuses if row['status']['stopped'] and
                                    row['nextFrame']>source_target_cap]
                                if exhausted:
                                    raise AssertionError({'sourceCatchupCapReached':source_target_cap,
                                        'playback':last,'sourceStatuses':statuses})
                                # Restart every stopped seat before yielding or
                                # waiting on either player, so pure-timing
                                # confirmation never waits on an unstarted seat.
                                for row in restart:
                                    call(row['page'],'v=>host.autoStart(v.seat,v.target)',
                                        {'seat':row['seat'],'target':row['target']})
                                    source_drive.append({'seat':row['seat'],'from':row['from'],
                                        'target':row['target'],'rollingWindow':True})
                            native=diagnostics()
                            if native and native[2]:
                                raise AssertionError({'spectatorRestoreFailed':native,'sourceCheckpointCanonical':native[18:31],
                                    'nativeCanonicalAfterRestore':native[31:44],
                                    'checkpointRestoreMetadata':restore_metadata(native)})
                            if last and predicate(last):return last
                            assert time.monotonic()<deadline,{'waitingFor':label,'playback':last,'nativeSpectatorDiagnostics':native,
                                'sourceAutoDrive':[call(page,'host.autoStatus()') for page in players]}
                            observer.wait_for_timeout(1)
                    report0=pump_until('native Stage 2 report',lambda p:any(n['kind']=='stage' and n['stage']%256==1 for n in p['nodes']))
                    stage_nodes=[n for n in report0['nodes'] if n['kind']=='stage' and n['stage']%256==1]
                    assert stage_nodes and report0['min']==min(n['frame'] for n in stage_nodes),report0
                    seek_frame=report0['min']
                    case['stageTwoBoundary']={'setupAppliedAfterFrame':before[0]['net'][4],
                        'firstStage2CheckpointFrame':seek_frame,'stageTwoVerificationFrame':stage_two[0]['net'][4],
                        'reportedNodes':report0['nodes']};save()
                    assert source_checkpoint_captures and all(
                        capture['net'][3]==seek_frame for capture in source_checkpoint_captures),{
                            'stageTwoCheckpointFrame':seek_frame,
                            'sourceBoundaryCaptures':capture_summary(source_checkpoint_captures)}
                    source_checkpoint_rng=[capture['rng'] for capture in source_checkpoint_captures]
                    source_checkpoint_by_seat=[capture['canonical'] for capture in source_checkpoint_captures]
                    assert all(value and value[0]==1 for value in source_checkpoint_rng),{
                        'stageTwoCheckpointFrame':seek_frame,'sourceRngByPlayer':source_checkpoint_rng}
                    viewer_checkpoint_rng=None
                    if args.first_diff_only:
                        assert seek_frame==187,{'expectedCheckpointFrame':187,'actual':seek_frame}
                        source_checkpoint=source_checkpoint_captures[0]['canonical']
                        assert source_checkpoint and all(value==source_checkpoint for value in source_checkpoint_by_seat),{
                            'stageTwoCheckpointFrame':seek_frame,'sourceCanonicalBySeat':source_checkpoint_by_seat}
                        # Freeze first so Seek preserves the hold at the
                        # exact checkpoint rather than racing toward the edge.
                        assert request(observer,'spectator-playback',run=report0['run'],action='pause')
                        paused_before_seek=pump_until('pause before first-difference seek',lambda p:p['paused'])
                        assert request(observer,'spectator-playback',run=report0['run'],action='seek',frame=seek_frame)
                        at_checkpoint=pump_until('exact Stage 2 checkpoint',
                            lambda p:p['paused'] and not p['seeking'] and p['position']==seek_frame)
                        viewer_checkpoint=snapshot(observer)['canonical']
                        viewer_checkpoint_rng=call(observer,'host.spectatorRng()')
                        if viewer_checkpoint_rng!=source_checkpoint_rng[0]:
                            raise AssertionError({'checkpointFrame':seek_frame,
                                'sourceCheckpointRngByPlayer':source_checkpoint_rng,
                                'viewerFirstRestoreRng':viewer_checkpoint_rng,
                                'nativeDiagnostics':diagnostics()})
                        if source_checkpoint!=viewer_checkpoint:
                            checkpoint_groups=[group_names[i-2] for i in range(2,13)
                                if source_checkpoint[i]!=viewer_checkpoint[i]]
                            raise AssertionError({'checkpointFrame':seek_frame,
                                'sourceCanonical':source_checkpoint,'sourceCanonicalBySeat':source_checkpoint_by_seat,
                                'viewerCanonical':viewer_checkpoint,'groups':checkpoint_groups,
                                'checkpointFirstDifferenceBytes':byte_difference_report(observer,players,seek_frame,
                                    checkpoint_groups,at_checkpoint,source_checkpoint_captures),
                                'nativeDiagnostics':diagnostics(),'playback':at_checkpoint})
                        # Advance every source seat beyond the required
                        # 210-frame input frontier and keep their bounded
                        # drivers running while the first-frame bytes are read.
                        # The finite cap prevents an unbounded restart loop.
                        source_frontier=seek_frame+210
                        source_target=source_frontier+120
                        source_initial=[]
                        for seat,page in enumerate(players):
                            current=call(page,'()=>host.snapshot(false).net[3]')
                            target=source_target
                            if current<=target:
                                call(page,'v=>host.autoStart(v.seat,v.target)',{'seat':seat,'target':target})
                            source_initial.append({'seat':seat,'from':current,'target':target})
                        # Start every seat before waiting: pure-timing
                        # confirmation may require inputs from every player.
                        for seat,page in enumerate(players):
                            deadline=time.monotonic()+120;status=None
                            while True:
                                status=call(page,'host.autoStatus()')
                                assert not status or not status['error'],{'seat':seat,'sourceAutoDrive':status}
                                snapshot_now=snapshot(page,False)
                                if snapshot_now['net'][3]>=source_frontier:break
                                assert time.monotonic()<deadline,{'seat':seat,'sourceFrontier':source_frontier,
                                    'sourceStatus':status,'net':snapshot_now['net']}
                                page.wait_for_timeout(1)
                            still_connected=snapshot(page,False)
                            assert still_connected['net'][2] and still_connected['net'][3]>=source_frontier,{
                                'seat':seat,'sourceFrontier':source_frontier,'sourceStatus':still_connected['net']}
                            source_drive.append({**source_initial[seat],
                                'frontierReached':still_connected['net'][3],
                                'connectedAfterDrive':still_connected['net'][2]})
                        assert request(observer,'spectator-playback',run=at_checkpoint['run'],action='play')
                        play_state=playback()
                        assert play_state and not play_state['paused'] and not play_state['following'] and not play_state['seeking'],{
                            'historicalPlayState':play_state}
                        checked=[];first_difference=None;last_state=play_state;tick_states=[]
                        for _ in range(32):
                            before_tick=playback()
                            tick_snapshot=call(observer,'()=>host.spectatorTickOnce(true)')
                            last_state=playback()
                            tick_states.append({'before':before_tick['position'] if before_tick else None,
                                'after':last_state['position'] if last_state else None,
                                'paused':last_state['paused'] if last_state else None,
                                'following':last_state['following'] if last_state else None,
                                'seeking':last_state['seeking'] if last_state else None})
                            assert last_state and not last_state['paused'] and not last_state['following'] and not last_state['seeking'],{
                                'playMustAdvanceHistoricalFrames':last_state,'tickStates':tick_states}
                            if before_tick and last_state['position']!=before_tick['position']+1:
                                raise AssertionError({'eachSpectatorTickMustAdvanceExactlyOneFrame':tick_states})
                            frame=last_state['position']
                            if frame==217:
                                source_by_seat=[capture['canonical'] for capture in source_217_captures]
                                source=source_by_seat[0]
                                actual=tick_snapshot['canonical'];checked.append(frame)
                                if any(value!=source for value in source_by_seat[1:]):
                                    first_difference={'frame':frame,'reason':'players disagree',
                                        'sourceCanonicalBySeat':source_by_seat}
                                elif source!=actual:
                                    first_difference={'frame':frame,
                                        'groups':[group_names[i-2] for i in range(2,13) if source[i]!=actual[i]],
                                        'sourceCanonicalBySeat':source_by_seat,'viewer':actual}
                                break
                        assert checked==[217],{'checkedFrames':checked,'position':last_state,
                            'tickStates':tick_states,'sourceAutoDrive':[call(page,'host.autoStatus()') for page in players]}
                        current_source=[call(page,'host.autoStatus()') for page in players]
                        case['firstDifferenceProbe']={'checkpointFrame':seek_frame,
                            'checkpointCanonical':source_checkpoint,'pausedBeforeSeek':paused_before_seek,
                            'checkedFrames':checked,'firstDifference':first_difference,
                            'sourceCanonical':source_217_captures[0]['canonical'],
                            'sourceCanonicalP2':source_217_captures[1]['canonical'],
                            'sourceCanonicalBySeat':[capture['canonical'] for capture in source_217_captures],
                            'viewerCanonical':tick_snapshot['canonical'],
                            'runtimeInfoPlaybackAtCapture':bridge_playback(),
                            'sourceDrive':source_drive,'sourceStatusAtRead':current_source,'tickStates':tick_states,
                            'nativeDiagnostics':[call(page,'host.spectatorDiagnostics()') for page in pages]}
                        for page in players:call(page,'host.autoStop()')
                        if first_difference:
                            case['firstDifferenceProbe']['firstDifference']=first_difference
                            case['firstDifferenceBytes']=byte_difference_report(observer,players,217,
                                first_difference.get('groups',[]),last_state,source_217_captures)
                            case['phase']='first-difference-diagnosed';save()
                            raise AssertionError(case['firstDifferenceProbe'])
                        # Recreate the previously observed second-restore path
                        # without driving through hundreds of frames: restore
                        # CP187 once, play exactly to 217, then seek back to
                        # CP187 and compare against the Python-held source
                        # snapshot and exact named-field bytes.
                        assert request(observer,'spectator-playback',run=last_state['run'],action='pause')
                        paused_at_217=pump_until('pause at first future frame',
                            lambda p:p['paused'] and not p['seeking'] and p['position']==217)
                        assert request(observer,'spectator-playback',run=paused_at_217['run'],
                            action='seek',frame=seek_frame)
                        second_restore=pump_until('second restore to Stage 2 checkpoint',
                            lambda p:p['paused'] and not p['seeking'] and p['position']==seek_frame)
                        second_canonical=snapshot(observer)['canonical']
                        second_bytes=call(observer,'()=>host.canonicalBytesNow()')
                        source_checkpoint=source_checkpoint_captures[0]['canonical']
                        source_checkpoint_p2=source_checkpoint_captures[1]['canonical']
                        exact_group_matches={str(group):all(
                            capture['groups'][str(group)]==bytes(second_bytes[str(group)])
                            for capture in source_checkpoint_captures
                        ) for group in (1,8)}
                        second_diff_groups=[group_names[i-2] for i in range(2,13)
                            if source_checkpoint[i]!=second_canonical[i]]
                        source_checkpoint_divergent_seats=[seat for seat,capture in
                            enumerate(source_checkpoint_captures) if capture['canonical']!=source_checkpoint]
                        if not all(exact_group_matches.values()):
                            second_diff_groups=list(dict.fromkeys(second_diff_groups+
                                [name for name,group in (('players',1),('supervisor-control',8))
                                    if not exact_group_matches[str(group)]]))
                        case['secondRestoreProbe']={'checkpointFrame':seek_frame,
                            'checkpointBoundary':'NextFrame 187, before input 187',
                            'positionBeforeSecondSeek':paused_at_217['position'],
                            'playbackAfterSecondRestore':second_restore,
                            'sourceCanonicalByPlayer':[source_checkpoint,source_checkpoint_p2],
                            'sourceCanonicalBySeat':source_checkpoint_by_seat,
                            'sourceDivergentSeats':source_checkpoint_divergent_seats,
                            'viewerCanonical':second_canonical,
                            'sourceGroupBytesByPlayer':capture_summary(source_checkpoint_captures),
                            'exactGroupByteMatches':exact_group_matches,
                            'rngAtSecondRestore':call(observer,'host.spectatorRng()'),
                            'firstDifferenceBytes':byte_difference_report(observer,players,seek_frame,
                                second_diff_groups or ['players','supervisor-control'],second_restore,
                                source_checkpoint_captures)}
                        if source_checkpoint_divergent_seats or source_checkpoint!=second_canonical or \
                            not all(exact_group_matches.values()):
                            case['phase']='second-restore-diagnosed';save()
                            raise AssertionError(case['secondRestoreProbe'])
                        case['phase']='second-restore-clear';case['passed']=True;save()
                        print('TH08 observer',args.mode,count,'Stage 2 second restore: PASS',flush=True)
                        continue
                    future_frame=max(args.frames,seek_frame+600)
                    # Advance one source frame per observer pump. A 90-frame
                    # burst outruns native rendering and can trip the
                    # spectator inactivity deadline before it consumes history.
                    # Playback.Edge trails Received by the 90-frame relay
                    # watermark. Keep that bounded source headroom so the
                    # observer can consume every requested frame.
                    source_target_cap=future_frame+90
                    for seat,page in enumerate(players):
                        current=snapshot(page,False)['net'][3]
                        assert current<=source_target_cap,{'sourceAlreadyPastCatchupCap':{
                            'seat':seat,'nextFrame':current,'cap':source_target_cap}}
                        target=current
                        call(page,'v=>host.autoStart(v.seat,v.target)',{'seat':seat,'target':target})
                        source_drive.append({'seat':seat,'from':current,'target':target,'rollingWindow':False})
                    source_checkpoint=source_checkpoint_captures[0]['canonical']
                    assert source_checkpoint and all(value==source_checkpoint for value in source_checkpoint_by_seat),{
                        'stageTwoCheckpointFrame':seek_frame,'sourceCanonicalBySeat':source_checkpoint_by_seat}
                    case['stageTwoLateRestore']={'min':seek_frame,'sourceCheckpointCanonical':source_checkpoint,
                        'stageTwoVerificationFrame':stage_two[0]['net'][4],'futureTarget':future_frame,
                        'sourceTargetCap':source_target_cap,'sourceWindowFrames':1,
                        'sourceCheckpointCanonicalBySeat':source_checkpoint_by_seat,
                        'sourceCheckpointRngByPlayer':source_checkpoint_rng,
                        'sourceCheckpointCaptures':capture_summary(source_checkpoint_captures),
                        'viewerFirstRestoreRng':viewer_checkpoint_rng};save()
                    # The playback view starts from the late viewer's Stage 2
                    # checkpoint and receives future confirmed input. Stop at
                    # the live edge only after it contains 600 future frames.
                    assert request(observer,'spectator-playback',run=report0['run'],action='live')
                    at_future_edge=pump_until('600 frames after Stage 2 checkpoint',
                        lambda p:p['position']>=future_frame and not p['seeking'],
                        keep_sources_until=future_frame,source_target_cap=source_target_cap)
                    case['stageTwoLateRestore']['sourceDriveWindows']=list(source_drive);save()
                    assert request(observer,'spectator-playback',run=at_future_edge['run'],action='pause')
                    paused=pump_until('pause at future edge',lambda p:p['paused'])
                    assert paused['position']>=future_frame,paused
                    source_future_by_seat=[call(page,'f=>host.canonicalAt(f)',paused['position']) for page in players]
                    source_future=source_future_by_seat[0]
                    source_future_p2=source_future_by_seat[1]
                    viewer_future=snapshot(observer)['canonical']
                    source_timelines=[call(page,'()=>host.timelineData()') for page in players]
                    viewer_trace=call(observer,'()=>host.spectatorCanonicalTrace()')
                    source_by_frame=[{int(row[0]):row[1] for row in timeline} for timeline in source_timelines]
                    compared=0;first_difference=None;missing_source_frames_by_seat=[[] for _ in players]
                    for row in viewer_trace:
                        frame=int(row[0]);sources=[timeline.get(frame) for timeline in source_by_frame]
                        missing=[seat for seat,value in enumerate(sources) if value is None]
                        if missing:
                            for seat in missing:
                                if len(missing_source_frames_by_seat[seat])<8:
                                    missing_source_frames_by_seat[seat].append(frame)
                            continue
                        compared+=1;actual=row[1:];reference=sources[0]
                        source_mismatch=next((seat for seat,value in enumerate(sources[1:],1)
                            if value!=reference),None)
                        if source_mismatch is not None:
                            first_difference={'frame':frame,'reason':'players disagree','seatA':0,
                                'seatB':source_mismatch,'sourceCanonicalBySeat':sources};break
                        if reference!=actual:
                            first_difference={'frame':frame,'groups':[group_names[i-2] for i in range(2,13)
                                if reference[i]!=actual[i]],'sourceCanonicalBySeat':sources,'viewer':actual};break
                    expected_start=seek_frame+1
                    expected_end=min(paused['position'],future_frame)
                    expected_frames=set(range(expected_start,expected_end+1))
                    viewer_frames={int(row[0]) for row in viewer_trace}
                    missing_viewer_frames=sorted(expected_frames-viewer_frames)
                    missing_source_by_seat=[sorted(expected_frames-set(timeline)) for timeline in source_by_frame]
                    source_frames_by_seat=[len(timeline) for timeline in source_by_frame]
                    case['sourceViewerFrameComparison']={'comparedFrames':compared,
                        'sourceFramesP1':source_frames_by_seat[0],'sourceFramesP2':source_frames_by_seat[1],
                        'sourceFramesBySeat':source_frames_by_seat,'viewerFrames':len(viewer_trace),
                        'expectedStrictRange':[expected_start,expected_end],
                        'strictRangeFrameCount':len(expected_frames),
                        'sourceMissingAtViewerFramesBySeat':missing_source_frames_by_seat,
                        'missingSourceFramesBySeat':[missing[:32] for missing in missing_source_by_seat],
                        'missingExpectedViewerFrames':missing_viewer_frames[:32],
                        'missingExpectedSourceFramesP1':missing_source_by_seat[0][:32],
                        'missingExpectedSourceFramesP2':missing_source_by_seat[1][:32],
                        'missingExpectedSourceFramesBySeat':[missing[:32] for missing in missing_source_by_seat],
                        'firstDifference':first_difference}
                    if first_difference and first_difference.get('frame') is not None and 'groups' in first_difference:
                        diff_frame=first_difference['frame']
                        assert request(observer,'spectator-playback',run=report0['run'],action='seek',frame=diff_frame)
                        seek_diag=pump_until('pause at first strict difference',
                            lambda p:p['paused'] and not p['seeking'] and p['position']==diff_frame)
                        case['firstDifferenceBytes']=byte_difference_report(observer,players,diff_frame,
                            first_difference['groups'],seek_diag)
                    save()
                    assert source_future and all(value==source_future for value in source_future_by_seat) and \
                        source_future==viewer_future,{'futureFrame':paused['position'],
                        'sourceCanonicalP1':source_future,'sourceCanonicalP2':source_future_p2,
                        'sourceCanonicalBySeat':source_future_by_seat,
                        'viewerCanonical':viewer_future,'playback':paused}
                    assert expected_end>=future_frame and not missing_viewer_frames and \
                        all(not missing for missing in missing_source_by_seat) and compared>=len(expected_frames) and \
                        first_difference is None,case['sourceViewerFrameComparison']
                    case['stageTwoLateRestore'].update({'sourceFrame':paused['position'],'sourceCanonical':source_future,
                        'sourceCanonicalBySeat':source_future_by_seat,
                        'viewerCanonical':viewer_future,'futureFramesAfterCheckpoint':paused['position']-seek_frame,
                        'initialPlayback':at_future_edge});save()
                    audio_before_checkpoint_restore=audio_stats()
                    assert request(observer,'spectator-playback',run=paused['run'],action='seek',frame=seek_frame)
                    checkpoint_restore=pump_until('paused Stage 2 checkpoint restore',
                        lambda p:p['paused'] and not p['seeking'] and p['position']==seek_frame)
                    audio_after_checkpoint_restore=audio_stats()
                    viewer_seek=snapshot(observer)['canonical']
                    second_restore_source_matches=[capture['canonical']==viewer_seek
                        for capture in source_checkpoint_captures]
                    assert all(second_restore_source_matches),{
                        'seekFrame':seek_frame,'sourceCanonical':source_checkpoint,
                            'sourceCanonicalBySeat':source_checkpoint_by_seat,
                            'sourceMatchesViewerBySeat':second_restore_source_matches,
                            'viewerCanonical':viewer_seek,'playback':checkpoint_restore}
                    case['secondCheckpointRestore']={'frame':seek_frame,
                        'sourceCanonicalBySeat':source_checkpoint_by_seat,
                        'sourceMatchesViewerBySeat':second_restore_source_matches,
                        'viewerCanonical':viewer_seek,'sourceCaptureSummary':capture_summary(source_checkpoint_captures)}
                    assert audio_after_checkpoint_restore[1:]==audio_before_checkpoint_restore[1:],{
                        'checkpointRestoreMustNotEmitAudio':{'before':audio_before_checkpoint_restore,
                            'after':audio_after_checkpoint_restore},'playback':checkpoint_restore}
                    # Seeking exactly to the checkpoint reconstructs no simulation
                    # ticks. Exercise an actual muted historical replay before
                    # checking that normal Play forwards SFX again.
                    muted_seek_frame=300
                    assert muted_seek_frame>seek_frame and muted_seek_frame<=future_frame,{
                        'invalidMutedSeekFrame':{'checkpoint':seek_frame,'target':muted_seek_frame,'future':future_frame}}
                    audio_before_seek=audio_stats()
                    assert request(observer,'spectator-playback',run=checkpoint_restore['run'],
                        action='seek',frame=muted_seek_frame)
                    sought=pump_until('paused historical Stage 2 seek',
                        lambda p:p['paused'] and not p['seeking'] and p['position']==muted_seek_frame)
                    audio_after_seek=audio_stats()
                    source_seek_by_seat=[call(page,'f=>host.canonicalAt(f)',muted_seek_frame) for page in players]
                    source_seek=source_seek_by_seat[0]
                    source_seek_p2=source_seek_by_seat[1]
                    viewer_seek=snapshot(observer)['canonical']
                    assert source_seek and all(value==source_seek for value in source_seek_by_seat) and \
                        source_seek==viewer_seek,{
                        'mutedSeekCanonical':{'frame':muted_seek_frame,'sourceP1':source_seek,
                            'sourceP2':source_seek_p2,'sourceBySeat':source_seek_by_seat,
                            'viewer':viewer_seek,'playback':sought}}
                    assert audio_after_seek[1]>audio_before_seek[1] and \
                        audio_after_seek[2]>audio_before_seek[2] and \
                        audio_after_seek[3]==audio_before_seek[3],{
                            'historicalSeekMustSuppressAttemptedSounds':{'before':audio_before_seek,
                                'after':audio_after_seek,'frame':muted_seek_frame},'playback':sought}
                    assert request(observer,'spectator-playback',run=sought['run'],action='play')
                    play_target=muted_seek_frame+30
                    played=pump_until('play after seek',lambda p:p['position']>=play_target and not p['paused'])
                    assert request(observer,'spectator-playback',run=played['run'],action='pause')
                    paused_after_play=pump_until('pause after play',lambda p:p['paused'])
                    play_source_by_seat=[call(page,'f=>host.canonicalAt(f)',paused_after_play['position']) for page in players]
                    play_source=play_source_by_seat[0]
                    play_source_p2=play_source_by_seat[1]
                    play_viewer=snapshot(observer)['canonical']
                    assert play_source and all(value==play_source for value in play_source_by_seat) and \
                        play_source==play_viewer,{'playFrame':paused_after_play['position'],
                            'sourceCanonicalP1':play_source,'sourceCanonicalP2':play_source_p2,
                            'sourceCanonicalBySeat':play_source_by_seat,'viewerCanonical':play_viewer}
                    audio_after_play=audio_stats()
                    assert audio_after_play[3]>audio_after_seek[3],{
                        'normalPlayMustEmitSounds':{'afterSeek':audio_after_seek,'afterPlay':audio_after_play},'playback':paused_after_play}
                    # A local pause with no new relay input must remain usable
                    # beyond the normal live-input inactivity deadline. Stop
                    # both source drivers and prove their native input frontiers
                    # stay fixed while the observer remains paused.
                    for page in players:call(page,'host.autoStop()')
                    paused_hold_origin=playback()
                    assert paused_hold_origin and paused_hold_origin['paused'] and not paused_hold_origin['seeking'],{
                        'pausedNoInputHoldStart':paused_hold_origin}
                    source_hold_before=[snapshot(page,False)['net'][3] for page in players]
                    hold_ms=16_000;hold_started=time.monotonic();hold_samples=0;hold_latest=paused_hold_origin
                    case['pausedNoInputHold']={'requestedMs':hold_ms,'position':paused_hold_origin['position'],
                        'edge':paused_hold_origin['edge'],'sourceNextFramesBefore':source_hold_before,
                        'sourcePacketsStopped':True,'phase':'holding'}
                    save()
                    while (time.monotonic()-hold_started)*1000<hold_ms:
                        hold_latest=playback();hold_snapshot=snapshot(observer,False);hold_samples+=1
                        assert hold_latest and hold_latest['paused'] and not hold_latest['seeking'] and \
                            hold_latest['position']==paused_hold_origin['position'] and not hold_snapshot['error'],{
                                'pausedNoInputHoldProgress':{'origin':paused_hold_origin,'latest':hold_latest,
                                    'snapshot':hold_snapshot,'elapsedMs':int((time.monotonic()-hold_started)*1000),
                                    'samples':hold_samples}}
                        if hold_samples%8==0:
                            case['pausedNoInputHold'].update({'elapsedMs':int((time.monotonic()-hold_started)*1000),
                                'samples':hold_samples,'latest':hold_latest})
                            save()
                        observer.wait_for_timeout(250)
                    hold_elapsed_ms=int((time.monotonic()-hold_started)*1000)
                    source_hold_after=[snapshot(page,False)['net'][3] for page in players]
                    assert hold_elapsed_ms>15_000 and source_hold_after==source_hold_before,{
                        'pausedNoInputHold':{'elapsedMs':hold_elapsed_ms,'requestedMs':hold_ms,
                            'sourceNextFramesBefore':source_hold_before,'sourceNextFramesAfter':source_hold_after,
                            'playback':hold_latest}}
                    case['pausedNoInputHold'].update({'elapsedMs':hold_elapsed_ms,'samples':hold_samples,
                        'sourceNextFramesAfter':source_hold_after,'latest':hold_latest,'phase':'passed'})
                    save()
                    assert request(observer,'spectator-playback',run=paused_after_play['run'],action='live')
                    live=pump_until('return to live edge',lambda p:p['following'] and not p['seeking'] and p['position']>=p['edge'])
                    old_edge=live['edge'];live_source_windows=[]
                    for seat,page in enumerate(players):
                        status=call(page,'host.autoStatus()')
                        if status is None and case.get('pausedNoInputHold',{}).get('phase')=='passed':
                            # The no-input pause regression intentionally clears
                            # the source auto-driver. Read the stopped native
                            # frontier directly before starting the next bounded
                            # source window below.
                            native=snapshot(page,False)
                            status={'seat':seat,'target':None,'stopped':True,'error':'',
                                'net':native['net'],'idleAfterPausedNoInputHold':True}
                        assert status and not status['error'],{'seat':seat,'sourceAutoDrive':status}
                        next_frame=status['net'][3];last_frame=status['net'][4]
                        target=next_frame+119 # Exactly 120 additional source inputs.
                        call(page,'v=>host.trackFrames(v)',list(range(max(0,last_frame-8),target+3)))
                        live_source_windows.append({'seat':seat,'fromNextFrame':next_frame,
                            'targetInputFrame':target,'expectedInputs':120})
                    deadline=time.monotonic()+120;latest=live;max_live_lag=0;live_statuses=[]
                    live_edge_before=old_edge
                    case['boundedLiveFollowProgress']={'expectedInputsPerSeat':120,'completedInputsPerSeat':0,
                        'edgeStart':old_edge,'phase':'running','sourceWindows':live_source_windows}
                    save()
                    # Advance every source by one confirmed input, then let the
                    # observer consume that newly published edge before asking
                    # any source for the next input. This keeps ordinary live
                    # playback out of the catch-up mute path while preserving
                    # exactly 120 source inputs per seat.
                    for step in range(120):
                        source_frame=live_source_windows[0]['fromNextFrame']+step
                        source_frontiers=[snapshot(page,False)['net'][3] for page in players]
                        assert source_frontiers==[source_frame]*len(players),{
                            'liveSourceFrontierMismatch':{'step':step,'expected':source_frame,
                                'sourceFrontiers':source_frontiers,'windows':live_source_windows}}
                        for row,page in zip(live_source_windows,players):
                            call(page,'v=>host.autoStart(v.seat,v.target)',
                                {'seat':row['seat'],'target':source_frame})
                        while True:
                            call(observer,'t=>host.tickTo(t,false)',0xfffffffe)
                            latest=playback()
                            assert latest and latest['following'] and not latest['seeking'],{
                                'liveFollowState':latest,'step':step}
                            max_live_lag=max(max_live_lag,max(0,latest['edge']-latest['position']))
                            live_statuses=[call(page,'host.autoStatus()') for page in players]
                            assert all(status and not status['error'] for status in live_statuses),{
                                'sourceAutoDrive':live_statuses,'playback':latest,'step':step}
                            if all(status['stopped'] for status in live_statuses):
                                for row,status in zip(live_source_windows,live_statuses):
                                    advance=status['net'][3]-row['fromNextFrame']
                                    assert advance==step+1 and status['net'][3]==source_frame+1,{
                                        'liveSourceInputCount':{'step':step,'frame':source_frame,
                                            'window':row,'status':status,'advanced':advance}}
                                if latest['edge']>live_edge_before and latest['position']>=latest['edge']:
                                    break
                            assert time.monotonic()<deadline,{'sourceAutoDrive':live_statuses,
                                'step':step,'previousEdge':live_edge_before,'playback':latest,
                                'maxExtraLag':max_live_lag}
                            observer.wait_for_timeout(1)
                        live_edge_before=latest['edge']
                        if (step+1)%10==0 or step==119:
                            case['boundedLiveFollowProgress'].update({'completedInputsPerSeat':step+1,
                                'edge':latest['edge'],'position':latest['position'],
                                'maxExtraLag':max_live_lag,'phase':'running' if step<119 else 'inputs-complete'})
                            save()
                    case['boundedLiveFollowProgress'].update({'completedInputsPerSeat':120,
                        'edge':latest['edge'],'position':latest['position'],
                        'maxExtraLag':max_live_lag,'phase':'caught-up'})
                    save()
                    for page in players:call(page,'host.autoStop()')
                    live_source_by_seat=[call(page,'f=>host.canonicalAt(f)',latest['position']) for page in players]
                    live_source=live_source_by_seat[0]
                    live_source_p2=live_source_by_seat[1]
                    viewer_live=snapshot(observer)['canonical']
                    assert live_source and all(value==live_source for value in live_source_by_seat) and \
                        live_source==viewer_live,{'livePosition':latest['position'],
                            'sourceCanonicalP1':live_source,'sourceCanonicalP2':live_source_p2,
                            'sourceCanonicalBySeat':live_source_by_seat,
                            'viewerCanonical':viewer_live,'playback':latest}
                    final_extra_lag=latest['edge']-latest['position']
                    assert latest['edge']-old_edge>=100 and max_live_lag<=120 and final_extra_lag==0,{
                        'boundedLiveFollow':{'previousEdge':old_edge,'finalEdge':latest['edge'],
                            'edgeAdvance':latest['edge']-old_edge,'maxExtraLag':max_live_lag,
                            'finalExtraLag':final_extra_lag,
                            'sourceWindows':live_source_windows,'sourceStatus':live_statuses,'playback':latest}}
                    audio_after_live=audio_stats()
                    assert audio_after_live[3]>audio_after_play[3],{
                        'livePlayMustEmitSounds':{'afterPlay':audio_after_play,'afterLive':audio_after_live},'playback':latest}
                    runtime_info_playback=bridge_playback()
                    assert runtime_info_playback and runtime_info_playback['run']==latest['run'],{
                        'runtimeInfoSpectatorBridge':runtime_info_playback,'nativePlayback':latest}
                    case['playbackControls']={'paused':paused,'checkpointFrame':seek_frame,
                        'checkpointRestore':checkpoint_restore,'checkpointRestoreAudio':{
                            'before':audio_before_checkpoint_restore,'after':audio_after_checkpoint_restore},
                        'checkpointRestoreSourceMatchesBySeat':second_restore_source_matches,
                        'seekFrame':muted_seek_frame,'sought':sought,'played':paused_after_play,
                        'seekCanonicalBySeat':source_seek_by_seat,
                        'playCanonicalBySeat':play_source_by_seat,
                        'pausedNoInputHold':case['pausedNoInputHold'],
                        'liveBeforeAdvance':live,'liveAfterAdvance':latest,'liveCanonical':live_source,
                        'liveCanonicalBySeat':live_source_by_seat,
                        'liveSourceWindows':live_source_windows,'liveMaxExtraLag':max_live_lag,
                        'boundedLiveFollowProgress':case['boundedLiveFollowProgress'],
                        'liveFinalExtraLag':final_extra_lag,
                        'runtimeInfoPlayback':runtime_info_playback,
                        'continuedFrames':latest['edge']-old_edge,
                        'soundStats':{'afterSeek':audio_after_seek,'afterPlay':audio_after_play,'afterLive':audio_after_live}}
                    assert request(observer,'read',path='score.dat')['bytes']==sentinel
                    case.update({'passed':True,'phase':'complete','liveResourceReplacementRejected':True,
                        'persistentRecordsUnchanged':True,'observerPeer':call(observer,'host.observerPeer()'),
                        'sourceStoppedExplicitly':True});save()
                    print('TH08 observer',args.mode,count,'players Stage 2 controls: PASS',flush=True)
                    continue
                initial_target=120 if args.lifecycle_reentry else 119
                caught,iterations=advance(initial_target,True)
                case['caughtUp']={'iterations':iterations,'values':caught};save()
                assert iterations<120,'observer did not use bounded native catch-up'
                if args.lifecycle_reentry:
                    lifecycle_entries=[]
                    lifecycle_entries.append(lifecycle_entry(initial_target,caught))
                    case['phase']='repeated-runtime-admission-and-idle-pause'
                    case['lifecycleReentry']={'entries':lifecycle_entries,'phase':'initial-admission',
                        'permanentHostIframe':observer_host_iframe,'runtimeDocumentEpochs':[observer_host_iframe['epoch']]};save()
                    # Re-enter the same running room twice with the same
                    # observer identity. Each connection must receive a fresh
                    # timing/bootstrap/checkpoint/input path and reach the
                    # exact source boundary before any pause test begins.
                    for entry,target in ((1,initial_target+1),(2,initial_target+2)):
                        old_socket_records=observer_data_socket_records()
                        old_sockets=[dict(record) for record in old_socket_records]
                        assert old_sockets and not old_sockets[-1]['closed'],{
                            'entry':entry,'runtimeSocketsAtAssertion':json.dumps(old_sockets,
                                separators=(',',':'))}
                        old_socket=old_socket_records[-1]
                        old_host_iframe=call(observer,'host.iframeState()')
                        exits=call(observer,'host.close()')
                        epoch_exits=[event for event in exits if event.get('epoch')==old_host_iframe['epoch']]
                        assert len(epoch_exits)==1 and epoch_exits[0].get('status')=='success',{
                            'entry':entry,'runtimeEpoch':old_host_iframe['epoch'],
                            'epochExitEvents':epoch_exits,'allRuntimeExitEvents':[dict(event) for event in exits]}
                        deadline=time.monotonic()+15
                        while not old_socket['closed']:
                            assert time.monotonic()<deadline,{'entry':entry,
                                'oldRuntimeSocketAtAssertion':json.dumps(dict(old_socket),separators=(',',':'))}
                            observer.wait_for_timeout(25)
                        player_sockets={seat:player_data_sockets(seat) for seat in range(count)}
                        assert all(rows and not rows[-1]['closed'] for rows in player_sockets.values()),{
                            'entry':entry,'playerDataSocketsAtAssertion':json.dumps(player_sockets,
                                separators=(',',':'))}
                        wire_probe['entry']=entry
                        call(observer,'host.open()')
                        observer.wait_for_function('host.ready()',timeout=120000)
                        host_iframe=call(observer,'host.iframeState()')
                        assert host_iframe['creationCount']==observer_host_iframe['creationCount'] and \
                            host_iframe['blankNavigations']==entry and host_iframe['epoch']==observer_host_iframe['epoch']+entry and \
                            f'runtimeEpoch={host_iframe["epoch"]}' in host_iframe['src'],{
                                'entry':entry,'initialHostIframe':observer_host_iframe,'hostIframe':host_iframe}
                        request(observer,'configure',options={**common,'netplayPlayer':-1,
                            'netplaySpectator':True,'netplaySpectatorId':observer_id,
                            'netplayUrl':url+'&member=fixture_'+observer_id},music='none')
                        request(observer,'launch')
                        case['lifecycleReentry'].update({'phase':f'entry-{entry}-waiting',
                            'targetLastFrame':target,'runtimeSocketClosedBeforeReentry':True,
                            'permanentHostIframe':host_iframe,
                            'runtimeDocumentEpochs':case['lifecycleReentry']['runtimeDocumentEpochs']+[host_iframe['epoch']]});save()
                        values,_=advance(target,True)
                        lifecycle_entries.append(lifecycle_entry(target,values))
                        case['lifecycleReentry'].update({'entries':lifecycle_entries,
                            'phase':f'entry-{entry}-ready'});save()

                    # After bootstrap, passive Following must stay at one
                    # authored frame per native tick even when new input is
                    # available. Feed one source frame, receive it, then make
                    # exactly one viewer tick and compare the cursor movement.
                    current=call(observer,'host.nativePlayback()')
                    assert current and not current['paused'],{'bootstrapPlayback':current}
                    request(observer,'spectator-playback',run=current['run'],action='live')
                    deadline=time.monotonic()+60
                    while True:
                        call(observer,'t=>host.tickTo(t,false)',0xfffffffe)
                        current=call(observer,'host.nativePlayback()')
                        if current and current['following'] and not current['seeking'] and \
                                current['position']>=current['edge']:
                            break
                        assert time.monotonic()<deadline,{'waitingForInitialLive':current,
                            'observerStatus':snapshot(observer,False)}
                        observer.wait_for_timeout(1)
                    passive_follow_ticks=[]
                    for _ in range(3):
                        before_tick=call(observer,'host.nativePlayback()')
                        source_next=snapshot(players[0],False)['net'][3]
                        source_frame,_=advance(source_next,False)
                        assert all(value['net'][3]==source_next+1 for value in source_frame),{
                            'sourceNextFrameBeforeOneXInput':source_next,'sourceAfterInput':source_frame}
                        expected_received=before_tick['received']+1
                        deadline=time.monotonic()+15;received=None
                        while True:
                            received=call(observer,'host.spectatorPollOnce()')
                            if received and received['received']>=expected_received:break
                            assert time.monotonic()<deadline,{'expectedReceived':expected_received,
                                'sourceNextFrame':source_next+1,'nativePlayback':received}
                            observer.wait_for_timeout(1)
                        assert received['received']==expected_received and \
                                received['edge']==before_tick['edge']+1,{
                            'oneSourceFrameMustMoveEdgeOne':{'before':before_tick,'afterReceive':received,
                                'sourceNextFrame':source_next+1}}
                        call(observer,'()=>host.spectatorTickOnce(false)')
                        after_tick=call(observer,'host.nativePlayback()')
                        assert after_tick and after_tick['following'] and not after_tick['paused'] and \
                                not after_tick['seeking'] and after_tick['position']==before_tick['position']+1,{
                            'passiveFollowingMustAdvanceExactlyOnePerNativeTick':{
                                'before':before_tick,'afterReceive':received,'afterTick':after_tick}}
                        passive_follow_ticks.append({'before':before_tick,'afterReceive':received,
                            'afterOneTick':after_tick})

                    # Create a fixed seek with enough available history to
                    # remain in progress after one accelerated callback. Pause
                    # must cancel that target immediately at the actual cursor.
                    before_backlog=call(observer,'host.nativePlayback()')
                    source_next=snapshot(players[0],False)['net'][3]
                    backlog_frames=96
                    backlog_target=source_next+backlog_frames-1
                    backlog_sources,_=advance(backlog_target,False)
                    assert all(value['net'][3]==backlog_target+1 for value in backlog_sources),{
                        'backlogTargetLastFrame':backlog_target,'sources':backlog_sources}
                    expected_received=before_backlog['received']+backlog_frames
                    deadline=time.monotonic()+30;after_backlog=None
                    while True:
                        after_backlog=call(observer,'host.spectatorPollOnce()')
                        if after_backlog and after_backlog['received']>=expected_received:break
                        assert time.monotonic()<deadline,{'expectedReceived':expected_received,
                            'nativePlayback':after_backlog,'backlogSourceNextFrame':backlog_target+1}
                        observer.wait_for_timeout(1)
                    assert after_backlog['received']==expected_received and \
                            after_backlog['edge']>=before_backlog['position']+80,{
                        'seekBacklogNotAvailable':{'before':before_backlog,'afterReceive':after_backlog,
                            'backlogFrames':backlog_frames}}
                    seek_target=before_backlog['position']+80
                    assert request(observer,'spectator-playback',run=after_backlog['run'],
                        action='seek',frame=seek_target)
                    seeking=call(observer,'host.nativePlayback()')
                    assert seeking and seeking['seeking'] and seeking['target']==seek_target,{
                        'seekDidNotStart':seeking,'seekTarget':seek_target}
                    call(observer,'()=>host.spectatorTickOnce(false)')
                    after_seek_tick=call(observer,'host.nativePlayback()')
                    assert after_seek_tick and after_seek_tick['seeking'] and \
                            before_backlog['position']<after_seek_tick['position']<seek_target,{
                        'seekMustBeInProgressBeforePause':{'before':before_backlog,
                            'afterReceive':after_backlog,'afterSeekTick':after_seek_tick,
                            'seekTarget':seek_target}}
                    request(observer,'spectator-playback',run=after_seek_tick['run'],action='pause')
                    stopped_seek=call(observer,'host.nativePlayback()')
                    assert stopped_seek and stopped_seek['paused'] and not stopped_seek['seeking'] and \
                            stopped_seek['position']==after_seek_tick['position'] and \
                            stopped_seek['target']==after_seek_tick['position'],{
                        'pauseMustStopActiveSeekAtCurrentPosition':{
                            'afterSeekTick':after_seek_tick,'afterPause':stopped_seek,
                            'requestedSeekTarget':seek_target}}
                    case['lifecycleReentry'].update({'phase':'one-x-and-stop-seek',
                        'passiveFollowingOneX':passive_follow_ticks,
                        'pauseStopsSeek':{'beforeBacklog':before_backlog,
                            'afterBacklog':after_backlog,'seekTarget':seek_target,
                            'afterSeekTick':after_seek_tick,'afterPause':stopped_seek}});save()
                    request(observer,'spectator-playback',run=stopped_seek['run'],action='live')
                    deadline=time.monotonic()+60
                    while True:
                        call(observer,'t=>host.tickTo(t,false)',0xfffffffe)
                        current=call(observer,'host.nativePlayback()')
                        if current and current['following'] and not current['seeking'] and \
                                current['position']>=current['edge']:
                            break
                        assert time.monotonic()<deadline,{'returningToLiveAfterCancelledSeek':current,
                            'observerStatus':snapshot(observer,False)}
                        observer.wait_for_timeout(1)

                    # Hold the real source transports open while the fixture
                    # stops driving source native frames. This isolates the
                    # spectator's healthy idle/pause behavior from gameplay
                    # Menu capture and its input-delay boundary.
                    source_hold_snapshots=[snapshot(page,False) for page in players]
                    assert all(row and not row['error'] for row in source_hold_snapshots),{
                        'sourceHoldSnapshots':source_hold_snapshots}
                    source_frontier=[row['net'][3] for row in source_hold_snapshots]
                    case['lifecycleReentry'].update({'phase':'test-source-hold',
                        'sourceHoldMode':'fixture stops source native progression; player data sockets stay open',
                        'sourceFrontier':source_frontier,
                        'sourceSnapshotsAtHold':source_hold_snapshots});save()
                    current=call(observer,'host.nativePlayback()')
                    assert current and not snapshot(observer,False)['error'],{
                        'sourceFrontier':source_frontier,'nativePlayback':current,
                        'observerStatus':snapshot(observer,False)}
                    assert current['following'] and not current['paused'] and \
                            current['position']>=current['edge'],{
                        'observerNotAtLiveEdgeBeforeSourceHold':current}

                    def hold_healthy_idle(label,seconds,viewer_pause):
                        before=call(observer,'host.nativePlayback()')
                        assert before and not snapshot(observer,False)['error'],{
                            'label':label,'nativePlayback':before,'observerStatus':snapshot(observer,False)}
                        deadline=time.monotonic()+seconds;polls=0
                        while time.monotonic()<deadline:
                            call(observer,'t=>host.tickTo(t,false)',0xfffffffe)
                            current=call(observer,'host.nativePlayback()');status=snapshot(observer,False)
                            source_status=[snapshot(page,False) for page in players]
                            assert current and not status['error'] and status['spectator'] and \
                                status['spectator'][1]==1 and status['spectator'][7]==1,{
                                    'label':label,'nativePlayback':current,'observerStatus':status}
                            assert all(row['net'][3]==source_frontier[seat] and not row['error']
                                for seat,row in enumerate(source_status)),{
                                    'label':label,'sourceNextFrame':source_frontier,'sources':source_status}
                            connection_snapshot=connection_status_snapshot()
                            player_sockets=connection_snapshot['players']
                            observer_sockets=connection_snapshot['observer']
                            connection_snapshot={'players':player_sockets,'observer':observer_sockets}
                            case['lastIdleConnectionSnapshot']={'label':label,
                                'wallMonotonic':time.monotonic(),'connections':connection_snapshot}
                            sockets_healthy=all(rows and not rows[-1]['closed']
                                for rows in player_sockets.values()) and \
                                observer_sockets and not observer_sockets[-1]['closed']
                            if not sockets_healthy:save()
                            assert sockets_healthy,{'label':label,'connectionSnapshotAtAssertion':
                                json.dumps(connection_snapshot,separators=(',',':'))}
                            assert current['edge']==before['edge'] and \
                                    current['position']==before['position'] and \
                                    current['received']==before['received'],{
                                'label':label,'before':before,'after':current}
                            if viewer_pause:assert current['paused'],{'label':label,'playback':current}
                            else:assert current['following'] and not current['paused'],{
                                'label':label,'playback':current}
                            polls+=1;observer.wait_for_timeout(50)
                        return {'label':label,'wallSeconds':round(seconds,3),'pumpPolls':polls,
                            'sourceHoldMode':'fixture stopped source native progression; player data sockets stay open',
                            'sourceNextFrame':source_frontier,
                            'observerPlaybackBefore':before,'observerPlaybackAfter':current,
                            'observerSocketOpen':True,'allPlayerSocketsOpen':True,
                            'allRuntimeSocketsOpen':True}

                    source_idle_hold=hold_healthy_idle('test-source-hold-over-15s',16.2,False)
                    current=call(observer,'host.nativePlayback()')
                    request(observer,'spectator-playback',run=current['run'],action='pause')
                    deadline=time.monotonic()+10
                    while True:
                        call(observer,'t=>host.tickTo(t,false)',0xfffffffe)
                        current=call(observer,'host.nativePlayback()')
                        if current and current['paused'] and not current['seeking']:break
                        assert time.monotonic()<deadline,{'waitingForViewerPause':current}
                        observer.wait_for_timeout(1)
                    observer_pause_hold=hold_healthy_idle('observer-local-pause-over-15s',16.2,True)
                    case['lifecycleReentry'].update({'phase':'complete',
                        'entries':lifecycle_entries,'sourceIdleHold':source_idle_hold,
                        'observerLocalPauseHold':observer_pause_hold,
                        'sourceHoldMode':'fixture stopped source native progression; player data sockets stay open',
                        'sameObserverId':observer_id,'sameRunSerial':started['serial']});save()
                    case.update({'passed':True,'phase':'complete','runtimeSockets':runtime_sockets_snapshot(),
                        'observerPeer':call(observer,'host.observerPeer()')})
                    save()
                    print('TH08 observer repeated admission and >15s pauses: PASS',flush=True)
                    continue
                # Browser controls must not become an extra input participant.
                request(observer,'keyboard',code='KeyX',down=True)
                request(observer,'direct-touch',type='down',id=31,x=.5,y=.7)
                request(observer,'direct-touch',type='move',id=31,x=.8,y=.3)
                request(observer,'touch-controls',controls={'focusEnabled':True,'fireEnabled':True,'bombSerial':1})
                case['checkpoints']=[]
                if args.stage_two_controls:
                    case['checkpoints'].append({'frame':args.frames,'canonical':[s['canonical'] for s in caught]})
                    values=caught
                else:
                    for target in sorted(set(range(179,args.frames+1,60))|{args.frames}):
                        values,_=advance(target,True)
                        case['checkpoints'].append({'frame':target,'canonical':[s['canonical'] for s in values]});save()
                        assert all(s['canonical']==values[0]['canonical'] for s in values),{'frame':target,'values':values}
                case.update({'phase':'comparison','values':values,'writeProbe':writes,'peer':call(observer,'host.observerPeer()')});save()
                assert all(s['canonical']==values[0]['canonical'] for s in values),values
                assert all(s['state'][:2]+s['state'][3:]==values[0]['state'][:2]+values[0]['state'][3:] for s in values),values
                assert all(s['driver'][6:9]==values[0]['driver'][6:9] for s in values),values
                viewer=values[-1]
                assert viewer['state'][2]==-1 and viewer['driver'][3:6]==[0,0,0],viewer
                assert viewer['driver'][9]==0 and viewer['driver'][12]==3,viewer
                assert case['peer']=={'route':'spectator','peers':0,'canSend':False},case['peer']
                assert request(observer,'read',path='score.dat')['bytes']==sentinel
                assert all(not f['path'].startswith('replay/') for f in request(observer,'list')['files'])
                # Native Pause then Restart, through player input. The observer
                # consumes that confirmed selection and ends this admitted run;
                # the players complete generation one on the same transport.
                case['phase']='restart';save();deadline=time.monotonic()+120;finished=None
                while True:
                    ready=True
                    for seat,page in enumerate(players):
                        s=snapshot(page,False);n=s['net']
                        if n[10]==0 or n[3]<120:
                            buttons=8 if seat==0 and n[10]==0 and n[3]==args.frames+21 else 16384 if seat==0 and n[10]==0 and n[3]==args.frames+36 else 0
                            if n[2]:call(page,'v=>host.buttons(v)',buttons)
                            s=call(page,'host.physicalTick(false)')
                        else:s=call(page,'t=>host.tickTo(t,false)',119)
                        ready=ready and s['net'][10]==1 and s['net'][4]==119 and s['net'][5]>=119 and s['net'][6]==0xffffffff
                    if finished is None:
                        s=call(observer,'host.physicalTick(false)')
                        if s['spectator'][2]:finished=s
                    if ready and finished is not None:break
                    assert time.monotonic()<deadline,{'players':[snapshot(page) for page in players],'observer':snapshot(observer)}
                    players[0].wait_for_timeout(1)
                assert finished['net'][10]==0 and finished['spectator'][1:3]==[1,1],finished
                call(observer,'host.finishCallback()')
                observer.wait_for_function("host.events.filter(e=>e.event==='exit').length===1",timeout=15000)
                assert not snapshot(observer)['app']
                assert request(observer,'read',path='score.dat')['bytes']==sentinel
                assert call(players[0],'host.lobbyOpen()')
                restarted=[snapshot(page) for page in players]
                assert all(s['canonical']==restarted[0]['canonical'] for s in restarted),restarted
                case.update({'passed':True,'phase':'complete','observerFinished':finished,'playersRestarted':restarted,
                             'persistentRecordsUnchanged':True,'lobbyRetained':True});save()
                print('TH08 observer',args.mode,count,'players: PASS',flush=True)
            except BaseException:
                case['latest']=[snapshot(page) for page in pages]
                case['runtimeSocketsAtFailure']=runtime_sockets_snapshot()
                case['nativeSpectatorDiagnostics']=[call(page,'host.spectatorDiagnostics()') for page in pages]
                try:
                    checkpoint_frame=case.get('stageTwoBoundary',{}).get('firstStage2CheckpointFrame')
                    if checkpoint_frame is not None:
                        case['rngAtFailure']={'checkpointFrame':checkpoint_frame,
                            'checkpointBoundary':'NextFrame 187, before input 187',
                            'sourceCheckpointByPlayer':([capture['rng'] for capture in source_checkpoint_captures]
                                if source_checkpoint_captures else
                                [call(page,'f=>host.rngAt(f)',checkpoint_frame) for page in players]),
                            'runtimeRngByPage':[call(page,'host.spectatorRng()') for page in pages]}
                        failed_playback=call(observer,'host.nativePlayback()')
                        failure_capture=(source_checkpoint_captures
                            if source_checkpoint_captures and
                                all(capture['net'][3]==checkpoint_frame for capture in source_checkpoint_captures)
                            else None)
                        source_canonical=(failure_capture[0]['canonical'] if failure_capture else
                            call(players[0],'f=>host.canonicalAt(f)',checkpoint_frame))
                        viewer_canonical=snapshot(observer)['canonical']
                        differing_groups=([group_names[i-2] for i in range(2,13)
                            if source_canonical and viewer_canonical and source_canonical[i]!=viewer_canonical[i]]
                            if 'group_names' in locals() else ['players'])
                        case['restoreFailureFirstDifferenceBytes']=byte_difference_report(observer,players,
                            checkpoint_frame,differing_groups,failed_playback,failure_capture)
                        case['restoreFailureCheckpointEvidence']={'playback':failed_playback,
                            'sourceCaptureSummary':capture_summary(failure_capture) if failure_capture else None,
                            'sourceCanonical':source_canonical,'viewerCanonical':viewer_canonical,
                            'groups':differing_groups}
                except BaseException as diagnostic_error:
                    case['rngAtFailureError']=str(diagnostic_error)
                case['events']=[call(page,"host.events.filter(e=>e.event==='exit'||e.event==='error'||(e.event==='runtime-info'&&e.spectatorPlayback)).slice(-16)") for page in pages];raise
            finally:
                for page in pages:
                    try:call(page,'host.autoStop()')
                    except BaseException:pass
                for context in contexts:context.close()
        assert not report['errors'],report['errors'];report['passed']=True
    except BaseException as error:report['error']=str(error);raise
    finally:browser.close();save()
