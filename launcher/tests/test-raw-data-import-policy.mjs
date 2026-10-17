import assert from "node:assert/strict";

import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import {
  createRawDataImportPackageDescriptor,
  rawDataImportFileNames,
  rawDataImportHashMatches,
  rawDataImportMatchesFileName,
  rawDataImportSizeMatches,
} from "../.cache/build/browser/assets/launcher/raw-data-import.mjs";

const rawImportGames = Object.entries(PRODUCT_GAMES)
  .filter(([, product]) => "rawDataImport" in product.package && !!product.package.rawDataImport)
  .map(([game]) => game);

for (const game of rawImportGames) {
  const product = PRODUCT_GAMES[game];
  const declared = product.package.rawDataImport.fileNames;
  assert.deepEqual(rawDataImportFileNames(game), declared);
  for (const name of declared) {
    assert.equal(rawDataImportMatchesFileName(game, name), true);
    assert.equal(rawDataImportMatchesFileName(game, name.toUpperCase()), true,
      `${game}: retail DATA filename matching must remain case-insensitive`);
  }
}

for (const [game] of Object.entries(PRODUCT_GAMES)) {
  if (rawImportGames.includes(game)) continue;
  assert.deepEqual(rawDataImportFileNames(game), []);
  assert.equal(rawDataImportMatchesFileName(game, `${game}.dat`), false,
    `${game}: undeclared raw DATA import must not be inferred from a filename convention`);
}

const expected = {
  path: "games/th08/th08.data",
  bytes: 4,
  sha256: "a".repeat(64),
  version: `sha256-${"a".repeat(64)}`,
  layout: `sha256-${"b".repeat(64)}`,
};
assert.equal(rawDataImportSizeMatches(expected, 4), true);
assert.equal(rawDataImportSizeMatches(expected, 3), false);
assert.equal(rawDataImportHashMatches(expected, "A".repeat(64)), true);
assert.equal(rawDataImportHashMatches(expected, "b".repeat(64)), false);

const descriptor = createRawDataImportPackageDescriptor("th08", expected, "A".repeat(64));
assert.equal(descriptor.game, "th08");
assert.equal(descriptor.revision, `raw-${"a".repeat(16)}`);
assert.equal(descriptor.runtimeRequirement?.dataFile, PRODUCT_GAMES.th08.package.dataFileId);
assert.equal(descriptor.runtimeRequirement?.dataLayout, expected.layout);
assert.equal(descriptor.files[PRODUCT_GAMES.th08.package.dataFileId].target, PRODUCT_GAMES.th08.package.dataTarget);
assert.deepEqual(descriptor.base.files, [PRODUCT_GAMES.th08.package.dataFileId]);
assert.deepEqual(descriptor.components, {});
assert.throws(() => createRawDataImportPackageDescriptor("th10", expected, "a".repeat(64)), /not declared/);
assert.throws(() => createRawDataImportPackageDescriptor("th08", expected, "b".repeat(64)), /identity/);

console.log(JSON.stringify({
  rawDataImportPolicy: "PASS",
  games: rawImportGames,
  destination: "canonical Package Store",
}));
