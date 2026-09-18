/** L3/workspace integration. Reads the explicitly configured upstream thcrap
 * repository and proves that repository-owned localization IDs still match it. */
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";

import { workspacePath } from "../lib/workspace-layout.mjs";
import { validateAsciiContract } from "../server/thcrap-ascii-contract.mjs";
import { validateStringContract } from "../server/thcrap-string-contract.mjs";

for (const [game, version] of [["th06", "v1.02h"], ["th07", "v1.00b"], ["th08", "v1.00d"]]) {
  const path = workspacePath("dependencies", "upstream-thcrap-tsa", "base_tsa", game, `stringlocs.${version}.js`);
  const stringlocs = JSON.parse(await readFile(path, "utf8"));
  const ids = new Set(Object.values(stringlocs));
  for (const record of validateAsciiContract(game).records) {
    if (!record.lookupOnly) assert.ok(ids.has(record.id), `${game}: missing upstream ASCII id ${record.id}`);
  }
  for (const record of validateStringContract(game).records) {
    assert.ok(ids.has(record.id), `${game}: missing upstream string id ${record.id}`);
  }
  if (game === "th06") {
    assert.equal(stringlocs.Rx6c4c8, "th06_practice_format",
      "TH06 duplicate Rx6c4c8 must preserve the final JSON value");
  }
}

console.log(JSON.stringify({ thcrapUpstreamContract: "PASS", games: ["th06", "th07", "th08"] }));
