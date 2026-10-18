import { resolve } from "node:path";

import { verifyReleaseBundle } from "../lib/release-bundle-verifier.mjs";

const result = await verifyReleaseBundle(resolve(process.argv[2] || "."));
console.log(JSON.stringify({ valid: true, ...result }));
