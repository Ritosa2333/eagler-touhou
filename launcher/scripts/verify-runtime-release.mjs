import { resolve } from "node:path";
import { verifyRuntimeRelease } from "../lib/runtime-release.mjs";

const result = await verifyRuntimeRelease(resolve(process.argv[2] || "."));
console.log(JSON.stringify({ schema: result.schema, games: Object.keys(result.games), verified: true }));
