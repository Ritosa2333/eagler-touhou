import 'dart:convert';
import 'dart:io';
import 'dart:isolate';

import 'package:archive/archive_io.dart';
import 'package:crypto/crypto.dart';
import 'package:path_provider/path_provider.dart';

import 'catalog.dart';
import 'game_file_archive.dart' show isGameFilePath;
import 'game_file_picker.dart';
import 'site_manifest.dart';

enum DataImportError { filename, size, checksum, archive, wrongGame }

enum DataImportPhase { copying, extracting, verifying }

typedef DataImportProgress = ({DataImportPhase phase, int? percent});
typedef GamePackageReference = ({Uri address, String? hint});

class GameDataImportException implements Exception {
  const GameDataImportException(this.reason);
  final DataImportError reason;
}

class GameResources {
  GameResources({
    Uri? origin,
    Future<Directory> Function()? appDirectory,
    Future<GameFileInput?> Function()? picker,
    Future<GamePackageReference?> Function()? reference,
  }) : _appDirectory = appDirectory ?? getApplicationSupportDirectory,
       _picker = picker ?? (() => pickGameFile(const ['dat', 'data', 'zip'])),
       _reference =
           reference ??
           (() => _readReference(origin ?? Uri.https('touhouonline.com')));

  final Future<Directory> Function() _appDirectory;
  final Future<GameFileInput?> Function() _picker;
  final Future<GamePackageReference?> Function() _reference;

  Future<GameFileInput?> pick() => _picker();
  Future<GamePackageReference?> reference() => _reference();

  static Future<GamePackageReference?> _readReference(Uri origin) async {
    final client = HttpClient();
    try {
      final manifest = await readSiteManifest(
        origin,
        client,
        const Duration(seconds: 8),
      );
      final fallback = (manifest['shared'] as Map)['gameDataFallback'];
      if (fallback is! Map || fallback['url'] is! String) return null;
      final address = Uri.tryParse(fallback['url'] as String);
      if (address == null ||
          address.scheme != 'https' ||
          address.host.isEmpty ||
          address.userInfo.isNotEmpty) {
        return null;
      }
      return (
        address: address,
        hint: fallback['hint'] is String ? fallback['hint'] as String : null,
      );
    } finally {
      client.close(force: true);
    }
  }

  Future<File> _destination(GameData data) async {
    if (!RegExp(r'^[a-zA-Z0-9._-]+\.(data|dat)$').hasMatch(data.name) ||
        !RegExp(r'^[a-f0-9]{64}$').hasMatch(data.sha256) ||
        data.bytes <= 0) {
      throw const FormatException('Invalid installed game data descriptor');
    }
    final app = await _appDirectory();
    return File.fromUri(
      app.uri.resolve('game-data/${data.sha256}/${data.name}'),
    );
  }

  Future<File?> installed(GameData? data) async {
    if (data == null) return null;
    final file = await _destination(data);
    return await file.exists() && await file.length() == data.bytes
        ? file
        : null;
  }

  Future<File> importData(
    Game game,
    GameFileInput source, {
    required void Function(DataImportProgress) progress,
  }) async {
    final data = game.data;
    if (data == null) {
      throw const GameDataImportException(DataImportError.filename);
    }
    final name = source.name.toLowerCase();
    final zipped = name.endsWith('.zip');
    if (!zipped && name != data.name.toLowerCase()) {
      throw const GameDataImportException(DataImportError.filename);
    }
    final declaredLength = await source.length();
    final limit = zipped ? game.packageMaxBytes : data.bytes;
    if (declaredLength != null &&
        (declaredLength <= 0 ||
            declaredLength > limit ||
            !zipped && declaredLength != data.bytes)) {
      throw const GameDataImportException(DataImportError.size);
    }

    final target = await _destination(data);
    await target.parent.create(recursive: true);
    final transaction = await target.parent.createTemp('import-');
    final draft = File.fromUri(transaction.uri.resolve(data.name));
    final sourceFile = zipped
        ? File.fromUri(transaction.uri.resolve('package.zip'))
        : draft;
    try {
      final output = await sourceFile.open(mode: FileMode.writeOnly);
      var bytes = 0;
      int? previousPercent;
      final total = zipped ? declaredLength : data.bytes;
      progress((
        phase: DataImportPhase.copying,
        percent: total == null ? null : 0,
      ));
      try {
        await for (final chunk in source.read()) {
          bytes += chunk.length;
          if (bytes > limit ||
              declaredLength != null && bytes > declaredLength) {
            throw const GameDataImportException(DataImportError.size);
          }
          await output.writeFrom(chunk);
          final percent = total == null ? null : bytes * 100 ~/ total;
          if (percent != previousPercent) {
            previousPercent = percent;
            progress((phase: DataImportPhase.copying, percent: percent));
          }
        }
        await output.flush();
      } finally {
        await output.close();
      }
      if (bytes == 0 ||
          declaredLength != null && bytes != declaredLength ||
          !zipped && bytes != data.bytes) {
        throw const GameDataImportException(DataImportError.size);
      }
      if (zipped) {
        progress((phase: DataImportPhase.extracting, percent: null));
        final archivePath = sourceFile.path;
        final outputPath = draft.path;
        final gameId = game.id;
        await Isolate.run(
          () => _extractData(archivePath, outputPath, gameId, data),
        );
      }
      progress((phase: DataImportPhase.verifying, percent: null));
      if (!await _verifyData(draft.path, data.sha256)) {
        throw const GameDataImportException(DataImportError.checksum);
      }
      return await draft.rename(target.path);
    } finally {
      if (await draft.exists()) await draft.delete();
      if (zipped && await sourceFile.exists()) await sourceFile.delete();
      await transaction.delete();
    }
  }
}

