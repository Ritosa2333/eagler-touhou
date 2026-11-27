// Canonical retail TH16 (v1.00a) PCM layout used by the portable SDL3 adapter.
// Keep this independent of any Runtime implementation so host/self-host
// content preparation never needs the TH16 Wasm archive reader.
//
// The table follows thbgm.fmt's archive order (archive_offset ascending):
// th16_13 precedes th16_12, and the final track is a TH128 borrow that keeps
// its original OGG filename. offset/loop/length are bytes of 16-bit stereo
// 44.1 kHz PCM inside thbgm.dat (length = loop end; loop = loop start).
export const TH16_DATA_LAYOUT = "sha256-38794a78bf866050e545717c8e056f63982b3c11f6b276f1809387a6afd3af5c";
export const TH16_MUSIC_FILES = Object.freeze([
  "th16_01.ogg", "th16_02.ogg", "th16_03.ogg", "th16_04.ogg", "th16_05.ogg",
  "th16_06.ogg", "th16_07.ogg", "th16_08.ogg", "th16_09.ogg", "th16_10.ogg",
  "th16_11.ogg", "th16_13.ogg", "th16_12.ogg", "th16_14.ogg", "th16_15.ogg",
  "th16_16.ogg", "th16_17.ogg", "th128_08.ogg",
]);

export const TH16_MUSIC_LAYOUT = Object.freeze([
  { offset: 16, loop: 1174528, length: 15562752 },
  { offset: 15562768, loop: 633712, length: 18885832 },
  { offset: 34448600, loop: 1070848, length: 19214720 },
  { offset: 53663320, loop: 1124608, length: 20704640 },
  { offset: 74367960, loop: 2286080, length: 18132224 },
  { offset: 92500184, loop: 571888, length: 22818424 },
  { offset: 115318608, loop: 7488468, length: 23333564 },
  { offset: 138652172, loop: 844640, length: 29527840 },
  { offset: 168180012, loop: 979792, length: 18599504 },
  { offset: 186779516, loop: 1026560, length: 31159680 },
  { offset: 217939196, loop: 3568512, length: 24244736 },
  { offset: 242183932, loop: 2349680, length: 18632336 },
  { offset: 260816268, loop: 1159520, length: 33597984 },
  { offset: 294414252, loop: 5175040, length: 10191872 },
  { offset: 304606124, loop: 9743488, length: 14056832 },
  { offset: 318662956, loop: 588800, length: 31469568 },
  { offset: 350132524, loop: 1133056, length: 32003072 },
  { offset: 382135596, loop: 1879240, length: 6741184 },
].map(Object.freeze));
