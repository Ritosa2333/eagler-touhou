// TH11 replay container note. Unlike th10/th20, TH11's original t11r replay
// already carries optional touch input in an embedded USER/T11T section
// (ReplayRecorder.cpp); the portable C++ layer renames a touch recording to
// `.rpyx` and validates the whole file in `th11_validate_file`. The shell
// therefore only needs a structural check plus extension passthrough here.
const MAGIC = 0x72313174; // "t11r" little-endian: 74 31 31 72.
export function validateMotionReplay(bytes) {
 if (!(bytes instanceof Uint8Array)) throw Error('录像数据无效');
 if (bytes.length < 0x24) return false;
 return new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength).getUint32(0, true) === MAGIC;
}
export function exportReplayName(name) { return name; }
export function importReplayName(name) { return name; }
