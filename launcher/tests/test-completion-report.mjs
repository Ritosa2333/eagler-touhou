/** L3/module. Preconditions: completion report contract only. Mutation: none.
 * Proves exact fields and accepted state vocabulary. Does NOT prove any reported work. */
import assert from "node:assert/strict";
import {
  COMPLETION_FIELDS,
  COMPLETION_REPORT_SCHEMA,
  validateCompletionReport,
} from "../lib/completion-report.mjs";

const completion = Object.fromEntries(COMPLETION_FIELDS.map(field => [field, "pending"]));
completion.IMPLEMENTED = "yes";
completion.RELEASED = "no";
const valid = { schema: COMPLETION_REPORT_SCHEMA, steps: [], completion };
assert.equal(validateCompletionReport(valid), valid);
await assert.rejects(async () => validateCompletionReport({ ...valid, completion: { ...completion, EXTRA: "yes" } }), /fields/);
await assert.rejects(async () => validateCompletionReport({ ...valid, completion: { ...completion, RELEASED: "done" } }), /invalid completion state/);
await assert.rejects(async () => validateCompletionReport({ ...valid, completion: { ...completion, "HUMAN-ACCEPTED": undefined } }), /invalid completion state/);
console.log("Completion report contract: PASS");
