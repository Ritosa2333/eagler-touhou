"""Real RTC, raw SDL Pause -> Restart -> Yes, and optional retired-ACK interleave.

Requires the existing portable/multiplayer/serve.mjs HTTP surface and a real
Launcher relay for signaling. This checker owns two independent desktop
Chromium processes. It calls stopped sdl_loop_tick through netTicks(1), so this
is a lifecycle/correctness test, not an rAF cadence or input-to-photon benchmark.
It never uses fixtures, automatic wipe/retry, peerAdvance, remote-input writes,
or multiplayer_packet_apply. The optional late packet is an unchanged native
retirement-cache ACK sent again over its original RTC control DataChannel.
"""
import argparse
import hashlib
import json
import time
import traceback
import uuid
from pathlib import Path
from urllib.parse import parse_qsl, urlencode, urlsplit, urlunsplit

from playwright.sync_api import sync_playwright


# Test-only wrappers. Production transport code and queue ownership stay intact.
# A terminal ACK is identified by BOTH the complete wire shape and a native
# pump entered with Retired=1 / generation_pending=1 in the old generation.
WIRE_SCRIPT = r"""(options => {
  const originalSend = RTCDataChannel.prototype.send;
  const events = {nativeTerminalSends:[],receivedTerminalAcks:[],held:null,released:null};
  let armed=false, held=null, expectedHex=null, expectedSource=null;
  let lastOldFrame=-1, matchedReceipts=0, eventOverflow=0;
  const hex=bytes=>Array.from(bytes,x=>x.toString(16).padStart(2,'0')).join('');
  const copy=data=>data instanceof ArrayBuffer?new Uint8Array(data.slice(0)):
    ArrayBuffer.isView(data)?new Uint8Array(data.buffer.slice(data.byteOffset,data.byteOffset+data.byteLength)):null;
  const keep=(rows,value)=>{if(rows.length<1024)rows.push(value);else ++eventOverflow;};
  const packet=data=>{
    const bytes=copy(data);if(!bytes||bytes.length!==44)return null;
    // TH10's common protocol uses EANP, version 4, Input=1.
    if(bytes[0]!==69||bytes[1]!==65||bytes[2]!==78||bytes[3]!==80||
       bytes[4]!==4||bytes[5]!==1||bytes[7]!==2||bytes[42]!==0||bytes[43]!==0)return null;
    const v=new DataView(bytes.buffer),latest=v.getUint32(24,true),ack=v.getUint32(28,true);
    if(bytes[6]>1||latest===0xffffffff||latest!==ack||v.getUint32(32,true)!==0xffffffff||
       v.getUint32(36,true)!==((latest+1)>>>0)||v.getInt16(40,true)!==0)return null;
    const low=v.getUint32(8,true),high=v.getUint32(12,true);
    if(low!==0x51524146||high!==0x11223344)return null;
    return {bytes,hex:hex(bytes),length:bytes.length,sessionLow:low,sessionHigh:high,
      sessionId:high.toString(16).padStart(8,'0')+low.toString(16).padStart(8,'0'),
      sender:bytes[6],players:bytes[7],sequence:v.getUint32(16,true),
      ackSequence:v.getUint32(20,true),latestFrame:latest,ackFrame:ack,
      firstInputFrame:0xffffffff,senderFrame:v.getUint32(36,true),inputCount:0};
  };
  const brief=p=>{const {bytes,...rest}=p;return rest;};
  const snapshot=()=>{
    const smoke=globalThis.multiplayerSmoke,wire=globalThis.__eaglerPeerTransport;
    const value={nowMs:performance.now(),net:smoke.netStatus(),generation:smoke.generationStatus(),
      calibration:smoke.calibrationStatus(),life:smoke.lifecycle(),results:smoke.resultsStatus(),
      native:smoke.nativeStatus(),transport:smoke.transportStatus(),route:wire?.route??null,
      networkError:smoke.networkError(),errorDetail:smoke.errorDetail(),
      wire:{held:!!held,released:!!events.released,matchedReceipts}};
    if(value.generation[1]===0&&value.net[3]>=0)lastOldFrame=value.net[3];
    return value;
  };
  const api={
    before:null,snapshot,
    arm(){armed=true;return true;},
    attach(){
      const wire=globalThis.__eaglerPeerTransport;
      if(!wire||typeof wire.receiveBinary!=='function')throw Error('Native browser transport missing');
      if(wire.__manualRestartObserver)return true;
      wire.__manualRestartObserver=true;
      const receive=wire.receiveBinary;
      wire.receiveBinary=function(data){
        const p=armed?packet(data):null;
        let receipt=null;
        if(p){
          const at=snapshot(),matched=expectedHex!==null&&p.hex===expectedHex&&p.sender===expectedSource;
          if(matched)++matchedReceipts;
          receipt={...brief(p),at,matchedExpected:matched,transportWasOpen:!this.closed,
            originalReceiveReturned:false,
            observedAt:'real RTC onmessage -> existing receiveBinary; native status sampled before enqueue'};
          keep(events.receivedTerminalAcks,receipt);
        }
        const result=receive.call(this,data);
        if(receipt)receipt.originalReceiveReturned=true;
        return result;
      };
      return true;
    },
    expect(bytes,source){
      const p=packet(new Uint8Array(bytes));
      if(!p||p.sender!==source||source===options.seat)throw Error('Expected old ACK provenance mismatch');
      expectedHex=p.hex;expectedSource=source;return true;
    },
    heldPacket(){return held?{...events.held,bytes:Array.from(held.bytes)}:null;},
    release(){
      if(!held||events.released)throw Error('No unreleased native retired ACK');
      const wire=globalThis.__eaglerPeerTransport,peer=wire?.peers.get(held.target);
      if(wire?.route!=='rtc'||peer?.controlDc!==held.channel||held.channel.readyState!=='open')
        throw Error('Original RTC control channel no longer owns the held ACK');
      const at=snapshot();
      originalSend.call(held.channel,new Uint8Array(held.bytes));
      events.released={...brief(held.packet),target:held.target,label:held.channel.label,at,
        sendReturned:true,via:'original RTCDataChannel.send; unchanged saved native bytes'};
      return events.released;
    },
    read(){return {...events,eventOverflow,armed,matchedReceipts,lastOldFrame};}
  };
  globalThis.__manualRestartWire=api;
  RTCDataChannel.prototype.send=function(data){
    const p=armed?packet(data):null,wire=globalThis.__eaglerPeerTransport;
    if(p&&wire?.route==='rtc'&&p.sender===options.seat){
      const target=[...wire.peers].find(([,peer])=>peer.controlDc===this)?.[0];
      if(target!==undefined){
        const before=api.before;
        const retiredPump=!!before&&before.generation[1]===0&&before.generation[2]===1&&
          before.generation[3]===1&&before.net[0]===0;
        const metadata={...brief(p),target,label:this.label,atMs:performance.now(),
          pumpBefore:before,retiredPump,lastObservedOldFrame:lastOldFrame,
          terminalMatchesOldFrame:p.latestFrame===lastOldFrame};
        // Skip Retire()'s first synchronous send: retired_ is assigned after it.
        // The controlled poll-only window below captures an existing cache
        // retransmission while the old runtime is observably retired.
        if(options.lateTerminalAck&&options.seat===1&&!held&&retiredPump&&p.latestFrame===lastOldFrame){
          held={bytes:new Uint8Array(p.bytes),packet:p,target,channel:this};
          events.held={...metadata,reason:'native retired generation pump; terminal ACK cache retransmission'};
          keep(events.nativeTerminalSends,{...metadata,held:true});return;
        }
        keep(events.nativeTerminalSends,{...metadata,held:false});
      }
    }
    return originalSend.call(this,data);
  };
})"""

