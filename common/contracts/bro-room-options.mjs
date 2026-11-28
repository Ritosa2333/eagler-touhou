// BossRush (third-party derivative) multiplayer room options.
//
// BRO has no authored difficulty ladder, but the shared room UI needs a
// difficulty slot; Vanilla/Legacy are the two playable tiers (see
// BRO_MULTIPLAYER_DIFFICULTIES in product-catalog.mjs; index 2/3 are the
// 天空璋AEX / Bad Apple!! standalone chapter entries, not queue stages). This
// module carries the two BRO-only room arrays that the host chooses before a
// match:
//
//   broSystems     - the single system loadout every player carries (1..11)
//   broStageQueue  - an ordered subset of chapters to play (levels 1..19)
//
// Both are deliberately outside the native replay stream: a queued run is a
// launcher-driven sequence of stages, not a single native BossRush session.
// Every product boundary gates on the `bromp-` room prefix so other titles are
// never affected by these fields.

export const BRO_SYSTEM_COUNT = 11;
export const BRO_STAGE_COUNT = 19;

// System ids follow SystemList in game/THlib/UI/menu.lua (1..11).
export const BRO_SYSTEMS = Object.freeze([
  Object.freeze({ id: 1, name: "\u5996\u5996\u68a6\u6a31\u82b1\u7cfb\u7edf" }),
  Object.freeze({ id: 2, name: "\u6c38\u591c\u6284\u4eba\u6001\u7cfb\u7edf" }),
  Object.freeze({ id: 3, name: "\u82b1\u6620\u51a2\u84c4\u529b\u7cfb\u7edf" }),
  Object.freeze({ id: 4, name: "\u98ce\u795e\u5f55\u4fe1\u4ef0\u7cfb\u7edf" }),
  Object.freeze({ id: 5, name: "\u5730\u7075\u6bbf\u4fe1\u53f7\u7cfb\u7edf" }),
  Object.freeze({ id: 6, name: "\u661f\u83b2\u8239\u98de\u789f\u7cfb\u7edf" }),
  Object.freeze({ id: 7, name: "\u795e\u7075\u5e99\u7075\u754c\u7cfb\u7edf" }),
  Object.freeze({ id: 8, name: "\u8f89\u9488\u57ce\u9053\u5177\u7cfb\u7edf" }),
  Object.freeze({ id: 9, name: "\u7ec0\u73e0\u4f20\u64e6\u5f39\u7cfb\u7edf" }),
  Object.freeze({ id: 10, name: "\u5929\u7a7a\u748b\u5b63\u8282\u7cfb\u7edf" }),
  Object.freeze({ id: 11, name: "\u9b3c\u5f62\u517d\u66b4\u8d70\u7cfb\u7edf" }),
]);

// Chapter levels follow stage_selects in game/mod/main_stage.lua (1..19).
// `id` is the stage level, `stage` is the stageName suffix used by
// BRO_enter_stage ("TH<stage>@BossRush").
export const BRO_STAGES = Object.freeze([
  Object.freeze({ level: 1, stage: "06", name: "\u7ea2\u9b54\u4e61" }),
  Object.freeze({ level: 2, stage: "07", name: "\u5996\u5996\u68a6" }),
  Object.freeze({ level: 3, stage: "08", name: "\u6c38\u591c\u6284" }),
  Object.freeze({ level: 4, stage: "09", name: "\u82b1\u6620\u51a2" }),
  Object.freeze({ level: 5, stage: "095", name: "\u6587\u82b1\u5e16" }),
  Object.freeze({ level: 6, stage: "10", name: "\u98ce\u795e\u5f55" }),
  Object.freeze({ level: 7, stage: "11", name: "\u5730\u7075\u6bbf" }),
  Object.freeze({ level: 8, stage: "12", name: "\u661f\u83b2\u8239" }),
  Object.freeze({ level: 9, stage: "125", name: "\u6587\u82b1\u5e16DS" }),
  Object.freeze({ level: 10, stage: "128", name: "\u5927\u6218\u4e89" }),
  Object.freeze({ level: 11, stage: "13", name: "\u795e\u7075\u5e99" }),
  Object.freeze({ level: 12, stage: "14", name: "\u8f89\u9488\u57ce" }),
  Object.freeze({ level: 13, stage: "143", name: "\u5929\u90aa\u9b3c" }),
  Object.freeze({ level: 14, stage: "15", name: "\u7ec0\u73e0\u4f20" }),
  Object.freeze({ level: 15, stage: "16", name: "\u5929\u7a7a\u748b" }),
  Object.freeze({ level: 16, stage: "165", name: "\u68a6\u9b47\u65e5\u8bb0" }),
  Object.freeze({ level: 17, stage: "17", name: "\u9b3c\u5f62\u517d" }),
  Object.freeze({ level: 18, stage: "18", name: "\u8679\u9f99\u6d1e" }),
  Object.freeze({ level: 19, stage: "185", name: "\u5f39\u5e55\u9ed1\u5e02" }),
]);

// Default: every system on (BossRush is a system-stacking game); empty queue
// (empty queue means the native full 19-chapter run, no launcher sequencing).
export const BRO_DEFAULT_SYSTEMS = Object.freeze(BRO_SYSTEMS.map(s => s.id));
export const BRO_DEFAULT_STAGE_QUEUE = Object.freeze([]);

function validIdArray(value, count) {
  return Array.isArray(value) &&
    value.length <= count &&
    value.every(n => Number.isInteger(n) && n >= 1 && n <= count);
}

/** Systems are a de-duplicated subset of 1..BRO_SYSTEM_COUNT. Order is irrelevant. */
export function validBroSystems(value) {
  if (value === undefined) return true;
  if (!validIdArray(value, BRO_SYSTEM_COUNT)) return false;
  return new Set(value).size === value.length;
}

/** Queue is an ordered (possibly repeating) list of chapter levels 1..BRO_STAGE_COUNT. */
export function validBroStageQueue(value) {
  return value === undefined || validIdArray(value, BRO_STAGE_COUNT);
}

export function normalizeBroSystems(value) {
  if (!validBroSystems(value)) return undefined;
  return value === undefined ? undefined : [...value].sort((a, b) => a - b);
}

export function normalizeBroStageQueue(value) {
  if (!validBroStageQueue(value)) return undefined;
  return value === undefined ? undefined : [...value];
}
