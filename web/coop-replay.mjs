const MAGIC = 0x35504d52; // RMP5
const VERSION = 1;
const STATE_BYTES = 360;
const INPUT_BYTES = 12;
const LIMIT = 32 * 1024 * 1024;

const viewOf = bytes => new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
const inRange = (n, min, max) => Number.isInteger(n) && n >= min && n <= max;

function checkpointInfo(state) {
  if (state.length !== STATE_BYTES) throw Error('Invalid TH05 replay checkpoint size');
  const view = viewOf(state);
  const count = view.getInt32(4, true), stage = view.getInt32(8, true);
  const difficulty = view.getInt32(16, true);
  const characters = Array.from({length: 3}, (_, seat) => view.getInt32(28 + seat * 4, true));
  const stageMagic = view.getUint32(0, true);
  if (![0x35535631, 0x35535632, 0x35535633].includes(stageMagic) || ![2, 3].includes(count) ||
      !inRange(stage, 0, 6) || !inRange(difficulty, 0, 4) || (stage === 6) !== (difficulty === 4) ||
      !characters.every(character => inRange(character, 0, 3))) throw Error('Invalid TH05 replay checkpoint');
  return {count, stage, difficulty, stageMagic, characters: characters.slice(0, count)};
}

export function inspectCoopReplay(bytes) {
  if (!(bytes instanceof Uint8Array) || bytes.length < 12 || bytes.length > LIMIT) throw Error('Invalid TH05 multiplayer replay size');
  const view = viewOf(bytes);
  if (view.getUint32(0, true) !== MAGIC || view.getUint32(4, true) !== VERSION) throw Error('Unsupported TH05 multiplayer replay');
  const count = view.getUint32(8, true);
  if (!inRange(count, 1, 7)) throw Error('Invalid TH05 replay stage count');
  const stages = [];
  let offset = 12, run;
  for (let index = 0; index < count; index++) {
    if (offset + 4 + STATE_BYTES > bytes.length) throw Error('Truncated TH05 replay checkpoint');
    const frames = view.getUint32(offset, true);
    const state = bytes.subarray(offset + 4, offset + 4 + STATE_BYTES);
    const info = checkpointInfo(state);
    const id = info.stage + 1;
    if (run && (id <= stages.at(-1).id || info.count !== run.count || info.difficulty !== run.difficulty || info.stageMagic !== run.stageMagic ||
        info.characters.some((character, seat) => character !== run.characters[seat]))) throw Error('Inconsistent TH05 replay stages');
    run ??= info;
    offset += 4 + STATE_BYTES;
    const length = frames * run.count * INPUT_BYTES;
    if (!frames || offset + length > bytes.length) throw Error('Invalid TH05 replay input length');
    const inputs = bytes.subarray(offset, offset + length);
    const inputView = viewOf(inputs);
    for (let at = 0; at < inputs.length; at += INPUT_BYTES) {
      if (inputs[at + 2] > 3 || inputs[at + 3] !== 0) throw Error('Invalid TH05 replay input mode');
      for (const axis of [4, 8]) {
        const coordinate = inputView.getFloat32(at + axis, true);
        if (Math.abs(coordinate) > 8192 || !Number.isInteger(coordinate * 256)) throw Error('Invalid TH05 replay coordinates');
      }
    }
    stages.push({id, frames, state, inputs});
    offset += length;
  }
  if (offset !== bytes.length) throw Error('Unexpected TH05 replay trailing data');
  return {stages, description: {
    format: 'th05-multiplayer', version: VERSION, name: run.count + 'P', difficulty: run.difficulty,
    character: run.characters[0], characters: run.characters, practice: false,
    stages: stages.map(({id, frames}) => ({id, frames, score: null, label: id === 7 ? 'EXTRA' : 'STAGE ' + id})),
  }};
}

