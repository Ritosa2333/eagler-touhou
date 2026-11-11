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

// SHA-256 of JSON.stringify(TH13_DATA_LAYOUT_SPEC), shared with browser code.
export const TH13_DATA_LAYOUT = 'sha256-9aceb48a8a4c87b1f3d916ae86c8788dee80e271ce42553f3246e699d91a7f18';

// TH13 BGM is one continuous Vorbis stream spanning all tracks addressed by thbgm.fmt.
export const TH13_MUSIC_FILES = Object.freeze([
  "thbgm.ogg",
]);
