import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, readFileSync, rmSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join, resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {createOracleReportWriter} from './oracle-report-writer.mjs';

const hash = bytes => createHash('sha256').update(bytes).digest('hex');

test('oracle report writer binds target and evidence source, and reads candidate only for comparisons', () => {
  const owner = mkdtempSync(join(tmpdir(), 'oracle-report-owner-'));
  try {
    const targetPath = join(owner, 'target.json');
    const originalPath = join(owner, 'fixture.exe');
    const candidatePath = join(owner, 'candidate.wasm');
    const outputDirectory = join(owner, 'reports');
    const originalBytes = Buffer.from('accepted original executable');
    const target = {game: 'fixture', variant: 'normal', executable: 'fixture.exe', sha256: hash(originalBytes)};
    writeFileSync(originalPath, originalBytes);
    writeFileSync(targetPath, JSON.stringify(target));
    const writer = createOracleReportWriter({targetPath, candidateWasmPath: candidatePath, outputDirectory});

    const original = writer.write('original-check', {
      passed: true,
      target: {game: 'spoof'},
      evidenceSource: 'candidate-comparison',
      coreSha256: 'spoofed-core-hash',
    }, 'original-only');
    assert.deepEqual(original.target, target);
    assert.equal(original.evidenceSource, 'original-only');
    assert.equal(original.targetExecutableSha256, hash(originalBytes));
    assert.equal(Object.hasOwn(original, 'coreSha256'), false);
    assert.equal(JSON.parse(readFileSync(join(outputDirectory, 'original-check.json'), 'utf8')).evidenceSource, 'original-only');
    original.target.game = 'mutated-return-value';
    assert.deepEqual(writer.create({passed: true}, 'original-only').target, target);

    assert.throws(() => writer.create({passed: true}, 'other'), /Unknown oracle evidence source/);
    assert.throws(() => writer.write('../escape', {passed: true}, 'original-only'), /simple filename stem/);
    assert.throws(() => writer.create({passed: true}), /candidate\.wasm/);

    writeFileSync(originalPath, 'changed executable');
    assert.throws(() => writer.create({passed: true}, 'original-only'), /SHA-256 mismatch/);

    const candidateBytes = Buffer.from([0, 4, 8, 15, 16, 23, 42]);
    writeFileSync(candidatePath, candidateBytes);
    const comparison = writer.write('candidate-check', {passed: true, coreSha256: 'spoofed'}, 'candidate-comparison');
    assert.equal(comparison.evidenceSource, 'candidate-comparison');
    assert.equal(comparison.coreSha256, hash(candidateBytes));
    assert.equal(Object.hasOwn(comparison, 'targetExecutableSha256'), false);

    const missingOwner = join(owner, 'missing-owner');
    const missingTarget = join(missingOwner, 'target.json');
    const missingOutput = join(missingOwner, 'reports');
    const missingCandidate = join(missingOwner, 'candidate.wasm');
    const missingBytes = Buffer.from('missing original');
    mkdirSync(missingOwner);
    writeFileSync(missingTarget, JSON.stringify({game: 'fixture', executable: 'missing.exe', sha256: hash(missingBytes)}));
    const missingWriter = createOracleReportWriter({targetPath: missingTarget, candidateWasmPath: missingCandidate, outputDirectory: missingOutput});
    assert.throws(() => missingWriter.create({passed: true}, 'original-only'), /missing\.exe/);

    const absoluteTarget = join(missingOwner, 'absolute-target.json');
    writeFileSync(absoluteTarget, JSON.stringify({game: 'fixture', executable: originalPath, sha256: hash(originalBytes)}));
    const absoluteWriter = createOracleReportWriter({targetPath: absoluteTarget, candidateWasmPath: missingCandidate, outputDirectory: missingOutput});
    assert.throws(() => absoluteWriter.create({passed: true}, 'original-only'), /relative target executable path/);
  } finally {
    rmSync(owner, {recursive: true, force: true});
  }
});

test('TH15 normal and MP helpers preserve original-only target identity and require absolute private input paths', async () => {
  for (const variant of ['normal', 'mp']) {
    const helperPath = variant === 'normal'
      ? '../../ports/th15/web/tests/cpp/helpers.mjs'
      : '../../ports/th15-mp/web/tests/cpp/helpers.mjs';
    const helper = await import(helperPath);
    const envelope = helper.reportRecord({
      passed: true,
      target: {game: 'spoof'},
      evidenceSource: 'candidate-comparison',
      coreSha256: 'spoofed',
    }, 'original-only');
    assert.deepEqual(envelope.target, helper.target, variant);
    assert.equal(envelope.evidenceSource, 'original-only', variant);
    assert.equal(Object.hasOwn(envelope, 'coreSha256'), false, variant);

    const envName = `ORACLE_INPUT_PATH_TEST_${variant.toUpperCase()}`;
    const oldValue = process.env[envName];
    try {
      delete process.env[envName];
      assert.throws(() => helper.privateOracleInput(envName), /private oracle input receipt/);
      process.env[envName] = 'relative/original.exe';
      assert.throws(() => helper.privateOracleInput(envName), /absolute receipt-bound path/);
      const absolutePath = resolve(tmpdir(), `${variant}-original.exe`);
      process.env[envName] = absolutePath;
      assert.equal(helper.privateOracleInput(envName), absolutePath);
    } finally {
      if (oldValue === undefined) delete process.env[envName];
      else process.env[envName] = oldValue;
    }
  }
});
