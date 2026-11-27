// Native THSSS practice session. The launcher owns only the pre-launch switch
// and locale; live tools, stage warp, and starting resources stay here.

export const PRACTICE_STAGES = Object.freeze([
  ["St1", "1"],
  ["Bs1", "1"],
  ["St2", "2"],
  ["Bs2", "2"],
  ["St3", "3"],
  ["Bs3", "3"],
  ["St4", "4"],
  ["Bs4", "4"],
  ["St5", "5"],
  ["Bs5", "5"],
  ["St6", "6"],
  ["Bs6", "6"],
  ["StEx", "Ex"],
]);

const COPY = {
  "zh-CN": {
    title: "练习",
    invulnerable: "无敌",
    disableBomb: "禁用炸弹",
    ignoreFocusLoss: "失焦不暂停",
    life: "残机",
    spell: "炸弹",
    power: "Power",
    stage: "关卡",
    boss: "符卡",
    apply: "应用开局资源",
  },
  "ja-JP": {
    title: "練習",
    invulnerable: "無敵",
    disableBomb: "ボム無効",
    ignoreFocusLoss: "非アクティブでも止めない",
    life: "残機",
    spell: "ボム",
    power: "パワー",
    stage: "ステージ",
    boss: "スペル",
    apply: "初期資源を適用",
  },
  "en-US": {
    title: "Practice",
    invulnerable: "Invulnerable",
    disableBomb: "Disable bomb",
    ignoreFocusLoss: "Keep playing when unfocused",
    life: "Lives",
    spell: "Bombs",
    power: "Power",
    stage: "Stage",
    boss: "Spell",
    apply: "Apply starting resources",
  },
};

export function practiceLocale(value) {
  // The built-in script is Chinese. The launcher's no-pack language id is ja
  // and arrives as ja-JP; that id selects this title's original copy.
  return String(value || "") === "en-US" ? "en-US" : "zh-CN";
}

export function practiceCopy(locale) {
  return COPY[practiceLocale(locale)];
}

function clampInt(value, min, max, fallback) {
  const number = Number(value);
  if (!Number.isInteger(number)) return fallback;
  return Math.max(min, Math.min(max, number));
}

export function createPracticeState(locale = "zh-CN") {
  const state = {
    enabled: false,
    locale: practiceLocale(locale),
    open: false,
    invulnerable: false,
    disableBomb: false,
    ignoreFocusLoss: false,
    life: 2,
    spell: 2,
    power: 400,
    stage: "St1",
  };

  function configure(options) {
    const bag = options || {};
    state.enabled = bag.thpracEnabled === true;
    state.locale = practiceLocale(bag.thpracLocale || state.locale);
    if (!state.enabled) state.open = false;
  }

  function toggle(name) {
    if (!state.enabled) return false;
    if (name === "Back") {
      state.open = !state.open;
      return true;
    }
    if (name === "F1") state.invulnerable = !state.invulnerable;
    else if (name === "F2") state.disableBomb = !state.disableBomb;
    else if (name === "F3") state.ignoreFocusLoss = !state.ignoreFocusLoss;
    else return false;
    return true;
  }

  function resources() {
    state.life = clampInt(state.life, 0, 8, 2);
    state.spell = clampInt(state.spell, 0, 8, 2);
    state.power = clampInt(state.power, 0, 400, 400);
    return { life: state.life, spell: state.spell, power: state.power };
  }

  return { state, configure, toggle, resources, copy: () => practiceCopy(state.locale) };
}
