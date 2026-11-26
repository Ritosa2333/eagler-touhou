import createWire from './room-wire.mjs';

import {editThibpScript} from './eagler-host.mjs';
const titleAnchor = 'Draw_Corsor;';
const titleLoop = 'case( STATE_TITLE ){';
const roomStart = [
  'let roomDifficulty = TakeThibpRoomDifficulty;',
  'if (roomDifficulty >= 0) {',
  '  SetCommonData(DC_COUNT, 0);',
  '  SetCommonData(DC_REPLAY_FILE, "");',
  '  let roomStage = 1;',
  '  if (roomDifficulty == 4) { roomStage = 7; }',
  '  SetAreaCommonData(COMMONAREA_SCENE, SC_STAGEINDEX, roomStage);',
  '  SetAreaCommonData(COMMONAREA_SCENE, SC_GAMEMODE, 0);',
  '  SetAreaCommonData(COMMONAREA_SCENE, SC_STAGEPHASE, 0);',
  '  SetAreaCommonData(COMMONAREA_SCENE, SC_DIFFICULTY, roomDifficulty);',
  '  SetAreaCommonData(COMMONAREA_SCENE, SC_PLAYER_TYPE, GetThibpRoomPlayerType);',
  '  state_active = STATE_GAMEPLAY;',
  '  command_active = 0;',
  '  Thread_StageManager;',
  '}',
].join('\r\n');

const roomResult = [
  'let thibpMenuChoice = GetScriptResult( obj );',
  'if (IsThibpCooperative && thibpMenuChoice == RESULT_RETRY) {',
  '  let roomStage = 1;',
  '  if (GetAreaCommonData(COMMONAREA_SCENE, SC_DIFFICULTY, 0) == 4) { roomStage = 7; }',
  '  SetAreaCommonData(COMMONAREA_SCENE, SC_STAGEINDEX, roomStage);',
  '  SetAreaCommonData(COMMONAREA_SCENE, SC_CONTINUE, 0);',
  '  state_active = STATE_GAMEPLAY;',
  '  Thread_StageManager;',
  '  return;',
  '}',
  'FinishThibpRoom;',
  'alternative( thibpMenuChoice )',
].join('\r\n');