export function createCoopRecorder({module, rows, count, save}) {
  const stride = count * INPUT_BYTES;
  let stages = [], stage, serial, frame = 0, size = 12, stopped = false;

  function checkpoint() {
    const length = module._th05_spectator_size();
    if (length !== STATE_BYTES) throw Error('TH05 replay checkpoint unavailable');
    const pointer = module._th05_spectator_state();
    const state = module.HEAPU8.slice(pointer, pointer + length);
    const info = checkpointInfo(state);
    if (stage && info.stage <= stage.id) finishSegment();
    if (size + 4 + STATE_BYTES >= LIMIT) { stop(); return; }
    stage = {id: info.stage, state, frames: 0, inputs: new Uint8Array(stride * 512)};
    stages.push(stage);
    size += 4 + STATE_BYTES;
    serial = module._th05_spectator_serial();
  }

  function finishSegment() {
    const complete = stages.filter(item => item.frames > 0);
    if (complete.length) {
      const length = complete.reduce((total, item) => total + 4 + STATE_BYTES + item.frames * stride, 12);
      const bytes = new Uint8Array(length), view = viewOf(bytes);
      view.setUint32(0, MAGIC, true);
      view.setUint32(4, VERSION, true);
      view.setUint32(8, complete.length, true);
      let offset = 12;
      for (const item of complete) {
        view.setUint32(offset, item.frames, true);
        bytes.set(item.state, offset + 4);
        offset += 4 + STATE_BYTES;
        bytes.set(item.inputs.subarray(0, item.frames * stride), offset);
        offset += item.frames * stride;
      }
      save(bytes);
    }
    stages = [];
    stage = null;
    size = 12;
  }

  function stop() {
    if (stopped) return;
    stopped = true;
    finishSegment();
  }

  return {
    begin: checkpoint,
    flush(confirmed) {
      if (stopped) return;
      while (frame <= confirmed) {
        if (size + stride > LIMIT) { stop(); return; }
        const offset = stage.frames * stride;
        if (offset + stride > stage.inputs.length) {
          const grown = new Uint8Array(Math.min(LIMIT, stage.inputs.length * 2));
          grown.set(stage.inputs);
          stage.inputs = grown;
        }
        const view = viewOf(stage.inputs);
        for (let seat = 0; seat < count; seat++) {
          const input = rows[seat].get(frame);
          if (!input) throw Error('Confirmed TH05 replay input unavailable');
          const at = offset + seat * INPUT_BYTES;
          view.setUint16(at, input[0], true);
          view.setUint8(at + 2, input[3]);
          view.setFloat32(at + 4, input[1], true);
          view.setFloat32(at + 8, input[2], true);
        }
        stage.frames++;
        frame++;
        size += stride;
      }
      if (serial !== module._th05_spectator_serial()) {
        if (frame !== module._th05_net_stat(5)) throw Error('Unconfirmed TH05 replay stage transition');
        checkpoint();
      }
    },
    stop,
  };
}

export function createCoopReplayDriver(module, read, applyDisplay) {
  let recording, path, selected, input, frame = 0;
  return {
    inspect(bytes) {
      return inspectCoopReplay(bytes).description;
    },
    open(file, stage, refresh) {
      const next = refresh || path !== file ? inspectCoopReplay(read(file)) : recording;
      const section = next?.stages.find(item => item.id === stage + 1);
      if (!section) return false;
      const pointer = module._malloc(STATE_BYTES);
      if (!pointer) return false;
      try {
        module.HEAPU8.set(section.state, pointer);
        if (!module._th05_spectator_load(pointer, STATE_BYTES)) return false;
      } finally { module._free(pointer); }
      recording = next;
      path = file;
      selected = section;
      input = viewOf(section.inputs);
      applyDisplay();
      frame = 0;
      return true;
    },
    step() {
      const seats = recording.description.characters.length;
      for (let seat = 0; seat < seats; seat++) {
        const at = (frame * seats + seat) * INPUT_BYTES;
        module._th05_coop_input(seat, input.getUint16(at, true), input.getFloat32(at + 4, true),
          input.getFloat32(at + 8, true), input.getUint8(at + 2));
      }
      if (!module._th05_tick(0)) return false;
      frame++;
      return true;
    },
    frame: () => frame,
    stage: () => (selected?.id ?? 0) - 1,
  };
}