STEP = r"""action=>{
  const wire=globalThis.__manualRestartWire,smoke=globalThis.multiplayerSmoke;
  const before=wire.snapshot();wire.before=before;
  if(action==='tick')smoke.netTicks(1);
  else if(!smoke.pollNetwork())throw Error('Native network pump failed: '+smoke.errorDetail());
  return {before,after:wire.snapshot()};
}"""

RTC_EVIDENCE = r"""async()=>{
  const wire=globalThis.__eaglerPeerTransport,peers=[];
  if(!wire)return {route:null,peers};
  for(const [seat,peer] of wire.peers){
    const stats=await peer.pc.getStats();let pair=null;
    for(const item of stats.values())if(item.type==='transport'&&item.selectedCandidatePairId){
      pair=stats.get(item.selectedCandidatePairId);if(pair)break;
    }
    if(!pair)for(const item of stats.values())if(item.type==='candidate-pair'&&
      item.state==='succeeded'&&(item.nominated||item.selected)){pair=item;break;}
    const candidate=id=>{
      const item=id&&stats.get(id);if(!item)return null;
      return {id:item.id,address:item.address??item.ip??null,port:item.port??null,
        protocol:item.protocol??null,candidateType:item.candidateType??null};
    };
    peers.push({seat,connectionState:peer.pc.connectionState,iceConnectionState:peer.pc.iceConnectionState,
      input:{label:peer.inputDc?.label,state:peer.inputDc?.readyState},
      control:{label:peer.controlDc?.label,state:peer.controlDc?.readyState},
      pair:pair?{id:pair.id,state:pair.state,nominated:pair.nominated,
        bytesSent:pair.bytesSent,bytesReceived:pair.bytesReceived,
        local:candidate(pair.localCandidateId),remote:candidate(pair.remoteCandidateId)}:null,
      dataChannels:[...stats.values()].filter(x=>x.type==='data-channel').map(x=>({
        label:x.label,state:x.state,messagesSent:x.messagesSent,messagesReceived:x.messagesReceived,
        bytesSent:x.bytesSent,bytesReceived:x.bytesReceived}))});
  }
  return {route:wire.route,failed:wire.failed,disconnected:wire.disconnected,error:wire.error,peers};
}"""