const roomStockHud = `
task Draw_ThibpRoomStocks {
  let seats = GetThibpSeatCount;
  let top = 98;
  let spacing = 33;
  if (seats == 2) { top = 106; spacing = 40; }
  ascent(seat in 0..seats) {
    Draw_ThibpSeatStock(seat, top + seat * spacing);
  }
}
task Draw_ThibpSeatStock(let seat, let y) {
  let stockPlate = ObjPrim_Create(OBJ_PRIMITIVE_2D);
  Obj_SetRenderPriorityI(stockPlate, 82);
  ObjRender_SetBlendType(stockPlate, BLEND_ALPHA);
  ObjPrim_SetPrimitiveType(stockPlate, PRIMITIVE_TRIANGLESTRIP);
  ObjPrim_SetVertexCount(stockPlate, 4);
  ascent(vertex in 0..4) {
    ObjPrim_SetVertexPosition(stockPlate, vertex, 427 + 210 * (vertex % 2), y - 10 + 32 * floor(vertex / 2), 0);
    ObjPrim_SetVertexColor(stockPlate, vertex, 18, 12, 26);
    ObjPrim_SetVertexAlpha(stockPlate, vertex, 192);
  }
  let stockLabel = ObjText_Create;
  Obj_SetRenderPriorityI(stockLabel, 83);
  ObjText_SetFontSize(stockLabel, 16);
  ObjText_SetFontBold(stockLabel, true);
  ObjText_SetFontBorderType(stockLabel, BORDER_FULL);
  ObjText_SetFontBorderWidth(stockLabel, 1);
  ObjText_SetFontBorderColor(stockLabel, 0, 0, 0);
  ObjText_SetText(stockLabel, "P" ~ IntToString(seat + 1));
  ObjRender_SetPosition(stockLabel, 431, y - 5, 0);

  let stockSprite = ObjPrim_Create(OBJ_SPRITE_LIST_2D);
  Obj_SetRenderPriorityI(stockSprite, 83);
  ObjPrim_SetTexture(stockSprite, TEX_STAGE_FILEDIR[TEX_STAGE_UI]);
  let previousStock = [-1, -1, -1, -1, -1];
  while (!IsSceneEnd) {
    let stock = GetThibpSeatStock(seat);
    if (stock[0] != previousStock[0] || stock[1] != previousStock[1] ||
        stock[2] != previousStock[2] || stock[3] != previousStock[3] ||
        stock[4] != previousStock[4]) {
      ObjSpriteList2D_ClearVertexCount(stockSprite);
      let alpha = 255 - 55 * stock[4];
      ObjRender_SetAlpha(stockSprite, alpha);
      ObjRender_SetAlpha(stockLabel, alpha);
      if (stock[5] == 0) {
        DrawStockLane(461, 64, stock[0], stock[2], DF_LIFE_FRAGMENT_MAX);
        DrawStockLane(552, 80, stock[1], stock[3], DF_BOMB_FRAGMENT_MAX);
      } else {
        AddStockSprite(384, 32, 32, 32, 469, y + 4, 0.64);
        let deaths = stock[0];
        let place = 1;
        while (10 * place <= deaths) { place *= 10; }
        let digitX = 485;
        while (place >= 1) {
          DrawStockDigit(truncate(deaths / place) % 10, digitX, y + 4);
          digitX += 11;
          place /= 10;
        }
      }
      previousStock = stock;
    }
    let visible = !GetAreaCommonData(COMMONAREA_SCENE, SC_BORDERBREAK, 0);
    Obj_SetVisible(stockPlate, visible);
    Obj_SetVisible(stockLabel, visible);
    Obj_SetVisible(stockSprite, visible);
    yield;
  }
  Obj_Delete(stockPlate);
  Obj_Delete(stockLabel);
  Obj_Delete(stockSprite);

  function DrawStockLane(let x, let spriteY, let full, let fragments, let limit) {
    let partial = round(5 * fragments / limit);
    ascent(slot in 0..8) {
      let spriteX = 368;
      if (slot < full) { spriteX = 352; }
      else if (slot == full) {
        spriteX += 16 * partial;
        if (partial >= 5) { spriteX = 352; }
      }
      AddStockSprite(spriteX, spriteY, 16, 16, x + 11 * slot, y, 0.875);
    }
    AddStockSprite(640, 32, 64, 32, x + 20, y + 13, 0.70);
    DrawStockDigit(fragments, x + 45, y + 13);
    DrawStockDigit(10, x + 56, y + 13);
    DrawStockDigit(limit, x + 67, y + 13);
  }
  function DrawStockDigit(let number, let x, let digitY) {
    if (number == 10) { AddStockSprite(512, 32, 32, 32, x, digitY, 0.62); }
    else { AddStockSprite(352 + 32 * number, 0, 32, 32, x, digitY, 0.62); }
  }
  function AddStockSprite(let u, let v, let width, let height, let x, let destY, let scale) {
    ObjSpriteList2D_SetSourceRect(stockSprite, u, v, u + width, v + height);
    ObjRender_SetPosition(stockSprite, x, destY, 0);
    ObjRender_SetScaleXYZ(stockSprite, scale, scale, 1);
    ObjSpriteList2D_SetDestCenter(stockSprite);
    ObjSpriteList2D_AddVertex(stockSprite);
  }
}`;

const roomTransferLabel = `
task Draw_ThibpTransferLabel {
  let label = ObjText_Create;
  ObjText_SetFontSize(label, 13);
  Obj_SetRenderPriorityI(label, 50);
  while (!Obj_IsDeleted(this)) {
    let count = GetThibpTransferCount;
    Obj_SetVisible(label, count >= 3);
    if (count >= 3) {
      ObjText_SetText(label, "L " ~ IntToString(count) ~ "/5");
      ObjRender_SetPosition(label, GetPlayerX - 24, GetPlayerY - 25, 0);
    }
    yield;
  }
  Obj_Delete(label);
}`;

