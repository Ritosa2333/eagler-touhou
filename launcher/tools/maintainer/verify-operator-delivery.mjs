#!/usr/bin/env node
import { resolve } from "node:path";
import { verifyOperatorDelivery } from "./operator-delivery.mjs";

if (process.argv.length !== 3) {
  throw new Error("usage: node tools/maintainer/verify-operator-delivery.mjs OPERATOR_DIRECTORY");
}
console.log(JSON.stringify(await verifyOperatorDelivery(resolve(process.argv[2])), null, 2));