def call(page, expression, argument=None):
    return page.evaluate(expression, argument) if argument is not None else page.evaluate(expression)


def require(condition, message, detail=None):
    if not condition:
        raise AssertionError(message + (': ' + json.dumps(detail, ensure_ascii=False) if detail is not None else ''))


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--url', required=True)
    parser.add_argument('--relay', required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--late-terminal-ack', action='store_true',
                        help='hold one native retired P2 ACK, then resend its exact bytes during P1 new calibration')
    parser.add_argument('--observe-after-canonical-mismatch', action='store_true',
                        help='diagnostic only: continue lifecycle observations after a raw canonical mismatch; final result still fails')
    parser.add_argument('--headed', action='store_true')
    parser.add_argument('--browser-channel', default='default')
    parser.add_argument('--renderer', choices=['hardware', 'swiftshader'], default='hardware')
    parser.add_argument('--frames', type=int, default=119, help='exact final simulated/confirmed boundary in generation 1')
    parser.add_argument('--tick-ms', type=float, default=16.0, help='real wait between one native tick per endpoint')
    parser.add_argument('--timeout', type=float, default=120.0, help='seconds per bounded lifecycle phase')
    args = parser.parse_args()
    if not 30 <= args.frames <= 600:
        parser.error('frames must be in 30..600')
    if not 4 <= args.tick_ms <= 50:
        parser.error('tick-ms must be in 4..50')
    if not 10 <= args.timeout <= 300:
        parser.error('timeout must be in 10..300')
    return args


