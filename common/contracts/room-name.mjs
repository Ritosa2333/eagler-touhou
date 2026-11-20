export const ROOM_NAME_MAX_LENGTH = 10;
const segments = typeof Intl.Segmenter === 'function' ? new Intl.Segmenter(undefined, {granularity: 'grapheme'}) : null;

export function normalizeRoomName(value) {
  if (typeof value !== 'string' || value.length > 128) return null;
  const text = value.normalize('NFC').trim();
  if (!text || /[\p{Cc}\p{Cf}]/u.test(text) || [...(segments ? segments.segment(text) : text)].length > ROOM_NAME_MAX_LENGTH) return null;
  return text;
}
