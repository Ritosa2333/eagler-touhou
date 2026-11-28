import 'dart:io';
import 'dart:isolate';
import 'dart:typed_data';

import 'package:archive/archive_io.dart';
import 'package:crypto/crypto.dart';

import 'th09_replay_match.dart' show th09ReplayMetadataLimit;

const storedFileLimit = 64 * 1024 * 1024;
const replayArchiveLimit = 128 * 1024 * 1024;
const saveArchiveLimit = 256 * 1024 * 1024;
const checkpointLimit = 100 * 1024 * 1024 + 96;

bool isCheckpointPath(String path) =>
    RegExp(r'^autosave/save[0-3]_[0-4]\.dat$').hasMatch(path.toLowerCase());

bool isGameFilePath(String path) {
  final reserved = RegExp(
    r'^(CON|PRN|AUX|NUL|COM[1-9¹²³]|LPT[1-9¹²³])(\.|$)',
    caseSensitive: false,
  );
  return path.length <= 240 &&
      path
          .split('/')
          .every(
            (part) =>
                part.isNotEmpty &&
                !part.endsWith('.') &&
                !part.endsWith(' ') &&
                !RegExp(r'[\\<>:"|?*\x00-\x1f\x7f]').hasMatch(part) &&
                !reserved.hasMatch(part),
          );
}

enum GameFileIssue {
  tooManyLocalSaves,
  cannotReadLocal,
  noLocalSaveFolder,
  noRouteHint,
  cloudSaveTooLarge,
  cloudSaveEmptyOrLarge,
  incompleteSave,
  replaySlotsFull,
  replayUnsupported,
  rollbackFailed,
  replayMissing,
  nameTaken,
  keepReplayExtension,
  replayNameRequired,
  saveNamesInvalid,
  tooManySelectedSaves,
  exportTooLarge,
  importTooLarge,
  wrongGameFormat,
  invalidPath,
  importNameConflict,
  wrongReplayName,
  importEmptyOrLarge,
  cloudFileEmptyOrLarge,
  incompleteFile,
  cannotReadReplayName,
  cannotReadArchive,
  cannotReadCloudArchive,
  archiveSpecialFile,
  archiveInvalidPath,
  archiveDuplicateFile,
  archiveTooManyFiles,
  archiveSizeInvalid,
  archiveMetadataMismatch,
  archiveNoUsableFiles,
  restoreCancelled,
  cloudSaveWrongFileFormat,
  cloudMainSaveMissing,
  cloudManifestMissing,
  cloudChecksumFailed,
  cloudArchiveIncomplete,
  cloudManifestMismatch,
  cloudWrongGameOrLimit,
  gameNotInCatalog,
}

class GameFileError implements Exception {
  const GameFileError(this.issue);
  final GameFileIssue issue;

  @override
  String toString() => 'GameFileError(${issue.name})';
}

Future<Map<String, String>> extractGameZip({
  required String source,
  required String staging,
  required List<String> replayExtensions,
  String? score,
  bool replayMatchMetadata = false,
}) => Isolate.run(
  () => _extract(
    source: source,
    staging: staging,
    replayExtensions: replayExtensions,
    score: score,
    replayMatchMetadata: replayMatchMetadata,
  ),
);

