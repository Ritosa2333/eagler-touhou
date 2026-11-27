import assert from "node:assert/strict";
import test from "node:test";
import { createPracticeState, practiceCopy, practiceLocale } from "../../wwwroot/host/practice/practice-state.mjs";

test("practice locale follows the launcher and function keys toggle the declared tools", () => {
  assert.equal(practiceLocale("zh-Hans"), "zh-CN");
  assert.equal(practiceLocale("ja"), "zh-CN");
  assert.equal(practiceCopy("ja").title, "练习");
  assert.equal(practiceCopy("ja-JP").stage, "关卡");
  const practice = createPracticeState("en-US");
  practice.configure({ thpracEnabled: true, thpracLocale: "zh-CN" });
  assert.equal(practice.toggle("F1"), true);
  assert.equal(practice.toggle("F2"), true);
  assert.equal(practice.toggle("F3"), true);
  assert.equal(practice.toggle("Back"), true);
  assert.equal(practice.state.open, true);
  assert.equal(practice.state.invulnerable, true);
  assert.equal(practice.state.disableBomb, true);
  assert.equal(practice.state.ignoreFocusLoss, true);
  assert.equal(practice.copy().title, "练习");
  practice.configure({ thpracEnabled: false });
  assert.equal(practice.state.enabled, false);
  assert.equal(practice.state.open, false);
  assert.equal(practice.toggle("F1"), false);
});

test("starting resources stay inside the values the menu can apply", () => {
  const practice = createPracticeState();
  practice.configure({ thpracEnabled: true, thpracLocale: "en-US" });
  practice.state.life = 99;
  practice.state.power = -4;
  assert.deepEqual(practice.resources(), { life: 8, spell: 2, power: 0 });
});
