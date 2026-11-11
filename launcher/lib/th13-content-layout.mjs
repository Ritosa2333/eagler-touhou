import {createHash} from 'node:crypto';

// Mount contract for TH13's prepared asset container.
export const TH13_DATA_SCHEMA = "th13-prepared-content/1";
export const TH13_DATA_MAGIC = "TH13PK01";
export const TH13_DATA_LAYOUT_SPEC = Object.freeze({
  schema: TH13_DATA_SCHEMA,
  magic: TH13_DATA_MAGIC,
  header: "magic8+manifestBytesLE32+utf8JSON",
  files: "contiguous-relative-offsets",
  mount: "/assets",
  music: "/assets/thbgm.ogg",
});

export const TH13_DATA_LAYOUT = 'sha256-' + createHash('sha256').update(JSON.stringify(TH13_DATA_LAYOUT_SPEC)).digest('hex');

// TH13 BGM is one continuous Vorbis stream spanning all tracks addressed by thbgm.fmt.
export const TH13_MUSIC_FILES = Object.freeze([
  "thbgm.ogg",
]);
