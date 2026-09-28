// TH09 uses the shared C++ rollback session and BrowserPeerTransport lanes.
const spectatorMagic = [0x54, 0x39, 0x53, 0x50, 1, 3, 2, 0]; // T9SP/1, 2 players
const spectatorFrameBytes = 46;
export class SharedNetplay {
  constructor(core, {onStatus, onClose, onResult}) {
    this.core = core;
    this.onStatus = onStatus;
    this.onClose = onClose;
    this.onResult = onResult;
    this.connected = false;
    this.timer = null;
    this.active = false;
    this.finished = false;
    this.spectator = false;
    this.spectatorFrame = 0;
    this.spectatorPending = [];
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
    const url = new URL(options.netplayUrl, location.href);
    const side = Number(options.netplayPlayer);
    const spectator = options.netplaySpectator === true;
    const spectatorId = String(options.netplaySpectatorId || '');
    if (!['ws:', 'wss:'].includes(url.protocol) || (!spectator && ![0, 1].includes(side)) ||
        Number(options.netplayPlayerCount) !== 2 || !/^th09mp-\d{4}$/.test(url.searchParams.get('room') || '') ||
        !/^\d+$/.test(url.searchParams.get('run') || '') ||
        (spectator ? !/^[A-Za-z0-9_-]{8,64}$/.test(spectatorId) ||
          url.searchParams.get('spectator') !== spectatorId || url.searchParams.has('player') :
          Number(url.searchParams.get('player')) !== side || url.searchParams.has('spectator')) ||
        (url.searchParams.has('players') && url.searchParams.get('players') !== '2')) throw Error('TH09 房间配置无效');
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
    this.finished = false;
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
    this.onStatus('正在连接 TH09 联机对手…');
    this.timer = setInterval(() => this.pump(), 16);
    this.pump();
  }
  pump() {
    if (!this.connected) return;
    try {
      const state = this.core._th09_peer_state();
      if (state < 0) {
        const pointer = this.core._th09_peer_error();
        const error = this.core.HEAPU8.subarray(pointer, pointer + 256);
        const end = error.indexOf(0);
        throw Error(new TextDecoder().decode(error.subarray(0, end < 0 ? error.length : end)) || 'TH09 联机传输中断');
      }
      if (state === 1 && !this.routeReady) {
        this.routeReady = true;
        if (this.spectator) {
          const loadouts = this.options.netplayLoadouts;
          if (!this.core._th09_spectator_begin(this.options.netplaySeed >>> 0,
              Number(this.options.netplayDifficulty), loadouts[0].character, loadouts[1].character))
            throw Error('TH09 观战初始化失败');
          this.prepared = this.active = true;
          globalThis.__eaglerNetplaySpectator = true;
          globalThis.__eaglerNetplayLanActive = true;
          this.onStatus('已连接 TH09 观战帧流');
          this.core._th09_loop_pause(+document.hidden);
        } else {
          const words = this.build.match(/.{8}/g).map(word => Number.parseInt(word, 16));
          const run = Number(new URL(this.options.netplayUrl, location.href).searchParams.get('run')) >>> 0;
          const [left, right] = this.options.netplayLoadouts.map(item => item.character);
          const difficulty = Number(this.options.netplayDifficulty);
          if (!this.core._th09_rollback_begin(this.options.netplaySeed >>> 0, this.side, difficulty,
              left, right, (words[0] ^ run) >>> 0, words[1],
              (words[2] ^ (left << 16) ^ (right << 20) ^ (difficulty << 24) ^ 0x09010000) >>> 0))
            throw Error('TH09 rollback 对局初始化失败');
          this.prepared = true;
          this.onStatus('已连接对手，正在确认版本和对局参数…');
        }
      }
      if (!this.routeReady || !this.connected) return;
      if (!this.spectator) {
        const ready = this.core._th09_rollback_pump();
        if (!ready) throw Error('TH09 rollback 会话中断');
        if (ready === 2 && !this.active && !this.finished) {
          this.acknowledged = this.active = true;
          globalThis.__eaglerNetplayLanActive = true;
          this.onStatus('对手已连接 · rollback 对局开始');
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
      if (!this.spectator) this.flushSpectators();
    } catch (error) { this.close(String(error?.message || error)); }
  }
  receive(bytes) {
    try {
        if (this.spectator) {
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
          return;
        }
        throw Error('TH09 gameplay input belongs to the C++ session');
    } catch (error) { this.close(String(error?.message || error)); }
  }
  input() {} // Captured once by C++; the legacy JS callback is intentionally idle.
  publish(frame, left, right, leftMode, leftX, leftY, rightMode, rightX, rightY) {
    if (this.spectator || !this.connected || this.side !== 0 ||
        Number(this.options.netplaySpectatorCount) < 1) return;
    const bytes = new Uint8Array(spectatorFrameBytes), data = new DataView(bytes.buffer);
    bytes.set(spectatorMagic); data.setUint32(8, frame >>> 0, true);
    bytes.set(this.buildBytes, 12);
    const side = (at, keys, mode, x, y) => {
      data.setUint16(at, keys & 65535, true); bytes[at + 2] = mode;
      data.setFloat32(at + 3, x, true); data.setFloat32(at + 7, y, true);
    };
    side(24, left, leftMode, leftX, leftY); side(35, right, rightMode, rightX, rightY);
    if (this.spectatorPending.length >= 8192) return this.close('TH09 观战帧积压过多');
    this.spectatorPending.push(bytes);
    this.flushSpectators();
  }
  flushSpectators() {
    while (this.connected && this.spectatorPending.length) {
      const bytes = this.spectatorPending[0];
      const at = this.core._th09_peer_packet_buffer();
      this.core.HEAPU8.set(bytes, at);
      if (!this.core._th09_peer_send_spectator(bytes.length)) break;
      this.spectatorPending.shift();
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
    clearInterval(this.timer);
    this.timer = null;
    this.routeReady = false;
    this.active = false;
    globalThis.__eaglerNetplayLanActive = false;
    globalThis.__eaglerNetplaySpectator = false;
    this.core._th09_peer_close();
    if (this.prepared) {
      if (this.spectator) this.core._th09_spectator_end();
      else this.core._th09_network_end();
    }
    this.prepared = false;
    if (reason) this.onStatus(reason);
    this.onClose(reason);
  }
}
