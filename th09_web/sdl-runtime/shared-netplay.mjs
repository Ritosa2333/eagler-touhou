// TH09's ordered input protocol runs over eagler-common's BrowserPeerTransport.
// The Launcher owns rooms, seats and loadouts; the common transport chooses
// WebRTC or relay and keeps both entry points on the same gameplay route.
const magic = [0x54, 0x39, 0x4e, 0x50, 1]; // T9NP/1
const hello = 1, acknowledged = 2, input = 3, hash = 4;
const spectatorMagic = [0x54, 0x39, 0x53, 0x50, 1, 3, 2, 0]; // T9SP/1, 2 players
const spectatorFrameBytes = 46;
const header = type => Uint8Array.from([...magic, type]);
const valid = (bytes, type, length) => bytes.length === length &&
  magic.every((value, index) => bytes[index] === value) && bytes[5] === type;

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
    this.hashFrame = 0;
    this.hashes = new Map();
    this.remoteHashes = new Map();
  }
  info() {
    const at = this.core._th09_network_info() / 4;
    return Array.from(this.core.HEAPU32.subarray(at, at + 6));
  }
  send(bytes) {
    if (!this.connected) return;
    const pointer = this.core._th09_peer_packet_buffer();
    this.core.HEAPU8.set(bytes, pointer);
    if (!this.core._th09_peer_send(bytes.length)) this.close('TH09 联机发送失败');
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
    this.hashes.clear(); this.remoteHashes.clear();
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
    if (!this.connected || this.finished) return;
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
          this.onStatus('已连接对手，正在确认 TH09 版本…');
          this.send(Uint8Array.from([...header(hello), ...new TextEncoder().encode(this.build)]));
        }
      }
      if (!this.routeReady || !this.connected) return;
      for (let count = 0; count < 128; ++count) {
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
        if (valid(bytes, hello, 30)) {
          const remoteBuild = new TextDecoder().decode(bytes.subarray(6));
          if (remoteBuild !== this.build) throw Error('双方 TH09 Runtime 版本不同');
          if (!this.prepared) {
            // Lobby loadouts, not either player's save file, determine the
            // shared simulation. Local unlocks/auto-focus may differ.
            if (!this.core._th09_network_room_begin(
              this.options.netplaySeed >>> 0, this.side, 0xffff, Number(this.options.netplayDifficulty), 0,
              this.options.netplayLoadouts[0].character, this.options.netplayLoadouts[1].character,
            )) throw Error('TH09 联机对局初始化失败');
            this.prepared = true;
          }
          this.send(header(acknowledged));
        } else if (valid(bytes, acknowledged, 6)) {
          if (!this.prepared) throw Error('TH09 联机握手顺序错误');
          this.acknowledged = this.active = true;
          globalThis.__eaglerNetplayLanActive = true;
          this.onStatus('对手已连接 · 对局开始');
          this.core._th09_loop_pause(+document.hidden);
        } else if (valid(bytes, input, 21)) {
          if (!this.prepared) throw Error('对手在握手前发送输入');
          const data = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
          if (!this.core._th09_network_receive(data.getUint32(6, true), data.getUint16(10, true),
              bytes[12], data.getFloat32(13, true), data.getFloat32(17, true)))
            throw Error('TH09 联机输入序号错误');
        } else if (valid(bytes, hash, 14)) {
          const data = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
          const frame = data.getUint32(6, true), remote = data.getUint32(10, true);
          this.remoteHashes.set(frame, remote);
          this.compareHash(frame);
        } else throw Error('TH09 联机数据格式错误');
    } catch (error) { this.close(String(error?.message || error)); }
  }
  input(frame, keys, moving = 0, x = 0, y = 0) {
    if (!this.active) return;
    // `moving` is the motion mode: 0 none, 1 pre-scaled velocity, 2 absolute
    // field target limited to the player's speed, 3 the same without the limit.
    // Targets travel absolute so each peer converts them on the frame it
    // simulates instead of aiming from a position the lockstep delay made stale.
    if (!Number.isInteger(moving) || moving < 0 || moving > 3 ||
        !Number.isFinite(x) || !Number.isFinite(y)) return this.close('TH09 联机输入无效');
    const bytes = header(input), payload = new Uint8Array(21), data = new DataView(payload.buffer);
    payload.set(bytes); data.setUint32(6, frame >>> 0, true); data.setUint16(10, keys & 65535, true);
    payload[12] = moving; data.setFloat32(13, x, true); data.setFloat32(17, y, true);
    this.send(payload);
  }
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
    globalThis.__eaglerNetplayLanConfirmed = frame;
    globalThis.__eaglerNetplayLanRollback = 0;
    globalThis.__eaglerNetplayLanResimulated = 0;
    if (!this.spectator && frame && frame % 120 === 0 && frame !== this.hashFrame) {
      this.hashFrame = frame;
      const value = this.core._th09_network_hash() >>> 0;
      this.hashes.set(frame, value);
      const payload = new Uint8Array(14), data = new DataView(payload.buffer);
      payload.set(header(hash)); data.setUint32(6, frame, true); data.setUint32(10, value, true);
      this.send(payload); this.compareHash(frame);
      if (this.hashes.size > 8) this.hashes.delete(this.hashes.keys().next().value);
    }
  }
  compareHash(frame) {
    if (!this.hashes.has(frame) || !this.remoteHashes.has(frame)) return;
    if (this.hashes.get(frame) !== this.remoteHashes.get(frame)) this.close('TH09 联机状态不同步');
    else this.remoteHashes.delete(frame);
  }
  result() {
    if (!this.connected || this.finished) return;
    // The native TH09 versus result transitions into its replay-name/slot
    // screens. Keep the transport alive until both peers finish that local UI;
    // closing it here could abort the other peer before its result callback.
    this.finished = true;
    this.active = false;
    clearInterval(this.timer);
    this.timer = null;
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
