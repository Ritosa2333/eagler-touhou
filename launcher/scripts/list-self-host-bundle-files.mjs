#!/usr/bin/env node
import { SELF_HOST_BUNDLE_COPY_RULES } from "../lib/self-host-bundle.mjs";

process.stdout.write(`${JSON.stringify(SELF_HOST_BUNDLE_COPY_RULES)}\n`);
