// Trailer layout matches Replay.ReadTitle: text after the 0xF00F marker.
// Stage ids match Game_Main.DescribeReplay (St1–St6 → 1–6, StEx → 7).

const characters = { Aya: 0, Plane: 1, Reimu: 2, Sanae: 3, Marisa: 4, Koishi: 5 };

function trailer(bytes) {
  const data = bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes || []);
  for (let i = 0; i + 6 < data.length; i++) {
    if (data[i] === 0xf0 && data[i + 1] === 0x0f && data[i + 2] === 0x0d && data[i + 3] === 0x0a
      && data[i + 4] === 0x52 && data[i + 5] === 0x65) {
      return new TextDecoder("gbk").decode(data.subarray(i + 2));
    }
  }
  return new TextDecoder("gbk").decode(data);
}

function scoreOf(value) {
  return Number.isFinite(value) && value >= 0 ? value : null;
}

export function describeReplay(bytes) {
  const lines = trailer(bytes).split(/\r?\n/);
  const index = lines.findIndex((line) => line === "ReplayInformation");
  if (index < 0 || lines[index + 1] !== "ver 1.00") throw Error("录像信息无效");
  const name = lines[index + 2] || "";
  const date = lines[index + 3] || "";
  const plane = lines[index + 5] || "";
  const weapon = lines[index + 6] || "";
  const difficulty = Number(lines[index + 7]);
  const start = lines[index + 8] || "";
  if (!Number.isInteger(difficulty) || !start) throw Error("录像信息无效");
  let row = index + 11;
  if (lines[row] === "TouchDelta=1") row += 1;
  if (typeof lines[row] === "string" && lines[row].startsWith("Prac=")) row += 1;
  const records = [];
  for (; row < lines.length; row++) {
    const cols = lines[row].split("\t");
    if (cols.length < 15) continue;
    records.push(Number(cols[3]));
  }
  const stages = [];
  if (start === "StEx") {
    if (records.length > 1) stages.push({ id: 7, label: "StEx", score: scoreOf(records[1]) });
  } else {
    const prefix = start.includes("St") ? "St" : "Bs";
    const startNum = Number(start.replace("St", "").replace("Bs", ""));
    if (!Number.isInteger(startNum)) throw Error("录像信息无效");
    for (let j = 0; j < records.length - 1; j++) {
      const menu = startNum - 1 + j;
      if (menu < 0 || menu >= 6) continue;
      stages.push({ id: menu + 1, label: prefix + (menu + 1), score: scoreOf(records[j + 1]) });
    }
  }
  if (!stages.length) throw Error("录像信息无效");
  const description = {
    name,
    date,
    character: Object.hasOwn(characters, plane) ? characters[plane] : 0,
    difficulty,
    stages,
  };
  if (weapon === "A" || weapon === "B") description.shot = weapon === "A" ? 0 : 1;
  return description;
}