export function prepareRoomScripts(fs) {
  const edit = editThibpScript.bind(null, fs);
  edit('scene/Scene_TitleMain.dnh', titleAnchor, titleAnchor + '\r\nMarkThibpRoomTitle;');
  edit('scene/Scene_TitleMain.dnh', titleLoop, titleLoop + '\r\n' + roomStart + '\r\n');
  edit('player/Player_Function.dnh', 'if( GetPlayerState != STATE_NORMAL || !flg_p_visible ){',
    'ObjRender_SetAlpha(obj, 255);\r\n' +
    'if (IsThibpGhost) { Obj_SetVisible(obj, true); ObjRender_SetAlpha(obj, 112); }\r\n' +
    'else if( GetPlayerState != STATE_NORMAL || !flg_p_visible ){');
  edit('scene/Scene_Stage.dnh', 'let pl = GetAreaCommonData( COMMONAREA_SAVE, SV_OP_PLAYER, 2 );',
    'let pl = GetAreaCommonData( COMMONAREA_SAVE, SV_OP_PLAYER, 2 );\r\n' +
    'if (IsThibpCooperative) { pl = 2; }');
  // Keep the native result menu on confirmed inputs until a player leaves the run.
  edit('scene/Scene_Stage.dnh', 'alternative( GetScriptResult( obj ) )', roomResult);
  edit('scene/Scene_Stage.dnh', 'case( STAGE_RESULT_BREAK_OFF ){',
    'case( STAGE_RESULT_BREAK_OFF ){ FinishThibpRoom;');
  edit('system/Sys_EndScene.dnh', 'if( GetAreaCommonData( COMMONAREA_SAVE, SV_OP_PLAYER, 2 ) != 2 )',
    'if( !IsThibpCooperative && GetAreaCommonData( COMMONAREA_SAVE, SV_OP_PLAYER, 2 ) != 2 )');
  const skillEnds = {ReimuA: 2, ReimuB: 1, MarisaA: 1, MarisaB: 1, YoumuA: 3, YoumuB: 2};
  for (const [character, endings] of Object.entries(skillEnds)) {
    const path = `player/Player_${character}_Shot.dnh`;
    edit(path, 'task CallSpecialAttack{', 'task CallSpecialAttack{ if (!TryThibpSpecial) { return; }');
    edit(path, 'flg_special = 0;', 'flg_special = 0; ReleaseThibpSpecial;', endings);
  }
  edit('system/Sys_Pause.dnh', 'if( GetVirtualKeyState( VK_OK ) == KEY_PUSH ){',
    'if (IsThibpRetryPressed) { command_active = COMMAND_MENU_RETRY; }\r\n' +
    'if( IsThibpRetryPressed || GetVirtualKeyState( VK_OK ) == KEY_PUSH ){');
  edit('system/Sys_ItemEvent.dnh', 'let getitem = GetEventArgument( 0 );',
    'let getitem = GetEventArgument( 0 );\r\n' +
    'if (RegisterThibpResourceItem(getitem, DF_LIFE_FRAGMENT_MAX, DF_BOMB_FRAGMENT_MAX)) {\r\n' +
    '  ObjPlaySE(SE_OBJ[SE_ITEM_SP]); NotifyEventAll(EV_TENSION_RISE, 800); return;\r\n' +
    '}');
  edit('system/Sys_ItemEvent.dnh',
    'let dis = GetDistance( GetPlayerX, GetPlayerY, ObjMove_GetX( obj ), ObjMove_GetY( obj ) );',
    'let dis = GetThibpItemDistance(obj);');
  edit('system/Sys_ItemEvent.dnh', 'if( GetPlayerState != STATE_NORMAL && state == 1 ){',
    'if (dis < 0 && state == 1) {');
  edit('system/Sys_ItemEvent.dnh', 'if( count > 45 && dis < c_range && GetPlayerState == STATE_NORMAL ){',
    'if (count > 45 && dis >= 0 && dis < c_range) {');
  edit('stage/Stage_UI_Main.dnh', 'Draw_PlayerStock;',
    'if (GetThibpSeatCount > 1) { Draw_ThibpRoomStocks; } else { Draw_PlayerStock; }');
  edit('stage/Stage_UI_Main.dnh', 'Draw_BombStock;', 'if (GetThibpSeatCount < 2) { Draw_BombStock; }');
  for (const line of ['Draw_Line( 440, 124, 192, 6 );', 'Draw_Line( 440, 154, 192, 3 );']) {
    edit('stage/Stage_UI_Main.dnh', line, `if (GetThibpSeatCount < 2) { ${line} }`);
  }
  edit('stage/Stage_UI_Main.dnh', 'task Draw_Difficulty{', roomStockHud + '\r\n' + 'task Draw_Difficulty{');
  edit('player/Player_Function.dnh', 'task Draw_PlayerMain( let tex_dir ){',
    roomTransferLabel + '\r\n' + 'task Draw_PlayerMain( let tex_dir ){ Draw_ThibpTransferLabel;');
}

const validInteger = (value, lo, hi) => Number.isInteger(value) && value >= lo && value <= hi;
const memberId = /^[A-Za-z0-9_-]{8,64}$/;