class ManualRestart:
    def __init__(self, args, report):
        self.args, self.report = args, report
        self.pages, self.contexts, self.browsers, self.current = [], [], [], []
        self.started = time.monotonic()
        self.retirement_window = None
        self.injected = False
        self.phase = 'open'

    def read(self, page):
        return call(page, '()=>__manualRestartWire.snapshot()')

    def open(self, playwright):
        flags = ['--disable-background-timer-throttling', '--disable-renderer-backgrounding',
                 '--disable-backgrounding-occluded-windows', '--autoplay-policy=no-user-gesture-required']
        options = {'headless': not self.args.headed}
        if self.args.renderer == 'hardware':
            options['channel'] = 'chromium'
            flags += ['--ignore-gpu-blocklist', '--use-gl=angle', '--use-angle=d3d11', '--enable-gpu']
        else:
            flags += ['--enable-unsafe-swiftshader', '--use-angle=swiftshader']
        if self.args.browser_channel != 'default':
            options['channel'] = self.args.browser_channel
        self.report['browserLaunch'] = {**options, 'args': flags, 'independentProcesses': 2}
        for seat in range(2):
            browser = playwright.chromium.launch(args=flags, **options)
            self.browsers.append(browser)
            context = browser.new_context(service_workers='block', viewport={'width': 800, 'height': 600})
            self.contexts.append(context)
            context.add_init_script(WIRE_SCRIPT + '(' + json.dumps({
                'seat': seat, 'lateTerminalAck': self.args.late_terminal_ack}) + ');')
            page = context.new_page()
            self.pages.append(page)
            page.on('pageerror', lambda error, local=seat: self.report['errors'].append({
                'seat': local, 'error': str(error)}))
            page.goto(self.args.url)
            page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
            self.report['browsers'].append(browser.version)
            self.report['identities'].append(call(page, 'multiplayerSmoke.identity()'))
            call(page, 'seat=>multiplayerSmoke.startAdonis([[0,0],[1,1]],seat)', seat)
        # Create both native worlds before starting the relay's RTC route clock.
        query = dict(parse_qsl(urlsplit(self.args.relay).query))
        query.update(room='manual-th10-' + uuid.uuid4().hex[:16], run='1')
        parts = urlsplit(self.args.relay)
        relay = urlunsplit((parts.scheme, parts.netloc, parts.path, urlencode(query), parts.fragment))
        self.report['roomRelay'] = relay
        for page in self.pages:
            require(call(page, 'url=>multiplayerSmoke.connect(url)', relay), 'Native connect rejected')
            call(page, '()=>__manualRestartWire.attach()')
            self.current.append(self.read(page))
        identities = self.report['identities']
        require(identities[0] == identities[1], 'Peer runtime identities differ', identities)
        require(identities[0]['variant'] == 'multiplayer', 'Wrong product identity', identities[0])
        self.report['initialGeneration'] = [row['generation'] for row in self.current]

    def guard(self, seat, row):
        require(not self.report['errors'], 'Browser page errors', self.report['errors'])
        require(row['native'][2] == 0, 'Native application error', {'seat': seat, 'row': row})
        require(row['transport'][3] == 0 and not row['networkError'],
                'Native transport failure', {'seat': seat, 'row': row})
        require(row['route'] in [None, 'rtc'], 'Actual transport fell back from RTC', row)
        require(row['generation'][1] <= 1, 'Unexpected automatic second restart', row)
        require(row['calibration'][0] == 1 and row['calibration'][1] not in [6, 8],
                'Calibration failed/unavailable', row)
        require(row['life'][8:10] == [0, 0] and row['net'][6] == 0,
                'Pure Delay entered rollback or captured undo bytes', row)
        calibration = row['calibration']
        if calibration[1] == 5:
            require(calibration[6:9] == [1, 0, 1] and calibration[10] == 0,
                    'Manual pure Delay contract changed', row)
        if row['generation'][1] == 1 and calibration[1] in [1, 2, 3, 4]:
            require(row['net'][1] == 0 and row['net'][2:4] == [0, -1],
                    'New gameplay passed its calibration/frame-zero gate', row)

    def record(self, seat, action, value):
        row = value['after']
        self.current[seat] = row
        require(len(self.report['trace']) < 20000, 'Bounded native trace exhausted')
        self.report['trace'].append({
            'tMs': round((time.monotonic() - self.started) * 1000, 3),
            'seat': seat, 'phase': self.phase, 'action': action, **row})
        self.guard(seat, row)

    def maybe_inject(self):
        if not self.args.late_terminal_ack or self.injected:
            return
        source, target = self.current[1], self.current[0]
        if not source['wire']['held'] or target['generation'][1] != 1:
            return
        if target['calibration'][1] not in [2, 3, 4] or target['net'][1]:
            return
        saved = call(self.pages[1], '()=>__manualRestartWire.heldPacket()')
        require(saved and saved['retiredPump'] and saved['terminalMatchesOldFrame'],
                'Held ACK lacks a proven native retirement-cache source', saved)
        require(saved['pumpBefore']['generation'][1:4] == [0, 1, 1],
                'Held source was not the retired old generation', saved)
        require(saved['sender'] == 1 and saved['target'] == 0 and saved['length'] == 44,
                'Held ACK roles/shape differ', saved)
        saved['sha256'] = hashlib.sha256(bytes(saved['bytes'])).hexdigest()
        call(self.pages[0], 'v=>__manualRestartWire.expect(v.bytes,v.source)',
             {'bytes': saved['bytes'], 'source': 1})
        released = call(self.pages[1], '()=>__manualRestartWire.release()')
        require(released['sendReturned'] and released['hex'] == saved['hex'],
                'Saved native ACK was not sent unchanged on RTC', released)
        self.report['lateTerminalAck'] = {
            'source': saved, 'receiverBeforeSend': target, 'release': released,
            'provenance': 'SessionChannel retirement-cache wire shape plus native retired/pending generation pump',
            'classificationSource': 'third_party/eagler-common/src/netplay/SessionChannel.cpp: Retire / SendRetired'}
        self.injected = True
        print('manual-restart: saved retired ACK sent over real RTC during new calibration', flush=True)

    def pump(self, target=None, generation=None):
        for seat, page in enumerate(self.pages):
            before = self.current[seat]
            action = 'poll' if target is not None and before['generation'][1] == generation and before['net'][3] >= target else 'tick'
            # Explicit, bounded lifecycle skew. A stopped native tick preserves
            # Retired=1 while the ordinary network pump reaches its 16 ms resend.
            if (self.args.late_terminal_ack and seat == 1 and not before['wire']['held'] and
                    before['generation'][1:4] == [0, 1, 1]):
                action = 'poll'
                if self.retirement_window is None:
                    self.retirement_window = {'started': time.monotonic(), 'polls': 0, 'before': before}
                require(time.monotonic() - self.retirement_window['started'] <= 0.15,
                        'No proven native retired ACK in bounded poll-only window', before)
                self.retirement_window['polls'] += 1
            value = call(page, STEP, action)
            self.record(seat, action, value)
            if (self.retirement_window is not None and 'milliseconds' not in self.retirement_window and
                    seat == 1 and value['after']['wire']['held']):
                self.retirement_window['milliseconds'] = round(
                    (time.monotonic() - self.retirement_window['started']) * 1000, 3)
                self.report['retirementPollWindow'] = {key: value for key, value in self.retirement_window.items()
                                                        if key != 'started'}
        self.maybe_inject()
        self.pages[0].wait_for_timeout(self.args.tick_ms)

    def wait(self, label, predicate, target=None, generation=None):
        self.phase = label
        deadline, next_progress = time.monotonic() + self.args.timeout, time.monotonic() + 5
        while not predicate(self.current):
            require(time.monotonic() < deadline, label + ' timed out', self.current)
            self.pump(target, generation)
            if time.monotonic() >= next_progress:
                print('manual-restart: waiting', label, json.dumps([{
                    'generation': x['generation'][1:4], 'net': x['net'][1:6],
                    'calibration': x['calibration'][1:4], 'results': x['results']}
                    for x in self.current]), flush=True)
                next_progress = time.monotonic() + 5
        self.report['milestones'].append({'name': label, 'peers': self.current[:]})
        print('manual-restart:', label, flush=True)

    def pulse(self, code, label, accepted):
        down = self.current[0]
        call(self.pages[0], 'v=>multiplayerSmoke.key(v.code,v.down)', {'code': code, 'down': True})
        self.report['actions'].append({'name': label, 'key': code, 'down': True, 'before': down})
        try:
            self.wait(label, lambda rows: accepted(rows[0]))
        finally:
            call(self.pages[0], 'v=>multiplayerSmoke.key(v.code,v.down)', {'code': code, 'down': False})
        release = self.current[0]
        self.report['actions'].append({'name': label, 'key': code, 'down': False, 'before': release})
        # D=1 plus one possibly latched capture and a further complete frame.
        # Let the raw release reach BOTH peers before another rising edge.
        release_frame = release['net'][2] + 3
        self.wait(label + ' raw release applied',
                  lambda rows: all(x['generation'][1] == 0 and x['net'][3] >= release_frame for x in rows))

    @staticmethod
    def menu(row, state, selected=None, timer=None):
        results = row['results']
        return (results is not None and results['state'] == state and
                (selected is None or results['selected'] == selected) and
                (timer is None or results['timer'] >= timer))

    def boundary(self, generation, frame, label):
        def reached(rows):
            return all(x['generation'][1] == generation and x['net'][1] == 1 and
                       x['net'][3] == frame and x['net'][4] >= frame and
                       x['net'][5] == -1 and x['net'][9] == 1 for x in rows)
        self.wait(label, reached, target=frame, generation=generation)
        for row in self.current:
            require(row['net'][3] == frame, 'Exact comparison boundary overrun', row)
            calibration = row['calibration']
            require(calibration[1] == 5 and calibration[2] == 129 and calibration[3] >= 96,
                    'Full native calibration did not finish', calibration)
            require(row['transport'][13] == 1, 'Native transport does not report RTC', row)
        values = [call(page, '''()=>({state:multiplayerSmoke.status(),canonical:multiplayerSmoke.canonical(),
            audio:multiplayerSmoke.audioStatus(),memory:multiplayerSmoke.memoryStatus(),
            net:multiplayerSmoke.netStatus(),generation:multiplayerSmoke.generationStatus(),
            calibration:multiplayerSmoke.calibrationStatus(),scene:multiplayerSmoke.sceneOwners(),
            portableCanonical:multiplayerSmoke.portableCanonical()})''') for page in self.pages]
        checkpoint = {'name': label, 'generation': generation, 'lastSimulatedFrame': frame,
                      'confirmedThrough': [x['net'][4] for x in values], 'peers': values}
        self.report['checkpoints'].append(checkpoint)
        gate_detail = {'name': label, 'generation': generation, 'lastSimulatedFrame': frame,
                       'confirmedThrough': checkpoint['confirmedThrough']}
        require(all(len(x['canonical']) == 44 and x['canonical'][0] == 2 for x in values),
                'Complete canonical schema 2 required', gate_detail)
        checkpoint['canonicalDifferences'] = [
            {'word': i, 'p0': left, 'p1': right}
            for i, (left, right) in enumerate(zip(values[0]['canonical'], values[1]['canonical'])) if left != right]
        if checkpoint['canonicalDifferences']:
            failure = {**gate_detail, 'differences': checkpoint['canonicalDifferences']}
            self.report.setdefault('canonicalFailures', []).append(failure)
            require(self.args.observe_after_canonical_mismatch, 'Full 44-word canonical mismatch', failure)
            print('manual-restart: raw canonical mismatch retained; diagnostic continuation only', flush=True)
        checkpoint['portableCanonicalEqual'] = values[0]['portableCanonical'] == values[1]['portableCanonical']
        require(values[0]['audio'] == values[1]['audio'], 'Confirmed audio mismatch', gate_detail)
        require(values[0]['state'][:2] + values[0]['state'][3:] ==
                values[1]['state'][:2] + values[1]['state'][3:], 'Gameplay status mismatch', gate_detail)
        rtc = [call(page, RTC_EVIDENCE) for page in self.pages]
        checkpoint['rtc'] = rtc
        for value in rtc:
            require(value['route'] == 'rtc' and len(value['peers']) == 1,
                    'RTC mesh evidence missing', value)
            peer = value['peers'][0]
            require(peer['connectionState'] == 'connected' and peer['input']['state'] == 'open' and
                    peer['control']['state'] == 'open' and peer['pair'] and
                    peer['pair']['state'] == 'succeeded' and peer['pair'].get('bytesReceived', 0) > 0,
                    'Actual connected RTC pair/channel evidence missing', value)
        return values

    def run(self):
        initial = self.boundary(0, 59, 'initial calibration and confirmed frame 59')
        require(all(self.menu(x, 0) and x['life'][6] >= 30 for x in self.current),
                'Gameplay is not ready for normal pause', self.current)
        self.pulse('Escape', 'Pause rising edge', lambda row: self.menu(row, 1))
        self.wait('Pause main menu interactive', lambda rows: all(self.menu(x, 2, 0) for x in rows))
        self.pulse('ArrowDown', 'Select Return to Title', lambda row: self.menu(row, 2, 1))
        require(all(self.menu(x, 2, 1) for x in self.current), 'Host menu selection not shared', self.current)
        self.pulse('ArrowDown', 'Select Restart', lambda row: self.menu(row, 2, 2))
        require(all(self.menu(x, 2, 2) for x in self.current), 'Restart selection not shared', self.current)
        self.pulse('KeyZ', 'Open Restart confirmation', lambda row: self.menu(row, 4))
        self.wait('Restart confirmation interactive, default No',
                  lambda rows: all(self.menu(x, 4, 1, 30) and x['results']['itemCount'] == 2 for x in rows))
        self.pulse('ArrowUp', 'Select Yes', lambda row: self.menu(row, 4, 0, 30))
        require(all(self.menu(x, 4, 0, 30) for x in self.current), 'Yes selection not shared', self.current)
        for page in self.pages:
            call(page, '()=>__manualRestartWire.arm()')
        self.pulse('KeyZ', 'Confirm Yes', lambda row: self.menu(row, 5, 0))
        self.wait('Native BeginNextRun generation 1',
                  lambda rows: all(x['generation'][1] == 1 for x in rows))
        # Read-only owner bytes at the fresh graph boundary. Keep the complete
        # canonical gate below; these bytes explain a failure rather than
        # excluding a mismatching owner from the oracle.
        self.report['newGenerationObservation'] = [call(page, '''()=>({
            net:multiplayerSmoke.netStatus(),generation:multiplayerSmoke.generationStatus(),
            calibration:multiplayerSmoke.calibrationStatus(),canonical:multiplayerSmoke.canonical(),
            scene:multiplayerSmoke.sceneOwners()})''') for page in self.pages]
        fresh = [x['generation'] for x in self.current]
        old_id = initial[0]['generation'][4] | (initial[0]['generation'][5] << 32)
        expected = old_id ^ 0x9e3779b97f4a7c15
        require(fresh[0][4:8] == fresh[1][4:8], 'New session/seed/ABI differ', fresh)
        require(all((x[4] | (x[5] << 32)) == expected for x in fresh),
                'BeginNextRun did not create the expected generation identity', fresh)
        self.report['generationTransition'] = {'original': [x['generation'] for x in initial],
                                              'fresh': fresh, 'expectedSessionId': format(expected, '016x')}
        final = self.boundary(1, self.args.frames, 'new calibration, HELLO gate and confirmed continuation')
        require(all(x['net'][2] == self.args.frames + 1 for x in final), 'Wrong NextFrame frontier', final)
        for seat in range(2):
            rows = [x for x in self.report['trace'] if x['seat'] == seat]
            require(any(x['generation'][1:4] == [0, 1, 1] for x in rows),
                    'Native retirement/pending generation was not observed', {'seat': seat})
            require(any(x['results'] and x['results']['state'] == 3 and x['results']['selected'] == 2
                        for x in rows), 'Normal confirmation did not return to Restart close state', {'seat': seat})
            require(any(x['generation'][1] == 1 and x['calibration'][1] in [2, 3, 4] and
                        x['net'][2:4] == [0, -1] for x in rows),
                    'Fresh generation calibration wait was not observed', {'seat': seat})
        wire = [call(page, '()=>__manualRestartWire.read()') for page in self.pages]
        self.report['wire'] = wire
        if self.args.late_terminal_ack:
            require(self.injected and wire[1]['held'] and wire[1]['released'],
                    'Requested old terminal ACK was not captured/sent', wire)
            receipts = [x for x in wire[0]['receivedTerminalAcks'] if x['matchedExpected'] and
                        x['originalReceiveReturned'] and x['transportWasOpen'] and
                        x['at']['generation'][1] == 1 and x['at']['calibration'][1] in [2, 3, 4] and
                        x['at']['net'][1] == 0 and x['at']['route'] == 'rtc']
            require(receipts, 'No actual saved-ACK receipt during new calibration', wire)
            self.report['lateTerminalAck']['receiptsDuringCalibration'] = receipts
            self.report['lateTerminalAck']['completedCalibrationAfterReceipt'] = True
        require(not self.report['errors'], 'Browser errors occurred', self.report['errors'])
        self.report['lifecycleCompleted'] = True
        self.report['canonicalPassed'] = not self.report.get('canonicalFailures')
        require(self.report['canonicalPassed'],
                'Full canonical gate failed; separate lifecycle diagnostics retained',
                self.report.get('canonicalFailures'))

    def capture_failure(self):
        self.report['lastObserved'] = self.current
        for seat, page in enumerate(self.pages):
            try:
                self.report.setdefault('failureNative', []).append({
                    'seat': seat, 'observation': self.read(page),
                    'wire': call(page, '()=>__manualRestartWire.read()')})
            except Exception as error:
                self.report.setdefault('failureCollectionErrors', []).append({'seat': seat, 'error': str(error)})

    def close(self):
        for page in self.pages:
            try:
                call(page, """()=>{for(const code of ['Escape','ArrowDown','ArrowUp','KeyZ'])
                    multiplayerSmoke.key(code,false);multiplayerSmoke.close();}""")
            except Exception as error:
                self.report.setdefault('cleanupErrors', []).append(str(error))
        for context in reversed(self.contexts):
            try:
                context.close()
            except Exception as error:
                self.report.setdefault('cleanupErrors', []).append(str(error))
        for browser in reversed(self.browsers):
            try:
                browser.close()
            except Exception as error:
                self.report.setdefault('cleanupErrors', []).append(str(error))


