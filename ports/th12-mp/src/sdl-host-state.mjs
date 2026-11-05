// Numeric SDL host ABI. Read a new view each time: WASM memory may grow.
// Older Runtime builds retain their JSON diagnostic entry point.
export function readSdlHostState(core) {
  if (!core._th12_host_state_values) return JSON.parse(core.UTF8ToString(core._th12_host_state()));
  const pointer = core._th12_host_state_values();
  const memory = core.HEAPU8.buffer;
  if (!Number.isInteger(pointer) || pointer < 0 || pointer % 8 || pointer + 27 * 8 > memory.byteLength)
    throw Error('Invalid SDL host state address');
  const v = new Float64Array(memory, pointer, 27);
  if (v[0] !== 1 || v[1] !== 27) throw Error('Unsupported SDL host state ABI');
  return {scene:core.UTF8ToString(v[2]), kind:core.UTF8ToString(v[3]), ready:!!v[4], fps:v[5],
    menuSerial:v[6], menuReady:!!v[7], menuAge:v[8], selection:v[9], stage:v[10], character:v[11], shot:v[12],
    cursor:v[13], instance:`${v[10]}:${v[14]}`, gestureScope:String(v[10]), x:v[15], y:v[16], fast:v[17], slow:v[18],
    paused:!!v[19], captureSerial:v[20], drawCalls:v[21], vertices:v[22], practiceVertices:v[23], capturing:!!v[24],
    track:core.UTF8ToString(v[25]), seeking:!!v[26]};
}
