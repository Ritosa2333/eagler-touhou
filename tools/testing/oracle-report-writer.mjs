import {mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {basename, dirname, isAbsolute, resolve} from 'node:path';

const EVIDENCE_SOURCES = new Set(['original-only', 'candidate-comparison']);

function sha256(bytes) {
  return createHash('sha256').update(bytes).digest('hex');
}

function reportData(value) {
  if (!value || typeof value !== 'object' || Array.isArray(value)) {
    throw new TypeError('Oracle report data must be an object');
  }
  const copy = {...value};
  delete copy.target;
  delete copy.evidenceSource;
  delete copy.coreSha256;
  delete copy.targetExecutableSha256;
  return copy;
}

export function createOracleReportWriter({targetPath, candidateWasmPath, outputDirectory}) {
  const targetFile = resolve(targetPath);
  const candidateFile = resolve(candidateWasmPath);
  const outputRoot = resolve(outputDirectory);
  const target = JSON.parse(readFileSync(targetFile, 'utf8'));

  function verifyTargetExecutable() {
    if (typeof target.executable !== 'string' || !target.executable || isAbsolute(target.executable) ||
        typeof target.sha256 !== 'string' || !/^[a-f0-9]{64}$/i.test(target.sha256)) {
      throw new Error('Original-only report requires a relative target executable path and SHA-256 in target.json');
    }
    const targetExecutable = resolve(dirname(targetFile), target.executable);
    const actual = sha256(readFileSync(targetExecutable));
    if (actual.toLowerCase() !== target.sha256.toLowerCase()) {
      throw new Error(`Target executable SHA-256 mismatch: ${target.executable}`);
    }
    return actual;
  }

  function create(data, source = 'candidate-comparison') {
    if (!EVIDENCE_SOURCES.has(source)) {
      throw new TypeError(`Unknown oracle evidence source: ${source}`);
    }
    const envelope = {
      ...reportData(data),
      target: JSON.parse(JSON.stringify(target)),
      evidenceSource: source,
    };
    if (source === 'original-only') {
      envelope.targetExecutableSha256 = verifyTargetExecutable();
    } else {
      envelope.coreSha256 = sha256(readFileSync(candidateFile));
    }
    return envelope;
  }

  function write(name, data, source = 'candidate-comparison') {
    if (typeof name !== 'string' || !/^[a-z0-9][a-z0-9._-]*$/i.test(name) || basename(name) !== name) {
      throw new TypeError('Oracle report name must be a simple filename stem');
    }
    const envelope = create(data, source);
    mkdirSync(outputRoot, {recursive: true});
    writeFileSync(resolve(outputRoot, `${name}.json`), `${JSON.stringify(envelope, null, 2)}\n`);
    return envelope;
  }

  return Object.freeze({create, write});
}