Map<String, String> _extract({
  required String source,
  required String staging,
  required List<String> replayExtensions,
  required String? score,
  required bool replayMatchMetadata,
}) {
  final input = InputFileStream(source)..byteOrder = ByteOrder.littleEndian;
  try {
    final zip = ZipDirectory();
    final found = <String, String>{};
    final seen = <String>{};
    var total = 0;
    final maxTotal = score == null ? replayArchiveLimit : saveArchiveLimit;
    for (final header in zip.headers(input, verify: true)) {
      final path = header.filename;
      final directory = path.endsWith('/');
      final validPath = directory ? path.substring(0, path.length - 1) : path;
      if (!isGameFilePath(validPath)) {
        {
          throw const GameFileError(GameFileIssue.archiveInvalidPath);
        }
      }
      final lower = validPath.toLowerCase();
      if (!seen.add(lower)) {
        throw const GameFileError(GameFileIssue.archiveDuplicateFile);
      }
      if (seen.length > 4096) {
        throw const GameFileError(GameFileIssue.archiveTooManyFiles);
      }
      if (directory) continue;
      final extension = lower.split('.').last;
      final sidecar =
          replayMatchMetadata &&
          RegExp(r'(^|/)th9_(?:\d{2}|ud[0-9a-f]{4})\.json$').hasMatch(lower);
      final checkpoint = score != null && isCheckpointPath(lower);
      if (score == null
          ? !sidecar && !replayExtensions.contains(extension)
          : lower != score.toLowerCase() && !checkpoint) {
        continue;
      }
      final kind = (header.externalFileAttributes >> 16) & 0xf000;
      if (kind != 0 && kind != 0x8000) {
        {
          throw const GameFileError(GameFileIssue.archiveSpecialFile);
        }
      }
      final limit = sidecar
          ? th09ReplayMetadataLimit
          : checkpoint
          ? checkpointLimit
          : storedFileLimit;
      final size = header.uncompressedSize;
      if (size <= 0 || size > limit || total + size > maxTotal) {
        {
          throw const GameFileError(GameFileIssue.archiveSizeInvalid);
        }
      }
      total += size;
      final entry = header.file!;
      if (entry.uncompressedSize != size) {
        {
          throw const GameFileError(GameFileIssue.archiveMetadataMismatch);
        }
      }
      entry.verify = true;
      entry.throwOnError = true;
      final name = 'expanded-${found.length}';
      final target = File.fromUri(Directory(staging).uri.resolve(name));
      final output = OutputFileStream(target.path);
      try {
        entry.decompress(output);
      } finally {
        output.closeSync();
      }
      found[path] = target.path;
    }
    if (found.isEmpty ||
        (replayMatchMetadata &&
            found.keys.every(
              (path) => !replayExtensions.contains(
                path.split('.').last.toLowerCase(),
              ),
            ))) {
      throw const GameFileError(GameFileIssue.archiveNoUsableFiles);
    }
    return found;
  } on ArchiveException {
    throw const GameFileError(GameFileIssue.cannotReadArchive);
  } finally {
    input.closeSync();
  }
}

Future<Map<String, Uint8List>> readVerifiedSaveZip(
  Uint8List bytes,
  Map<String, ({int size, String sha})> manifest,
) {
  if (manifest.isEmpty) {
    throw const GameFileError(GameFileIssue.cloudManifestMissing);
  }
  return Isolate.run(() {
    final input = InputMemoryStream(bytes)..byteOrder = ByteOrder.littleEndian;
    try {
      final result = <String, Uint8List>{};
      for (final header in ZipDirectory().headers(input, verify: true)) {
        final name = header.filename;
        final expected = manifest[name];
        final kind = (header.externalFileAttributes >> 16) & 0xf000;
        if (expected == null ||
            result.containsKey(name) ||
            expected.size != header.uncompressedSize ||
            kind != 0 && kind != 0x8000) {
          throw const GameFileError(GameFileIssue.cloudManifestMismatch);
        }
        final entry = header.file!;
        if (entry.uncompressedSize != expected.size || entry.flags & 1 != 0) {
          {
            throw const GameFileError(GameFileIssue.cloudSaveWrongFileFormat);
          }
        }
        entry.verify = true;
        entry.throwOnError = true;
        final output = OutputMemoryStream(size: expected.size);
        entry.decompress(output);
        final decoded = output.getBytes();
        if (decoded.length != expected.size ||
            sha256.convert(decoded).toString() != expected.sha) {
          throw const GameFileError(GameFileIssue.cloudChecksumFailed);
        }
        result[name] = decoded;
      }
      if (result.length != manifest.length) {
        {
          throw const GameFileError(GameFileIssue.cloudArchiveIncomplete);
        }
      }
      return result;
    } on ArchiveException {
      throw const GameFileError(GameFileIssue.cannotReadCloudArchive);
    }
  });
}

Future<void> writeGameZip({
  required String output,
  required Map<String, String> files,
}) => Isolate.run(() {
  final encoder = ZipFileEncoder()..create(output, level: 1);
  try {
    for (final entry in files.entries) {
      encoder.addFileSync(File(entry.value), entry.key);
    }
  } finally {
    encoder.closeSync();
  }
});
