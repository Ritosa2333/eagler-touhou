#!/usr/bin/env node
import { PRODUCT_GAMES } from "../lib/contracts/product-catalog.mjs";
import { createAdapterContractReport } from "../lib/adapter-contract-report.mjs";

const args = Object.fromEntries(process.argv.slice(2).map(value => {
  const split = value.indexOf("=");
  if (!value.startsWith("--") || split < 3) throw new Error(`invalid argument: ${value}`);
  return [value.slice(2, split), value.slice(split + 1)];
}));
const selected = args.game ? [args.game] : Object.keys(PRODUCT_GAMES);
const reports = selected.map(createAdapterContractReport);
console.log(JSON.stringify(args.game ? reports[0] : reports, null, 2));