export function readRoomOptions(options) {
  if (options?.netplayMode === undefined) return null;
  const {netplayPlayer: seat, netplayPlayerCount: players, netplaySeed: seed, netplayDifficulty: difficulty,
    netplayLoadouts: loadouts, netplayInputDelay: delay = 0, netplayChallengeMode: challenge = false} = options;
  const mode = options.netplayAdonisMode ?? 0;
  const predictionLimit = mode === 0 ? (options.netplayPredictionLimit ?? 4) : 0;
  const spectator = options.netplaySpectator === true ? options.netplaySpectatorId : '';
  if (options.netplayMode !== 'lan' || (options.netplaySpectator === true && !memberId.test(spectator ?? '')) ||
      ![0, 1].includes(mode) || !validInteger(predictionLimit, mode === 0 ? 1 : 0, 4) ||
      (mode === 0 && delay !== 0) || !validInteger(players, 2, 3) ||
      !validInteger(seat, 0, players - 1) || !validInteger(seed, 0, 65535) ||
      !validInteger(difficulty, 0, 4) || !validInteger(delay, 0, 8) ||
      !Array.isArray(loadouts) || loadouts.length !== players ||
      loadouts.some(row => !validInteger(row?.character, 0, 2) || !validInteger(row?.shot, 0, 1)) ||
      typeof challenge !== 'boolean') throw new Error('Unsupported THIBP room settings');
  if (typeof options.netplayUrl !== 'string' || options.netplayUrl.length > 2048) {
    throw new Error('Invalid THIBP relay address');
  }
  const url = new URL(options.netplayUrl), query = url.searchParams;
  const room = query.get('room'), run = query.get('run');
  const role = spectator ? 'spectator' : 'player', otherRole = spectator ? 'player' : 'spectator';
  if (!['ws:', 'wss:'].includes(url.protocol) || url.username || url.password || url.hash ||
      !/^thibpmp-\d{4,8}$/.test(room ?? '') || !/^[1-9]\d{0,9}$/.test(run ?? '') ||
      query.get(role) !== (spectator || String(seat)) || !memberId.test(query.get('member') ?? '') ||
      ['room', 'run', 'member', role].some(key => query.getAll(key).length !== 1) ||
      [otherRole, 'signal', 'lobby', 'directory'].some(key => query.has(key))) {
    throw new Error('Invalid THIBP room membership');
  }
  const playerTypes = loadouts.map(({character, shot}) => character * 2 + shot);
  return {url: url.href, room, run, seat, players, seed, difficulty, delay, playerTypes, challenge, spectator, predictionLimit};
}

const keyBits = Object.freeze({
  ArrowLeft: 1 << 0, ArrowRight: 1 << 1, ArrowUp: 1 << 2, ArrowDown: 1 << 3,
  KeyZ: (1 << 4) | (1 << 6), Enter: 1 << 4, KeyX: (1 << 5) | (1 << 7),
  ShiftLeft: 1 << 8, ShiftRight: 1 << 8, KeyC: 1 << 9, KeyV: 1 << 10, Escape: 1 << 11, KeyR: 1 << 12,
});
const encoder = new TextEncoder();
const interval = 1000 / 60;
const forwardLimit = 12;

export function createRoomInput() {
  const held = new Set();
  let tapped = 0;
  return {
    key(code, down) {
      if (!Object.hasOwn(keyBits, code)) return;
      if (down && !held.has(code)) {
        held.add(code);
        tapped |= keyBits[code];
      } else if (!down) {
        held.delete(code);
      }
    },
    clear() {
      held.clear();
      tapped = 0;
    },
    focusHeld() {
      return held.has('ShiftLeft') || held.has('ShiftRight');
    },
    sample() {
      let bits = tapped;
      for (const code of held) bits |= keyBits[code];
      tapped = 0;
      return bits;
    },
  };
}

