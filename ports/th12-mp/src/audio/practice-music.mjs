// TH12 ElBgmTest from src/practice/upstream/thprac_games.h.
// The TH12 hook always supplies the matching caller sentinel (0xffffffff).
// Commands identify native call sites, not every use of the audio transport:
// play=430183, stop=4226c7, pause=432820, resume=432982; all others=default.
let everlasting = false;
let queueLock = null;

// Native -1 means no lock. Stable string track IDs are also accepted by the
// browser host; numeric IDs retain the native >=0 test, including negative IDs.
function hasQueueLock() {
  return queueLock !== null &&
    (typeof queueLock !== 'number' || queueLock >= 0);
}

/** Return true to suppress the command before changing any music state.
 * enabled is the complete native hotkey predicate; retain is MODEFLAGS & 1
 * at the stop boundary, rather than the current session's practice mode.
 */
export function guard(command, {enabled, retain = false, track = null} = {}) {
  const bgmParam = track === -1 ? null : track;

  switch (command) {
    case 'play':
      if (queueLock === null)
        queueLock = bgmParam;
      if (queueLock !== bgmParam) {
        queueLock = null;
        everlasting = false;
      } else if (!everlasting && enabled) {
        everlasting = true;
        return false;
      }
      if (hasQueueLock() && queueLock !== bgmParam) {
        queueLock = null;
        everlasting = false;
      }
      break;
    case 'stop':
      if (hasQueueLock()) {
        queueLock = null;
        if (!retain || !enabled)
          everlasting = false;
      }
      break;
    case 'pause':
      if (hasQueueLock()) {
        if (enabled)
          everlasting = true;
        else
          everlasting = false;
      }
      break;
    case 'resume':
      if (hasQueueLock()) {
        if (!everlasting && enabled) {
          everlasting = true;
          return false;
        }
      }
      break;
    default:
      break;
  }

  return everlasting;
}

// Explicit session teardown only. Ordinary Retry must retain both fields until
// the native-equivalent stop/play commands update them above.
export function reset() {
  everlasting = false;
  queueLock = null;
}
