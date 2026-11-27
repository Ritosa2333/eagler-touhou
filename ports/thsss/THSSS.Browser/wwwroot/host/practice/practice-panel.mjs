import { PRACTICE_STAGES, createPracticeState } from "./practice-state.mjs";
import { keyNameFromEvent } from "../../directory-keyboard.mjs";

const STYLE = `
.thsss-prac{position:fixed;left:12px;top:12px;z-index:4;width:220px;color:#f4f0e6;background:rgba(12,14,18,.92);font:13px/1.35 "Iowan Old Style","Palatino Linotype",Palatino,serif;padding:10px 12px}
.thsss-prac[hidden]{display:none}
.thsss-prac button,.thsss-prac label{display:block;width:100%;margin:4px 0;color:inherit;background:transparent;border:0;text-align:left;font:inherit}
.thsss-prac button[aria-pressed="true"]{text-decoration:underline}
.thsss-prac :focus-visible{outline:2px solid currentColor;outline-offset:2px}
.thsss-prac .row{display:flex;gap:6px;align-items:center}
.thsss-prac input{width:4.5em;background:#111;color:inherit;border:1px solid #888}
.thsss-prac .stages{display:flex;flex-wrap:wrap;gap:4px}
.thsss-prac .stages button{width:auto;margin:0;padding:2px 6px}
`;

function stageLabel(copy, id, mark) {
  if (id.startsWith("Bs")) return copy.boss + " " + mark;
  return copy.stage + " " + mark;
}

export function mountPracticePanel({ parent, sync, warp }) {
  const model = createPracticeState();
  const style = document.createElement("style");
  style.textContent = STYLE;
  const root = document.createElement("div");
  root.className = "thsss-prac";
  root.hidden = true;
  parent.append(style, root);
  const controls = () => [...root.querySelectorAll("button,input")];

  function push() {
    if (typeof sync !== "function") return;
    const resources = model.resources();
    sync({
      enabled: model.state.enabled,
      invulnerable: model.state.invulnerable,
      disableBomb: model.state.disableBomb,
      ignoreFocusLoss: model.state.ignoreFocusLoss,
      life: resources.life,
      spell: resources.spell,
      power: resources.power,
    });
  }

  function focusGame() {
    const active = document.activeElement;
    if (active && root.contains(active) && typeof active.blur === "function") active.blur();
    root.inert = true;
    document.getElementById("game")?.focus();
  }

  function render() {
    const copy = model.copy();
    const on = model.state;
    const focused = controls().indexOf(document.activeElement);
    root.hidden = !(on.enabled && on.open);
    root.inert = root.hidden;
    root.replaceChildren();
    const title = document.createElement("div");
    title.textContent = copy.title;
    root.append(title);
    for (const [flag, label] of [
      ["invulnerable", copy.invulnerable],
      ["disableBomb", copy.disableBomb],
      ["ignoreFocusLoss", copy.ignoreFocusLoss],
    ]) {
      const button = document.createElement("button");
      button.type = "button";
      button.textContent = label;
      button.setAttribute("aria-pressed", on[flag] ? "true" : "false");
      button.addEventListener("click", () => {
        on[flag] = !on[flag];
        push();
        render();
      });
      root.append(button);
    }
    for (const [field, label, max] of [
      ["life", copy.life, 8],
      ["spell", copy.spell, 8],
      ["power", copy.power, 400],
    ]) {
      const row = document.createElement("label");
      row.className = "row";
      row.textContent = label;
      const input = document.createElement("input");
      input.type = "number";
      input.name = field;
      input.min = "0";
      input.max = String(max);
      input.value = String(on[field]);
      input.addEventListener("change", () => {
        on[field] = Number(input.value);
        model.resources();
        input.value = String(on[field]);
      });
      row.append(input);
      root.append(row);
    }
    const apply = document.createElement("button");
    apply.type = "button";
    apply.textContent = copy.apply;
    apply.addEventListener("click", () => {
      model.resources();
      push();
    });
    root.append(apply);
    const stages = document.createElement("div");
    stages.className = "stages";
    for (const [id, mark] of PRACTICE_STAGES) {
      const button = document.createElement("button");
      button.type = "button";
      button.textContent = stageLabel(copy, id, mark);
      button.addEventListener("click", () => {
        on.stage = id;
        model.resources();
        push();
        on.open = false;
        render();
        focusGame();
        if (typeof warp === "function") warp(id);
      });
      stages.append(button);
    }
    root.append(stages);
    if (!root.hidden && focused >= 0) controls()[focused]?.focus();
  }

  function key(name, down, repeat = false) {
    if (!down) return false;
    const wasOpen = !root.hidden;
    if (!repeat && model.toggle(name)) {
      push();
      render();
      if (!wasOpen && !root.hidden) {
        root.inert = false;
        controls()[0]?.focus();
      }
      if (wasOpen && root.hidden) focusGame();
      return true;
    }
    if (repeat && model.state.enabled && (name === "Back" || name === "F1" || name === "F2" || name === "F3")) return true;
    if (!wasOpen) return false;
    const items = controls();
    const index = items.indexOf(document.activeElement);
    const active = items[index];
    switch (name) {
      case "X":
      case "Escape":
        model.state.open = false;
        render();
        focusGame();
        return true;
      case "Z":
      case "Return":
        if (!repeat && active?.localName === "button") active.click();
        if (!repeat && active?.localName === "input") {
          model.state[active.name] = Number(active.value);
          model.resources();
          active.value = String(model.state[active.name]);
        }
        return true;
      case "Left":
      case "Right":
        if (active?.localName === "input") {
          if (name === "Left") active.stepDown();
          else active.stepUp();
          active.dispatchEvent(new Event("change"));
          return true;
        }
        break;
      case "Up":
      case "Down":
        break;
      default:
        return false;
    }
    const direction = name === "Up" || name === "Left" ? -1 : 1;
    const next = index < 0 ? 0 : (index + direction + items.length) % items.length;
    items[next]?.focus();
    return true;
  }

  root.addEventListener("keydown", (event) => {
    if (root.hidden) return;
    const name = keyNameFromEvent(event);
    if (name === "Back" || name === "X" || name === "Escape" || name === "Z" || name === "Return" || name === "Up" || name === "Down" || name === "Left" || name === "Right") {
      event.preventDefault();
    }
  });

  function pointer(message) {
    if (!model.state.enabled || !model.state.open || root.hidden) return;
    if (message.type !== "down" && message.type !== "click") return;
    const canvas = document.getElementById("game");
    const rect = canvas ? canvas.getBoundingClientRect() : { left: 0, top: 0, width: 640, height: 480 };
    const width = canvas && canvas.width ? canvas.width : 640;
    const height = canvas && canvas.height ? canvas.height : 480;
    const rawX = Number(message.x);
    const rawY = Number(message.y);
    const x = rawX >= 0 && rawX <= width ? rect.left + (rawX / width) * rect.width : rawX;
    const y = rawY >= 0 && rawY <= height ? rect.top + (rawY / height) * rect.height : rawY;
    const hit = document.elementFromPoint(x, y);
    if (hit && root.contains(hit) && typeof hit.click === "function") hit.click();
  }

  return {
    model,
    configure(options) {
      model.configure(options);
      push();
      render();
    },
    key,
    pointer,
    push,
  };
}