export async function startRoom({game, config, options, contentHash, input, onStart, onFinish, onDisconnect, onError, onStatus, onPlayback, prediction}) {
  const identity = encoder.encode(JSON.stringify([
    'thibp-coop/7', contentHash, config.room, config.run, config.players,
    config.seed, config.difficulty, config.delay, config.playerTypes, config.challenge,
  ]));
  const digest = new DataView(await crypto.subtle.digest('SHA-256', identity));
  const wire = await createWire({eaglerOptions: options});
  if (config.spectator) {
    return startViewer({game, wire, config, abi: digest.getUint32(8), onStart, onFinish, onError, onPlayback});
  }
  const types = config.playerTypes.reduce((packed, type, seat) => packed | (type << (seat * 3)), 0);
  const open = wire.ccall('thibp_room_open', 'number',
    ['string', 'number', 'number', 'number', 'number', 'number', 'number', 'number'],
    [config.url, config.seat, config.players, digest.getUint32(0), digest.getUint32(4),
      config.seed, digest.getUint32(8), config.delay]);
  if (!open) {
    wire._thibp_room_close();
    throw new Error('Could not open the author room transport');
  }

  let timer, closed = false, started = false, finished = false;
  let nextSample = 0, nextCapture = 0, nextFrame = 0;
  let lastPreviewTime = 0, lastStreamBoundary = -1;
  let lastPreview = null;
  let streamFailed = false;
  function publish(consumed) {
    if (config.seat !== 0 || streamFailed) return;
    const status = game._thibp_coop_stage_stream() / Uint32Array.BYTES_PER_ELEMENT;
    if (status) {
      const [generation, start, stage, one, two, three] = game.thibpHeapU32.subarray(status, status + 6);
      if (lastStreamBoundary !== start) {
        lastStreamBoundary = start;
        if (wire._thibp_stream_stage(generation, start, stage, config.difficulty, types,
          Number(config.challenge), one, two, three) !== 1) streamFailed = true;
      }
    }
    if (!streamFailed && lastStreamBoundary >= 0 && wire._thibp_stream_publish(consumed, Number(finished)) < 0) {
      streamFailed = true;
    }
  }
  function preview(now) {
    if (closed || finished || !started || now - lastPreviewTime < interval - 1) return;
    const consumed = game._thibp_coop_frame();
    if (consumed < 0 || consumed !== nextFrame || consumed >= nextCapture + config.delay) return;
    const future = [];
    for (let frame = consumed; frame < consumed + config.predictionLimit && frame < nextCapture + config.delay; frame++) {
      const address = wire._thibp_room_forecast(frame);
      if (!address) break;
      const index = address / Uint32Array.BYTES_PER_ELEMENT;
      future.push(Array.from(wire.HEAPU32.subarray(index, index + 3)));
    }
    if (!future.length) return;
    if (lastPreview?.frame === consumed && lastPreview.future.length === future.length &&
        future.every((inputs, tick) => inputs.every((bits, seat) => bits === lastPreview.future[tick][seat]))) return;
    lastPreviewTime = now;
    try {
      if (prediction.render(game, consumed, future, now)) lastPreview = {frame: consumed, future};
    } catch (error) {
      prediction.observation.error = String(error);
      delete game.thibpPreviewFrame;
    }
  }
  if (prediction && config.predictionLimit) game.thibpPreviewFrame = preview;
  function close() {
    if (closed) return;
    closed = true;
    clearInterval(timer);
    delete game.thibpPreviewFrame;
    prediction?.close();
    wire._thibp_room_close();
    input.clear();
  }
  function pump() {
    try {
      if (!finished && started && game._thibp_coop_finished()) {
        finished = true;
        publish(game._thibp_coop_final_tick());
        input.clear();
        onFinish();
      }
      const status = wire._thibp_room_pump();
      if (status < 0) {
        const reason = finished ? '' : wire.UTF8ToString(wire._thibp_room_error()) ||
          (status === -2 ? 'THIBP peer disconnected' : 'THIBP connection failed');
        close();
        if (!finished) onDisconnect(reason);
        return;
      }
      if (status === 0) return;
      if (finished) {
        publish(game._thibp_coop_final_tick());
        return;
      }
      if (!started) {
        if (game._thibp_coop_configure_room(config.players, config.seed, config.difficulty, types,
          Number(config.challenge)) !== 1) {
          throw new Error('The native title could not start the THIBP room');
        }
        started = true;
        onStart();
      }
      const consumed = game._thibp_coop_frame();
      if (consumed < 0) return;
      publish(consumed);
      const now = performance.now();
      if (now - nextSample > 2 * interval) nextSample = now;
      if (now >= nextSample && nextCapture <= consumed + forwardLimit) {
        nextSample += interval;
        const local = game._thibp_coop_touch_sample(config.seat, input.sample());
        if (wire._thibp_room_capture(nextCapture, local) !== 1) {
          throw new Error('The author room refused a local input frame');
        }
        nextCapture++;
      }
      while (nextFrame < consumed + forwardLimit) {
        const address = wire._thibp_room_prepare(nextFrame);
        if (address === 0) break;
        if (address === 1) throw new Error('Invalid THIBP input for a remote seat');
        const start = address / Uint32Array.BYTES_PER_ELEMENT;
        const memory = wire.HEAPU32;
        if (game._thibp_coop_confirm_frame(nextFrame, memory[start], memory[start + 1], memory[start + 2]) !== 1 ||
            wire._thibp_room_commit(nextFrame) !== 1) throw new Error('The THIBP frame queue desynchronized');
        if (prediction) prediction.resolve(nextFrame, memory.subarray(start, start + 3));
        nextFrame++;
      }
      onStatus({seat: config.seat, captured: nextCapture, confirmed: nextFrame,
        path: globalThis.__eaglerNetplayTransport ?? null, streamFailed, prediction: prediction?.observation ?? null});
    } catch (error) {
      close();
      onError(error);
    }
  }
  timer = setInterval(pump, 4);
  return {close};
}

