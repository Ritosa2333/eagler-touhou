import assert from "node:assert/strict";
import test from "node:test";
import { describeReplay } from "../../wwwroot/host/replay/describe.mjs";

function plane(score) {
  const cols = Array.from({ length: 15 }, () => "0");
  cols[3] = String(score);
  return cols.join("\t");
}

function replay(lines) {
  const text = `\r\n${lines.join("\r\n")}\r\n`;
  const body = new TextEncoder().encode(text);
  const bytes = new Uint8Array(2 + body.length);
  bytes[0] = 0xf0;
  bytes[1] = 0x0f;
  bytes.set(body, 2);
  return bytes;
}

const header = [
  "ReplayInformation",
  "ver 1.00",
  "Alice",
  "26/01/02",
  "12:00",
  "Reimu",
  "A",
  "2",
];

test("a story replay lists the stages that follow its start", () => {
  const description = describeReplay(replay([
    ...header,
    "St2",
    "St3",
    "0",
    plane(10),
    plane(20),
    plane(30),
  ]));
  assert.equal(description.name, "Alice");
  assert.equal(description.character, 2);
  assert.equal(description.shot, 0);
  assert.equal(description.difficulty, 2);
  assert.deepEqual(description.stages, [
    { id: 2, label: "St2", score: 20 },
    { id: 3, label: "St3", score: 30 },
  ]);
});

test("touch and practice trailers stay out of the stage list", () => {
  const description = describeReplay(replay([
    ...header.slice(0, 5),
    "Koishi",
    "B",
    "4",
    "StEx",
    "StEx",
    "0",
    "TouchDelta=1",
    "Prac=1,0,0,0,2,2,400",
    plane(1),
    plane(99),
  ]));
  assert.equal(description.character, 5);
  assert.equal(description.shot, 1);
  assert.equal(description.difficulty, 4);
  assert.deepEqual(description.stages, [{ id: 7, label: "StEx", score: 99 }]);
});
