// TH09 uses the shared C++ rollback session and BrowserPeerTransport lanes.
const spectatorMagic = [0x54, 0x39, 0x53, 0x50, 1, 3, 2, 0]; // T9SP/1, 2 players
const spectatorFrameBytes = 46;
export class SharedNetplay {
  constructor(core, {onStatus, onClose, onResult, onTiming = () => {}}) {
    this.core = core;
    this.onStatus = onStatus;
    this.onClose = onClose;
    this.onResult = onResult;
    this.onTiming = onTiming;
    this.timing = null;
    this.connected = false;
    this.timer = null;
    this.active = false;
    this.finished = false;
    this.recovering = false;
    this.disconnected = false;
    this.pumping = false;
    this.spectator = false;
    this.spectatorFrame = 0;
    this.spectatorPending = [];
    this.spectatorHead = 0;
    this.spectatorOutputStopped = false;
    this.spectatorSentFrame = 0;
    this.spectatorReceiveAt = 0;
    this.prepared = false;
    this.acknowledged = false;
    this.side = -1;
  }
  info() {
    const at = this.core._th09_network_info() / 4;
    return Array.from(this.core.HEAPU32.subarray(at, at + 6));
  }
  async connect(options) {
    if (this.connected) throw Error('已连接联机房间');
    if (typeof this.core._th09_peer_spectator_state !== 'function' ||
        typeof this.core._th09_peer_stop_spectators !== 'function')
      throw Error('TH09 Runtime 缺少观战故障隔离，请更新实验 Runtime');
    const url = new URL(options.netplayUrl, location.href);
    const side = Number(options.netplayPlayer);
    const spectator = options.netplaySpectator === true;
    const spectatorId = String(options.netplaySpectatorId || '');
    const query = new URLSearchParams(location.search);
    const names = {rollback: 0, adonis: 1, delay: 1, hybrid: 2};
    // The published global is telemetry, not configuration for a later match.
    const requestedMode = options.netplayAdonisMode ?? query.get('adonis') ?? 0;
    const adonisMode = Object.hasOwn(names, requestedMode) ? names[requestedMode] : Number(requestedMode);
    const inputDelay = Number(options.netplayInputDelay ?? query.get('inputDelay') ?? (adonisMode === 1 ? 4 : adonisMode === 2 ? 2 : 0));
    const automatic = options.netplayInputDelayAuto ?? (adonisMode !== 0 && options.netplayInputDelay === undefined && !query.has('inputDelay'));
    const predictionReserve = options.netplayPredictionReserve ?? 2;
    if (typeof automatic !== 'boolean' || (automatic && !adonisMode) ||
        !Number.isInteger(predictionReserve) || predictionReserve < 1 || predictionReserve > 2)
      throw Error('TH09 实测输入时序配置无效');
    if (!['ws:', 'wss:'].includes(url.protocol) || (!spectator && ![0, 1].includes(side)) ||
        Number(options.netplayPlayerCount) !== 2 || !/^th09mp-\d{4}$/.test(url.searchParams.get('room') || '') ||
        !/^\d+$/.test(url.searchParams.get('run') || '') ||
        (spectator ? !/^[A-Za-z0-9_-]{8,64}$/.test(spectatorId) ||
          url.searchParams.get('spectator') !== spectatorId || url.searchParams.has('player') :
          Number(url.searchParams.get('player')) !== side || url.searchParams.has('spectator')) ||
        (url.searchParams.has('players') && url.searchParams.get('players') !== '2') ||
        !Number.isInteger(inputDelay) || inputDelay < 0 || inputDelay > 9 ||
        !Number.isInteger(adonisMode) || adonisMode < 0 || adonisMode > 2) throw Error('TH09 房间配置无效');
    const loadouts = options.netplayLoadouts;
    if (!Array.isArray(loadouts) || loadouts.length < 2 ||
        loadouts.slice(0, 2).some(item => !Number.isInteger(item?.character) || item.character < 0 || item.character >= 16))
      throw Error('TH09 角色配置无效');
    const version = await fetch('version.json').then(response => {
      if (!response.ok) throw Error('TH09 版本读取失败');
      return response.json();
    });
    if (!/^[a-f0-9]{24}$/.test(version.build)) throw Error('TH09 版本无效');
    this.build = version.build;
    this.buildBytes = Uint8Array.from(version.build.match(/../g).map(pair => Number.parseInt(pair, 16)));
    this.options = options;
    this.side = side;
    this.spectator = spectator;
    this.spectatorFrame = 0;
    this.spectatorPending.length = 0;
    this.spectatorHead = 0;
    this.spectatorOutputStopped = false;
    this.spectatorSentFrame = 0;
    this.spectatorReceiveAt = 0;
    this.finished = false;
    this.recovering = this.disconnected = false;
    globalThis.__eaglerNetplayCalibrationSuspended = false;
    this.inputDelay = inputDelay;
    this.adonisMode = adonisMode;
    this.automatic = automatic;
    this.predictionReserve = predictionReserve;
    this.timing = null;
    this.calibrationRoute = null;
    this.prepared = this.acknowledged = false;
    delete globalThis.__eaglerNetplayTiming;
    delete globalThis.__eaglerNetplayInputDelayFrames;
    globalThis.__eaglerNetplayLanActive = false;
    globalThis.__eaglerNetplayTransport = 'connecting';
    const encoded = new TextEncoder().encode(url.href);
    if (encoded.length >= 1024) throw Error('TH09 联机地址过长');
    const pointer = this.core._th09_peer_url_buffer();
    this.core.HEAPU8.set(encoded, pointer);
    this.core.HEAPU8[pointer + encoded.length] = 0;
    if (spectator) {
      const id = new TextEncoder().encode(spectatorId), at = this.core._th09_peer_spectator_id_buffer();
      this.core.HEAPU8.set(id, at); this.core.HEAPU8[at + id.length] = 0;
    }
    if (!(spectator ? this.core._th09_peer_connect_spectator() : this.core._th09_peer_connect(side)))
      throw Error('TH09 联机传输初始化失败');
    this.connected = true;
    this.startupNoticeAt = 0;
    this.startupTransport = globalThis.__eaglerPeerTransport;
    if(this.startupTransport)this.startupTransport.onDisconnect=()=>this.connectionEnded();
    // Like Adonis2's receive thread, process calibration echoes on arrival.
    // The regular timer still schedules probes; gameplay keeps its own pump.
    if (!spectator && adonisMode && this.startupTransport)
      this.startupTransport.onReceive = () => { if (this.prepared && !this.active && this.connected) this.pump(); };
    this.onStatus('正在连接 TH09 联机对手…');
    this.pump();
  }
  pump() {
    if (!this.connected || this.disconnected || this.pumping) return;
    this.pumping=true;
    try {
      const state = this.core._th09_peer_state();
      if(state===3){this.connectionEnded();return;}
      if (state < 0) {
        const pointer = this.core._th09_peer_error();
        const error = this.core.HEAPU8.subarray(pointer, pointer + 256);
        const end = error.indexOf(0);
        throw Error(new TextDecoder().decode(error.subarray(0, end < 0 ? error.length : end)) || 'TH09 联机传输中断');
      }
      if(state===2&&!this.recovering){
        this.recovering=true;if(!this.finished)this.core._th09_loop_pause(1);this.onStatus('连接波动，正在尝试恢复…');
      }else if(state===1&&this.recovering){
        this.recovering=false;if(this.active||this.finished)this.core._th09_loop_pause(+document.hidden);
      }
      if (state === 1 && !this.routeReady) {
        this.routeReady = true;
        this.calibrationRoute = globalThis.__eaglerNetplayTransport;
        if (this.spectator) {
          const loadouts = this.options.netplayLoadouts;
          if (!this.core._th09_spectator_begin(this.options.netplaySeed >>> 0,
              Number(this.options.netplayDifficulty), loadouts[0].character, loadouts[1].character))
            throw Error('TH09 观战初始化失败');
          globalThis.__eaglerNetplayInputDelayFrames = 0;
          this.prepared = true;
          this.active = this.adonisMode === 0;
          this.spectatorReceiveAt = performance.now();
          globalThis.__eaglerNetplaySpectator = true;
          globalThis.__eaglerNetplayLanActive = this.active;
          this.onStatus(this.active ? '已连接 TH09 观战帧流' : '等待玩家完成实际游戏链路测量…');
          this.core._th09_loop_pause(+(document.hidden || !this.active));
        } else {
          const words = this.build.match(/.{8}/g).map(word => Number.parseInt(word, 16));
          const run = Number(new URL(this.options.netplayUrl, location.href).searchParams.get('run')) >>> 0;
          const [left, right] = this.options.netplayLoadouts.map(item => item.character);
          const difficulty = Number(this.options.netplayDifficulty);
          const inputDelay = this.inputDelay;
          const abi = (words[2] ^ (left << 16) ^ (right << 20) ^ (difficulty << 24) ^ 0x09010000) >>> 0;
          if (!this.core._th09_adonis_configure(this.adonisMode)) throw Error('TH09 联机时序模式无效');
          const begin = this.adonisMode ? this.core._th09_measured_begin : this.core._th09_rollback_begin;
          if (typeof begin !== 'function') throw Error('TH09 Runtime 不支持实际链路标定，请更新实验 Runtime');
          if (!begin(this.options.netplaySeed >>> 0, this.side, difficulty,
              left, right, (words[0] ^ run) >>> 0, words[1], abi,
              this.automatic ? 0xffffffff : inputDelay, this.predictionReserve))
            throw Error('TH09 rollback 对局初始化失败');
          if (!this.adonisMode) globalThis.__eaglerNetplayInputDelayFrames = inputDelay;
          globalThis.__eaglerNetplayAdonisMode = this.adonisMode;
          this.prepared = true;
          this.onStatus(this.adonisMode ? '正在实测实际游戏输入通道，完成后双方确认延迟…' : '已连接对手，正在确认版本和对局参数…');
          if (this.adonisMode) this.onTiming({phase:'measuring', automatic:this.automatic, adonisMode:this.adonisMode});
        }
      }
      if (!this.routeReady || !this.connected) return;
      if (!this.spectator) {
        globalThis.__eaglerNetplayCalibrationSuspended=!!document.hidden;
        if(this.adonisMode&&!this.active&&!this.finished&&this.calibrationRoute!==globalThis.__eaglerNetplayTransport){
          globalThis.__eaglerNetplayCalibrationSuspended=true;
          this.calibrationRoute=globalThis.__eaglerNetplayTransport;
        }
        const ready = this.core._th09_rollback_pump();
        if (!ready) {
          const p=this.core._th09_error(), bytes=this.core.HEAPU8, end=bytes.indexOf(0,p);
          throw Error(new TextDecoder().decode(bytes.subarray(p,end<0?p+256:end)) || 'TH09 rollback 会话中断');
        }
        if (this.adonisMode && !this.active && ready !== 2 && performance.now() >= this.startupNoticeAt) {
          this.startupNoticeAt = performance.now() + 100;
          const s=this.startupInfo();
          const phase=s[1]===7?'unavailable':document.hidden?'suspended':
            s[1]===6||(s[25]>1&&s[1]<=2&&!s[3])?'retrying':
            s[1]<2?'waiting':s[1]===2?(s[3]?'measuring':'stabilizing'):'negotiating';
          this.onTiming({phase,attempt:s[25],maxAttempts:4,reason:s[26],
            probes:s[3],replies:s[4],totalProbes:s[22],windowSamples:s[23],automatic:this.automatic,
            adonisMode:this.adonisMode,route:this.calibrationRoute,players:this.startupPlayers(s)});
          if(s[1]===7){this.disconnected=true;this.core._th09_loop_pause(1);return;}
        }
        if (ready === 2 && !this.active && !this.finished) {
          if (this.adonisMode) {
            const s=this.startupInfo();
            if(s[1]!==4 || s[10]>9 || s[13]!==this.adonisMode)throw Error('TH09 实测协商尚未完成');
            this.inputDelay=s[10];
            this.acceptTiming({phase:'ready',automatic:this.automatic,adonisMode:this.adonisMode,
              inputDelay:s[10],fullDelay:s[9],predictionReserve:s[11],rttP95Us:Math.max(s[5],s[6]),
              samples:Math.min(s[14],s[15]),lost:s[7]+s[8],route:this.calibrationRoute,
              calibration:{localPlayer:this.side,completedAt:new Date().toISOString(),
                build:Array.from(this.buildBytes,b=>b.toString(16).padStart(2,'0')).join(''),
                method:'adonis2-129-probes-16ms-tail200ms',stabilizeMs:1000,intervalMs:16,tailWaitMs:200,
                probes:129,windowStart:10,windowEnd:129,attempts:s[25],players:this.startupPlayers(s)}});
            if(this.side===0 && Number(this.options.netplaySpectatorCount)>0)this.spectatorPending.push(this.timingPacket());
          }
          this.acknowledged = this.active = true;
          globalThis.__eaglerNetplayCalibrationSuspended=false;
          if(this.startupTransport)this.startupTransport.onReceive=null;
          globalThis.__eaglerNetplayLanActive = true;
          this.onStatus(`对手已连接 · ${['Rollback', 'Adonis 无回滚', 'Adonis + Rollback'][this.adonisMode]} · D=${this.inputDelay}`);
          this.core._th09_loop_pause(+document.hidden);
        }
      } else for (let count = 0; count < 128; ++count) {
        const length = this.core._th09_peer_poll();
        if (length < 0) throw Error('TH09 联机数据过大');
        if (!length) break;
        const pointer = this.core._th09_peer_packet_buffer();
        this.receive(this.core.HEAPU8.slice(pointer, pointer + length));
        if (!this.connected) return;
      }
      // No silent frozen viewer if the host/upload disappears without a close
      // reaching this socket. This affects the viewer only, never the players.
      if (this.spectator && this.prepared && performance.now() - this.spectatorReceiveAt >= 15000)
        throw Error('TH09 观战确认帧流中断，玩家对局不受影响');
      if (!this.spectator) this.flushSpectators();
    } catch (error) { this.close(String(error?.message || error)); }
    finally {
      this.pumping=false;
      clearTimeout(this.timer);
      if (this.connected && !this.disconnected) {
        let delay=16;
        if(this.adonisMode && this.prepared && !this.active && !this.spectator) {
          const at=this.core._th09_startup_info()/4;
          // Native owner supplies its remaining deadline. An early browser
          // timer retries the remainder instead of losing another 16 ms.
          delay=Math.max(1,Math.ceil(this.core.HEAPU32[at+24]/1000));
        }
        this.timer=setTimeout(()=>this.pump(),delay);
      }
    }
  }
  startupInfo() {
    const at=this.core._th09_startup_info()/4;
    const s=Array.from(this.core.HEAPU32.subarray(at,at+27));
    if(s[0]!==3)throw Error('TH09 Runtime 缺少测量恢复接口，请更新 Runtime');
    return s;
  }
  connectionEnded() {
    if(!this.connected||this.disconnected)return;
    this.disconnected=true;this.recovering=false;
    clearTimeout(this.timer);this.timer=null;
    globalThis.__eaglerNetplayCalibrationSuspended=false;
    globalThis.__eaglerNetplayLanActive=false;
    if(!this.finished)this.core._th09_loop_pause(1);
    if(!this.active)this.onTiming({phase:'unavailable',reason:4,attempt:1,maxAttempts:4});
    this.onStatus('联机连接已断开，请返回房间重新开始。');
  }
  startupPlayers(s) {
    return [{player:this.side,p95Us:s[5],samples:s[14],lost:s[7],minUs:s[16],maxUs:s[17],meanUs:s[18]},
      {player:1-this.side,p95Us:s[6],samples:s[15],lost:s[8],minUs:s[19],maxUs:s[20],meanUs:s[21]}]
      .sort((a,b)=>a.player-b.player);
  }
  receive(bytes) {
    try {
        if (this.spectator) {
          if (bytes.length===40 && bytes[0]===84 && bytes[1]===57 && bytes[2]===84 && bytes[3]===77) {
            const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
            const d=bytes[6],reserve=bytes[7],full=v.getUint32(8,true),automatic=v.getUint32(32,true);
            if(bytes[4]!==1 || bytes[5]!==this.adonisMode || !this.adonisMode || d>9 || reserve>2 ||
               (this.adonisMode===1&&reserve!==0) || (this.adonisMode===2&&reserve!==Math.min(Math.max(0,full-(this.automatic?1:0)),this.predictionReserve)) ||
               full<1 || full>31 || v.getUint32(24,true)>1e6 || v.getUint32(28,true)>48 ||
               automatic>1 || !!automatic!==this.automatic || v.getUint32(36,true)<96 || v.getUint32(36,true)>120 ||
               this.buildBytes.some((b,i)=>b!==bytes[12+i]) ||
               (automatic?d!==Math.max(0,full-reserve):d!==this.inputDelay))throw Error('TH09 观战时序协商元数据无效');
            const timing={phase:'ready',automatic:!!automatic,adonisMode:bytes[5],inputDelay:d,fullDelay:full,
              predictionReserve:reserve,rttP95Us:v.getUint32(24,true),samples:v.getUint32(36,true),lost:v.getUint32(28,true),route:'spectator'};
            if(this.timing && JSON.stringify(this.timing)!==JSON.stringify(timing))throw Error('TH09 观战时序在局中改变');
            this.inputDelay=d;this.acceptTiming(timing);this.active=true;
            this.spectatorReceiveAt=performance.now();
            globalThis.__eaglerNetplayInputDelayFrames=0;globalThis.__eaglerNetplayLanActive=true;
            this.onStatus(`已连接 TH09 观战帧流 · 玩家 D=${d}`);this.core._th09_loop_pause(+document.hidden);return;
          }
          if(this.adonisMode&&!this.timing)throw Error('TH09 观战缺少已确认的时序元数据');
          if (bytes.length !== spectatorFrameBytes ||
              spectatorMagic.some((value, index) => bytes[index] !== value)) throw Error('TH09 观战帧格式错误');
          const data = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
          if (this.buildBytes.some((value, index) => value !== bytes[12 + index]))
            throw Error('TH09 观战 Runtime 版本不同');
          const frame = data.getUint32(8, true);
          if (frame !== this.spectatorFrame) throw Error('TH09 观战帧序号不连续');
          const side = at => [data.getUint16(at, true), bytes[at + 2],
            data.getFloat32(at + 3, true), data.getFloat32(at + 7, true)];
          const [left, leftMode, leftX, leftY] = side(24);
          const [right, rightMode, rightX, rightY] = side(35);
          if (!this.core._th09_spectator_feed(frame, left, right,
              leftMode, leftX, leftY, rightMode, rightX, rightY))
            throw Error('TH09 观战输入无效或过于滞后');
          ++this.spectatorFrame;
          this.spectatorReceiveAt=performance.now();
          return;
        }
        throw Error('TH09 gameplay input belongs to the C++ session');
    } catch (error) { this.close(String(error?.message || error)); }
  }
  input() {} // Captured once by C++; the legacy JS callback is intentionally idle.
  acceptTiming(timing) {
    this.timing=timing;
    globalThis.__eaglerNetplayTiming={...timing};
    globalThis.__eaglerNetplayInputDelayFrames=timing.inputDelay;
    this.onTiming({...timing});
  }
  timingPacket() {
    const t=this.timing,b=new Uint8Array(40),v=new DataView(b.buffer);
    b.set([84,57,84,77,1,t.adonisMode,t.inputDelay,t.predictionReserve]);
    v.setUint32(8,t.fullDelay,true);b.set(this.buildBytes,12);
    v.setUint32(24,t.rttP95Us,true);v.setUint32(28,t.lost,true);
    v.setUint32(32,+t.automatic,true);v.setUint32(36,t.samples,true);return b;
  }
  publish(frame, left, right, leftMode, leftX, leftY, rightMode, rightX, rightY) {
    if (this.spectator || !this.connected || this.spectatorOutputStopped || this.side !== 0 ||
        Number(this.options.netplaySpectatorCount) < 1) return;
    const bytes = new Uint8Array(spectatorFrameBytes), data = new DataView(bytes.buffer);
    bytes.set(spectatorMagic); data.setUint32(8, frame >>> 0, true);
    bytes.set(this.buildBytes, 12);
    const side = (at, keys, mode, x, y) => {
      data.setUint16(at, keys & 65535, true); bytes[at + 2] = mode;
      data.setFloat32(at + 3, x, true); data.setFloat32(at + 7, y, true);
    };
    side(24, left, leftMode, leftX, leftY); side(35, right, rightMode, rightX, rightY);
    if (this.spectatorPending.length - this.spectatorHead >= 8192)
      return this.stopSpectators('TH09 观战积压超限，已停止观战；玩家对局继续');
    this.spectatorPending.push(bytes);
    // Publishing may run many times in a single reconciliation callback.
    // Drain only from the transport pump, never once per published frame.
  }
  stopSpectators(reason) {
    if (this.spectatorOutputStopped || this.spectator || this.side !== 0) return;
    this.spectatorOutputStopped = true;
    this.spectatorPending.length = 0;
    this.spectatorHead = 0;
    this.core._th09_peer_stop_spectators();
    this.onStatus(reason);
  }
  flushSpectators() {
    if (!this.connected || this.spectator || this.spectatorOutputStopped || this.side !== 0 ||
        Number(this.options.netplaySpectatorCount) < 1) return;
    if (this.core._th09_peer_spectator_state() < 0)
      return this.stopSpectators('TH09 观战上传连接中断，已停止观战；玩家对局继续');
    const began = performance.now();
    for (let sent = 0; this.spectatorHead < this.spectatorPending.length && sent < 32; ++sent) {
      if (sent && performance.now() - began >= 2) break;
      const bytes = this.spectatorPending[this.spectatorHead];
      const at = this.core._th09_peer_packet_buffer();
      this.core.HEAPU8.set(bytes, at);
      if (!this.core._th09_peer_send_spectator(bytes.length)) break;
      if (bytes.length === spectatorFrameBytes)
        this.spectatorSentFrame = new DataView(bytes.buffer, bytes.byteOffset).getUint32(8, true) + 1;
      this.spectatorPending[this.spectatorHead++] = null;
    }
    if (this.spectatorHead === this.spectatorPending.length) {
      this.spectatorPending.length = 0; this.spectatorHead = 0;
    } else if (this.spectatorHead >= 4096) {
      this.spectatorPending = this.spectatorPending.slice(this.spectatorHead); this.spectatorHead = 0;
    }
  }
  frame() {
    if (!this.active) return;
    const frame = this.spectator ? this.core._th09_spectator_frame() : this.info()[3];
    globalThis.__eaglerNetplayLanFrame = frame;
    const at = this.core._th09_rollback_info() / 4;
    const stats = this.core.HEAPU32.subarray(at, at + 8);
    globalThis.__eaglerNetplayLanConfirmed = this.spectator ? frame : stats[1];
    globalThis.__eaglerNetplayLanRollback = this.spectator ? 0 : stats[2];
    globalThis.__eaglerNetplayLanResimulated = this.spectator ? 0 : stats[3];
  }
  result() {
    if (!this.connected || this.finished) return;
    // The native TH09 versus result transitions into its replay-name/slot
    // screens. Keep the transport alive until both peers finish that local UI;
    // closing it here could abort the other peer before its result callback.
    this.finished = true;
    this.active = false;
    // Keep servicing the terminal input ACK while the other peer exits.
    globalThis.__eaglerNetplayLanActive = false;
    this.onResult();
  }
  close(reason = '') {
    if (!this.connected) return;
    this.connected = false;
    clearTimeout(this.timer);
    if(this.startupTransport)this.startupTransport.onReceive=null;
    if(this.startupTransport)this.startupTransport.onDisconnect=null;
    globalThis.__eaglerNetplayCalibrationSuspended=false;
    this.onTiming({phase:'closed'});
    this.timer = null;
    this.routeReady = false;
    this.active = false;
    globalThis.__eaglerNetplayLanActive = false;
    globalThis.__eaglerNetplaySpectator = false;
    this.core._th09_peer_close();
    this.spectatorPending.length = 0;
    this.spectatorHead = 0;
    if (this.prepared) {
      if (this.spectator) this.core._th09_spectator_end();
      else this.core._th09_network_end();
    }
    this.prepared = false;
    if (reason) this.onStatus(reason);
    this.onClose(reason);
  }
}