function startViewer({game, wire, config, abi, onStart, onFinish, onError, onPlayback}) {
  if (!wire.ccall('thibp_view_open', 'number', ['string', 'string', 'number', 'number'],
    [config.url, config.spectator, config.players, abi])) {
    wire._thibp_room_close();
    throw new Error('Could not open the author spectator transport');
  }

  let started = false, closed = false, finished = false, queuedThrough = 0, reportAt = 0;
  let report = null;
  function publishView() {
    const address = wire._thibp_view_state();
    report = address ? JSON.parse(wire.UTF8ToString(address)) : null;
    onPlayback?.(report);
  }
  function pump() {
    try {
      const status = wire._thibp_view_pump();
      if (status < 0) throw new Error('The THIBP spectator stream disconnected or sent a frame gap');
      if (!status) return;
      const consumed = game._thibp_coop_frame();
      const waitingToStart = started && consumed < 0 && !game._thibp_coop_finished();
      const ahead = Math.max(0, queuedThrough - Math.max(0, consumed));
      // A missing native stage consumes no timeline frames. The receiver
      // can still install a seek/restart checkpoint with zero frame budget.
      const size = waitingToStart ? 0 : Math.max(0, (report?.seeking ? 16 : 3) - ahead);
      const address = wire._thibp_view_step(size);
      if (!address) throw new Error('The THIBP spectator checkpoint could not be restored');
      const pos = address / Uint32Array.BYTES_PER_ELEMENT;
      const memory = wire.HEAPU32;
      const [reset, frames, fast, from, seed, difficulty, challenge, types, one, two, three] =
        memory.subarray(pos, pos + 11);
      if (reset) {
        if (game._thibp_coop_configure_viewer(config.players, seed, difficulty, types,
          challenge, one, two, three, Number(started)) !== 1) {
          throw new Error('The native title could not restore the THIBP spectator run');
        }
        if (!started) onStart();
        started = true;
        finished = false;
        queuedThrough = 0;
      }
      game.thibpViewFastSteps = fast && !finished ? Math.min(12, frames + ahead) : 0;
      for (let offset = 0; offset < frames; ++offset) {
        const start = pos + 12 + offset * 3;
        if (game._thibp_coop_confirm_frame(from + offset, memory[start], memory[start + 1], memory[start + 2]) !== 1) {
          throw new Error('The native THIBP spectator queue desynchronized');
        }
        queuedThrough = from + offset + 1;
      }
      if (game._thibp_coop_finished() && !finished) {
        finished = true;
        onFinish();
      }
      const now = performance.now();
      if (now >= reportAt) {
        reportAt = now + 200;
        publishView();
      }
    } catch (error) {
      close();
      onError(error);
    }
  }
  const timer = setInterval(pump, 8);
  function close() {
    if (closed) return;
    closed = true;
    clearInterval(timer);
    wire._thibp_room_close();
    game.thibpViewFastSteps = 0;
    onPlayback?.(null);
  }
  function control({run, action, frame = 0}) {
    if (closed || !report || run !== report.run || !['pause', 'play', 'seek', 'live'].includes(action) ||
      !validInteger(frame, 0, 0xfffffffe) ||
      !wire.ccall('thibp_view_control', 'number', ['string', 'string', 'number'], [run, action, frame])) {
      throw new Error('The THIBP spectator position is unavailable');
    }
    publishView();
    return {};
  }
  return {close, control};
}