def main():
    args = parse_args()
    report = {
        'schemaVersion': 1, 'passed': False, 'lifecycleCompleted': False, 'canonicalPassed': False,
        'scope': 'TH10 manual native Restart over actual RTC, two independent desktop browser processes',
        'scopeLimits': 'stopped native ticks; no rAF/performance, WAN, Launcher UI or physical-device acceptance claim',
        'options': {key: str(value) if isinstance(value, Path) else value for key, value in vars(args).items()},
        'checkerSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        'inputTimingRequest': {'version': 4, 'mode': 1, 'automatic': False, 'delay': 1,
                              'reserve': 2, 'smokeBuildWords': [1, 2, 3, 4]},
        'inputSource': 'P1 raw sdl_key through multiplayerSmoke.key; native capture, transport and frame application',
        'canonicalScope': 'all 44 schema-2 words, at equal LastSimulatedFrame with confirmed input through that frame',
        'browsers': [], 'identities': [], 'actions': [], 'milestones': [],
        'checkpoints': [], 'trace': [], 'errors': []}
    test = ManualRestart(args, report)
    try:
        with sync_playwright() as playwright:
            try:
                test.open(playwright)
                test.run()
                report['passed'] = True
            except BaseException:
                test.capture_failure()
                raise
            finally:
                test.close()
        if report.get('cleanupErrors'):
            report['passed'] = False
    except BaseException as error:
        report['passed'] = False
        report['failure'] = str(error)
        report['traceback'] = traceback.format_exc()
    finally:
        report['durationSeconds'] = round(time.monotonic() - test.started, 3)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
    print(json.dumps({'passed': report['passed'], 'output': str(args.output),
                      'lateTerminalAck': args.late_terminal_ack, 'failure': report.get('failure')}), flush=True)
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
