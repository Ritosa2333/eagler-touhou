export const COMPLETION_REPORT_SCHEMA = "eagler-touhou/completion-report/1";
export const COMPLETION_FIELDS = Object.freeze([
  "IMPLEMENTED",
  "BUILD-VERIFIED",
  "STRUCTURE-VERIFIED",
  "RUNTIME-VERIFIED",
  "SEMANTIC-VERIFIED",
  "HUMAN-ACCEPTED",
  "RELEASED",
]);

const VALUES = new Set(["yes", "no", "pending", "not-applicable"]);

export function validateCompletionReport(report) {
  if (!report || report.schema !== COMPLETION_REPORT_SCHEMA || !Array.isArray(report.steps) ||
      !report.completion || typeof report.completion !== "object") {
    throw new Error("invalid completion report");
  }
  const fields = Object.keys(report.completion);
  if (fields.length !== COMPLETION_FIELDS.length || COMPLETION_FIELDS.some(field => !fields.includes(field))) {
    throw new Error("completion report fields do not match the platform contract");
  }
  for (const field of COMPLETION_FIELDS) {
    if (!VALUES.has(report.completion[field])) throw new Error(`invalid completion state: ${field}`);
  }
  return report;
}
