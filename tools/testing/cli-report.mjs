import {randomUUID} from 'node:crypto';
import {resolve} from 'node:path';
import {writeFileAtomic} from '../../launcher/lib/atomic-file.mjs';

// Replace the previous result before reading inputs or starting a browser.
export async function runWithReport({output, schema}, operation) {
  const report = {schema, runId: randomUUID(), startedAt: new Date().toISOString(), status: 'running', passed: false};
  const save = () => writeFileAtomic(resolve(output, 'report.json'), JSON.stringify(report, null, 2) + '\n');
  await save();
  try {
    await operation(report, save);
    report.status = report.passed === true ? 'passed' : 'failed';
  } catch (error) {
    report.status = 'failed'; report.passed = false;
    report.failure = String(error.stack || error);
  } finally {
    report.finishedAt = new Date().toISOString();
    await save();
  }
  return report;
}