const _invalidArchive = GameDataImportException(DataImportError.archive);

Future<bool> _verifyData(String path, String expected) => Isolate.run(() async {
  final digest = await sha256.bind(File(path).openRead()).first;
  return digest.toString() == expected;
});

void _extractData(String source, String target, String game, GameData data) {
  final archive = InputFileStream(source)..byteOrder = ByteOrder.littleEndian;
  try {
    final files = <String, ZipFileHeader>{};
    final names = <String>{};
    for (final header in ZipDirectory().headers(archive, verify: true)) {
      final path = header.filename;
      final directory = path.endsWith('/');
      final name = directory ? path.substring(0, path.length - 1) : path;
      if (!isGameFilePath(name) ||
          !names.add(name.toLowerCase()) ||
          names.length > 4096) {
        throw _invalidArchive;
      }
      if (!directory) files[path] = header;
    }

    final modern = files['package.json'];
    final definition = modern ?? files['manifest.json'];
    final meta = _verifiedZipFile(definition, maxBytes: 1024 * 1024);
    final content = OutputMemoryStream();
    meta.decompress(content);
    final manifest = jsonDecode(utf8.decode(content.getBytes()));
    if (manifest is! Map || manifest['game'] is! String) throw _invalidArchive;
    if (manifest['game'] != game) {
      throw const GameDataImportException(DataImportError.wrongGame);
    }
    final Object? declaration;
    final Object? path;
    if (modern != null) {
      final requirement = manifest['runtimeRequirement'];
      final declared = manifest['files'];
      if (manifest['schema'] != 'eagler-touhou/package/1' ||
          requirement is! Map ||
          requirement['protocol'] != 'eagler-touhou/1' ||
          requirement['target'] != game ||
          requirement['dataFile'] is! String ||
          declared is! Map) {
        throw _invalidArchive;
      }
      declaration = declared[requirement['dataFile']];
      if (declaration is! Map || declaration['target'] != '/${data.name}') {
        throw _invalidArchive;
      }
      path = declaration['source'];
    } else {
      if (manifest['schema'] != 'eagler-touhou/game-data-pack/1' &&
          manifest['schema'] != 'eagler-touhou/offline-game-pack/1') {
        throw _invalidArchive;
      }
      declaration = manifest['data'];
      if (declaration is! Map) throw _invalidArchive;
      path = declaration['path'];
      if (path != data.name) throw _invalidArchive;
    }
    final descriptor = declaration;
    if (path is! String || !isGameFilePath(path)) throw _invalidArchive;
    if (descriptor['bytes'] != data.bytes ||
        files[path]?.uncompressedSize != data.bytes) {
      if (!files.containsKey(path)) throw _invalidArchive;
      throw const GameDataImportException(DataImportError.size);
    }
    final declaredHash = descriptor['sha256'];
    if ((modern == null || declaredHash != null) &&
        (declaredHash is! String ||
            declaredHash.toLowerCase() != data.sha256)) {
      throw const GameDataImportException(DataImportError.checksum);
    }

    final file = _verifiedZipFile(files[path], maxBytes: data.bytes);
    final output = OutputFileStream(target);
    try {
      file.decompress(output);
    } finally {
      output.closeSync();
    }
  } on ArchiveException {
    throw _invalidArchive;
  } on FormatException {
    throw _invalidArchive;
  } finally {
    archive.closeSync();
  }
}

ZipFile _verifiedZipFile(ZipFileHeader? header, {required int maxBytes}) {
  if (header == null ||
      header.uncompressedSize <= 0 ||
      header.uncompressedSize > maxBytes ||
      header.generalPurposeBitFlag & 1 != 0 ||
      header.compressionMethod != 0 && header.compressionMethod != 8) {
    throw _invalidArchive;
  }
  final kind = (header.externalFileAttributes >> 16) & 0xf000;
  final file = header.file;
  if (kind != 0 && kind != 0x8000 ||
      file == null ||
      file.uncompressedSize != header.uncompressedSize) {
    throw _invalidArchive;
  }
  return file
    ..verify = true
    ..throwOnError = true;
}
