#!/usr/bin/env node
import { deployStaticSite } from "../lib/static-deployment.mjs";
const args = new Map(process.argv.slice(2).map(argument => {
  const match = /^--(source|releases|current)=(.+)$/.exec(argument);
  if (!match) throw new Error("usage: node scripts/deploy-static-site.mjs --source=PREPARED_SITE --releases=RELEASE_STORE --current=WEBROOT_SYMLINK");
  return [match[1], match[2]];
}));
if (args.size !== 3 || process.argv.length !== 5) throw new Error("Specify each of --source, --releases and --current exactly once");
console.log(JSON.stringify(await deployStaticSite(Object.fromEntries(args)), null, 2));
